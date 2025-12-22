#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include "../../include/utilities/threadpool.h"

namespace kipl {
namespace utilities {

ThreadPool::ThreadPool(unsigned int numThreads) : 
        logger("ThreadPool"),
        stop(false),
        submitted_tasks(0UL),
        processed_tasks(0UL)
{
    std::ostringstream msg_global;
    msg_global<<"Starting thread pool with "<<numThreads<<" threads";
    logger.verbose(msg_global.str());

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(
            [this,i] {
                std::ostringstream msg;

                for (;;) 
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                                                [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;

                        if (this->tasks.empty()) // To avoid dry spinning 
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    msg.str("");
                    msg<<"Thread "<<i<<" starts new task";
                    logger.verbose(msg.str());
                    
                    task();
                    ++processed_tasks;
                    // Notify any waiters that a task finished
                    this->cv_completed.notify_all();
#if defined(__cpp_lib_atomic_wait) && (__cpp_lib_atomic_wait >= 201907)
                    processed_tasks.notify_all();
#endif
                }
            }
        );
    }
}

void ThreadPool::barrier()
{ 
#if defined(__cpp_lib_atomic_wait) && (__cpp_lib_atomic_wait >= 201907)
    // C++20: Wait using atomic wait/notify without holding a mutex.
    // Snapshot the target to provide "flush" semantics: wait for tasks
    // submitted up to the call time.
    const auto target = this->submitted_tasks.load(std::memory_order_acquire);
    for (;;) {
        auto done = this->processed_tasks.load(std::memory_order_acquire);
        if (done >= target) break;
        this->processed_tasks.wait(done, std::memory_order_relaxed);
    }
#else
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    // Wait until all submitted tasks are processed and the queue is empty.
    // Using a predicate makes this resilient to spurious wakeups.
    this->cv_completed.wait(lock, [this]{
        return this->processed_tasks.load(std::memory_order_acquire) ==
                   this->submitted_tasks.load(std::memory_order_acquire)
               && this->tasks.empty();
    });
#endif
}

size_t ThreadPool::pool_size()
{
    return threads.size();
}

size_t ThreadPool::queue_size()
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    
    return tasks.size();
}

size_t ThreadPool::tasks_processed()
{
    return processed_tasks;
}

size_t ThreadPool::tasks_submitted()
{
    return submitted_tasks;
}

ThreadPool::~ThreadPool() 
{
    logger.verbose("Tearing down the thread pool");
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &thread : threads)
        thread.join();
}

}}
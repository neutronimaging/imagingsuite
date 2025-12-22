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

                        // No extra sleep here; predicate on condition avoids spinning.

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
    // C++20: Stable snapshot loop to handle tasks that enqueue more tasks.
    for (;;) {
        const auto target = this->submitted_tasks.load(std::memory_order_acquire);
        for (;;) {
            auto done = this->processed_tasks.load(std::memory_order_acquire);
            if (done >= target) break;
            this->processed_tasks.wait(done, std::memory_order_relaxed);
        }
        // If no new tasks were submitted while waiting, we are quiescent up to target.
        if (this->submitted_tasks.load(std::memory_order_acquire) == target)
            break;
        // Otherwise, loop and wait for the new submissions as well.
    }
#else
    // Fallback: use condition variable with the same stable snapshot semantics.
    for (;;) {
        const auto target = this->submitted_tasks.load(std::memory_order_acquire);
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        this->cv_completed.wait(lock, [this, target]{
            return this->processed_tasks.load(std::memory_order_acquire) >= target;
        });
        // If no new tasks were submitted while we waited, we are done.
        if (this->submitted_tasks.load(std::memory_order_acquire) == target)
            break;
        // else release lock and retry with the new target
    }
#endif
}

void ThreadPool::barrier_flush()
{
#if defined(__cpp_lib_atomic_wait) && (__cpp_lib_atomic_wait >= 201907)
    // Wait only for tasks submitted up to call time.
    const auto target = this->submitted_tasks.load(std::memory_order_acquire);
    for (;;) {
        auto done = this->processed_tasks.load(std::memory_order_acquire);
        if (done >= target) break;
        this->processed_tasks.wait(done, std::memory_order_relaxed);
    }
#else
    // Fallback: wait with condition variable until processed >= target.
    const auto target = this->submitted_tasks.load(std::memory_order_acquire);
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    this->cv_completed.wait(lock, [this, target]{
        return this->processed_tasks.load(std::memory_order_acquire) >= target;
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
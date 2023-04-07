#include <iostream>
#include <sstream>
#include <thread>
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

                        // if (this->tasks.empty())
                        //     std::this_thread::yield();

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    msg.str("");
                    msg<<"Thread "<<i<<" starts new task";
                    logger.verbose(msg.str());
                    
                    task();
                    ++processed_tasks;
                }
            }
        );
    }
}

void ThreadPool::barrier()
{ 
    for (;;) 
    {
        if (processed_tasks != submitted_tasks)
            std::this_thread::yield();
        else
            return ;
    }
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
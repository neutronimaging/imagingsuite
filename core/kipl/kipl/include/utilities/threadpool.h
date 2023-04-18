//<LICENSE>

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "../kipl_global.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <functional>

#include "../logging/logger.h"

namespace kipl {
namespace utilities {

/// @brief A generic thread pool that processes submitted tasks in available threads 
class KIPLSHARED_EXPORT ThreadPool 
{
    kipl::logging::Logger logger; ///< Logger instance, the messaging is thread safe.

public:
    /// @brief C'tor to set up the thread pool with requested number of threads
    /// @param numThreads Number of threads in the pool. Ideally, this should be number of cores (use std::thread::hardware_concurrency()).
    ThreadPool(unsigned int numThreads); 

    /// @brief Submit tasks to the queue
    /// @tparam F lambda function template
    /// @param f The lambda containing the task.
    template<class F>
    void enqueue(F &&f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
            ++submitted_tasks;
        }
        condition.notify_one();
    }
    /// @brief Waits until all tasks have be processed by comparing submitted and processed.
    void barrier();

    /// @brief Tells how many threads the pool has
    /// @return the number of threads
    size_t pool_size();

    /// @brief Tells how many tasks remains. This is a continuously changing number.
    /// @return number of tasks in the queue
    size_t queue_size();

    /// @brief Tells how many tasks have been processed
    /// @return the number of processed tasks
    size_t tasks_processed();

    /// @brief Tells how many tasks have been submitted to the task queue
    /// @return the number of submitted tasks
    size_t tasks_submitted();

    ~ThreadPool();

private:
    std::vector<std::thread>           threads;
    std::queue<std::function<void() >> tasks;

    std::mutex queue_mutex;

    std::condition_variable condition;
    bool stop;
    std::atomic_size_t submitted_tasks;
    std::atomic_size_t processed_tasks;
};

}}

#endif
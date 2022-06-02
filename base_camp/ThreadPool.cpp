#include "ThreadPool.h"

ThreadPool::~ThreadPool()
{
    stop();
}

ThreadPool::ThreadPool(int threads) : shutdown_(false)
{
    // Create the specified number of threads
    threads_.reserve(threads);
    for (int i = 0; i < threads; ++i)
        threads_.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
}

void ThreadPool::doJob(std::function <void(void)> func)
{
    if (shutdown_)
    {
        throw std::runtime_error("Thread pool was shutdowned!");
    }

    // Place a job on the queu and unblock a thread
    std::unique_lock <std::mutex> l(lock_);

    jobs_.emplace(std::move(func));
    condVar_.notify_one();
}

void ThreadPool::stop()
{
    {
        // Unblock any threads and tell them to stop
        std::unique_lock <std::mutex> l(lock_);

        shutdown_ = true;
        condVar_.notify_all();
    }

    // Wait for all threads to stop
    //std::cerr << "Joining threads" << std::endl;
    for (auto& thread : threads_)
        thread.join();
}

bool ThreadPool::isWorking()
{
    return !jobs_.empty() || workingThreads_ > 0;
}

void ThreadPool::threadEntry(int i)
{
    std::function <void(void)> job;

    while (1)
    {
        {
            std::unique_lock <std::mutex> l(lock_);

            while (!shutdown_ && jobs_.empty())
                condVar_.wait(l);

            if (shutdown_)
            {
                // No jobs to do and we are shutting down
                //std::cerr << "Thread " << i << " terminates" << std::endl;
                return;
            }

            //std::cerr << "Thread " << i << " does a job" << std::endl;
            job = std::move(jobs_.front());
            ++workingThreads_;
            jobs_.pop();
        }

        // Do the job without holding any locks
        job();
        --workingThreads_;
    }
}
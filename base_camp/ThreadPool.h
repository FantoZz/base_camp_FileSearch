#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>
#include <exception>


class ThreadPool
{
public:

    ThreadPool(int threads);

    ~ThreadPool();

    void doJob(std::function <void(void)> func);

    void stop();

    bool isWorking();

protected:

    void threadEntry(int i);

    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void(void)>> jobs_;
    std::vector <std::thread> threads_;
    std::atomic_uint workingThreads_ = 0;
};


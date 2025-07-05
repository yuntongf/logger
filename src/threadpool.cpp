#include "threadpool.h"

ThreadPool::ThreadPool(std::size_t size) {
    exit_.store(false);
    stop_.store(true);
    for (std::size_t i = 0; i < size; ++i) {
        workers_.emplace_back([this]{
            Task job;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this]{
                    return exit_.load() || (!stop_.load() && !jobs_.empty());
                });
                if (exit_.load()) {
                    return;
                }
                job = std::move(jobs_.front());
                jobs_.pop();
            }
            job();
        });
    }
}

ThreadPool::~ThreadPool() {
    exit_.store(true);
    cv_.notify_all();
    for (auto& t : workers_) {
        t.join();
    }
}

void ThreadPool::resume() {
    if (stop_.load()) {
        stop_.store(false);
        cv_.notify_all();
    }
}

void ThreadPool::pause() {
    if (!stop_.load()) {
        stop_.store(true);
        cv_.notify_all();
    }
}

void ThreadPool::reset() {
    pause(); // is this needed?
    {
        std::unique_lock<std::mutex> lock(mtx_);
        while (!jobs_.empty()) {
            jobs_.pop();
        }
    }
    resume();
}
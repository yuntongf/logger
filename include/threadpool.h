#pragma once

#include <thread>
#include <future>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>
#include <cstddef>
#include <utility>
#include <future>

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool() = delete;

    ThreadPool(std::size_t size);

    ThreadPool(const ThreadPool& other) = delete;

    ThreadPool& operator=(const ThreadPool& other) = delete;

    ~ThreadPool();

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    void resume();

    void reset();

    void pause();

private:
    std::vector<std::thread> workers_;
    std::queue<Task> jobs_;
    std::condition_variable cv_;
    std::mutex mtx_;
    std::atomic<bool> stop_;
    std::atomic<bool> exit_;
};
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
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ret_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<ret_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto ret = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mtx_);
            jobs_.push([task]{(*task)();});
            cv_.notify_one();
        }
        return ret;
    }

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
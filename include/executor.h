#pragma once

#include <unordered_map>
#include <string_view>
#include <memory>
#include <stdexcept>
#include <future>

#include "threadpool.h"
#include "scheduler.h"

enum class ExecutorTag {
    FILE_WRITE_BACK
};

class Executor {
using Tag = ExecutorTag;
using Runner = ThreadPool;

public:
    Executor();

    Executor(const Executor&) = delete;
    
    Executor(Executor&&) = delete;

    ~Executor() = default;

    void addRunner(Tag tag);

    void stopRunner(Tag tag);

    template <typename F, typename... Args> 
    auto postTask(Tag tag, F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>> {
        using ret_type = std::invoke_result_t<F, Args...>;
        if (tag_to_runner_.find(tag) == tag_to_runner_.end()) {
            throw std::runtime_error("Runner does not exist!");
        }
        auto task = std::packaged_task<ret_type()>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        auto ret = task.get_future();
        tag_to_runner_[tag]->submit(std::move(task));
        return ret;
    }

    template <typename T, typename R, typename F, typename... Args> 
    auto postDelayedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    template <typename T, typename R, typename F, typename... Args> 
    Scheduler::ScheduledTask::TaskId postRepeatedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args);

    void cancelTask(Scheduler::ScheduledTask::TaskId id);

    void start();

    void reset();

private:
    std::unordered_map<Tag, std::unique_ptr<Runner>> tag_to_runner_;
    std::unique_ptr<Scheduler> scheduler_;
};
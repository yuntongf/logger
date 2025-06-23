#pragma once

#include <unordered_map>
#include <string_view>
#include <memory>
#include <stdexcept>
#include <future>

#include "threadpool.h"
#include "scheduler.h"

class Executor {
using Tag = std::string_view;
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
        -> std::future<std::invoke_result_t<F, Args...>>;

    template <typename T, typename R, typename F, typename... Args> 
    auto postDelayedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    template <typename T, typename R, typename F, typename... Args> 
    ScheduledTask::TaskId postRepeatedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args);

    void cancelTask(ScheduledTask::TaskId id);

    void start();

    void reset();

private:
    std::unordered_map<Tag, std::unique_ptr<Runner>> tag_to_runner_;
    std::unique_ptr<Scheduler> scheduler_;
};
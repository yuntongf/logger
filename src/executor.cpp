#include "executor.h"

Executor::Executor() {
    scheduler_ = std::make_unique<Scheduler>();
}

void Executor::addRunner(Tag tag) {
    tag_to_runner_[tag] = std::make_unique<Runner>(1);
}

void Executor::stopRunner(Tag tag) {
    if (tag_to_runner_.find(tag) != tag_to_runner_.end()) {
        tag_to_runner_[tag]->pause();
    }
}

template <typename T, typename R, typename F, typename... Args> 
auto Executor::postDelayedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
    using ret_type = std::future<std::invoke_result_t<F, Args...>>;
    if (tag_to_runner_.find(tag) == tag_to_runner_.end()) {
        throw std::runtime_error("Runner does not exist!");
    }
    auto task = std::packaged_task<ret_type()>(
        std::bind(postTask, this, tag, std::forward<F>(f), std::forward<Args>(args)...)
    );
    auto res = task.get_future();
    scheduler_->postDelayedTask(std::move(task), period);
    return res;
}

template <typename T, typename R, typename F, typename... Args> 
Scheduler::ScheduledTask::TaskId Executor::postRepeatedTask(Tag tag, std::chrono::duration<T, R> period, F&& f, Args&&... args) {
    if (tag_to_runner_.find(tag) == tag_to_runner_.end()) {
        throw std::runtime_error("Runner does not exist!");
    }
    auto task_id = scheduler_->postRepeatedTask(
        std::bind(postTask, this, tag, std::forward<F>(f), std::forward<Args>(args)...), 
        period
    );
    return task_id;
}

void Executor::cancelTask(Scheduler::ScheduledTask::TaskId id) {
    scheduler_->cancelTask(id);
}

void Executor::start() {
    scheduler_->start();
}

void Executor::reset() {
    scheduler_->reset();
    tag_to_runner_.clear();
}
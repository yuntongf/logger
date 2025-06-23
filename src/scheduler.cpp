#include "scheduler.h"

Scheduler::Scheduler() {
    pool_ = std::make_unique<ThreadPool>(1);
    task_id_ = 0;
}

Scheduler::~Scheduler() {
    reset();
}

template <typename T, typename R>
ScheduledTask::TaskId Scheduler::postRepeatedTask(ThreadPool::Task task, std::chrono::duration<T, R> period, bool repeated) {
    auto id = getNextTaskId();
    {
        std::unique_lock<std::mutex> lock(mtx_);
        task_pq_.emplace(
            id,
            Scheduler::Clock::now(),
            std::chrono::duration_cast<ScheduledTask::Delta>(period),
            std::move(task),
            true
        )
        cv_.notify_all();
    }
    return id;
}

template <typename T, typename R>
ScheduledTask::TaskId postDelayedTask(ThreadPool::Task task, std::chrono::duration<T, R> period) {
    return postRepeatedTask(task, period, false);
}

void Scheduler::cancelTask(ScheduledTask::TaskId id) {
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cancelled_task_ids_.insert(id);
    }
}

void Scheduler::start() {
    pool_->submit([this]{
        {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this]{return !task_pq_.empty();});
        
                auto task = task_pq_.top();
                task_pq_.pop();
                
                if (cancelled_task_ids_.find(task.id) != cancelled_task_ids_.end()) {
                    continue;
                }
                cv_.wait_until(lock, task.start_time + task.delta);
                // re-enqueue repeated tasks
                if (task.repeated) {
                    task_pq_.emplace(
                        task.id, 
                        task.start_time + task.delta, 
                        task.delta, 
                        task.task, 
                        true
                    );
                }
                task.task();
            }
        }
    });
}

void Scheduler::reset() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (!task_pq_.empty()) {
        task_pq_.pop();
    }
    cv_.notify_all();
}
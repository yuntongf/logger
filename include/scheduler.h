#pragma once
#include <chrono>
#include <cstddef>
#include <queue>
#include <memory>
#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "threadpool.h"

class Scheduler {
    using Clock = std::chrono::high_resolution_clock;
public:
    /* This class need not be thread-safe */
    struct ScheduledTask {
        using TimePoint = Clock::time_point;
        using Delta = Clock::duration;
        using TaskId = uint64_t;

        TaskId id;
        TimePoint start_time;
        Delta delta;
        ThreadPool::Task task;
        bool repeated;

        bool operator<(const ScheduledTask& other) const {
            return start_time + delta > other.start_time + other.delta;
        }
    };

    Scheduler();

    ~Scheduler();

    Scheduler(const Scheduler& other) = delete;

    Scheduler& operator=(const Scheduler& other) = delete;

    template <typename T, typename R>
    ScheduledTask::TaskId postRepeatedTask(ThreadPool::Task task, std::chrono::duration<T, R> period, bool repeated);
    
    template <typename T, typename R>
    ScheduledTask::TaskId postDelayedTask(ThreadPool::Task task, std::chrono::duration<T, R> period);

    void cancelTask(ScheduledTask::TaskId);

    void start();

    void reset();

private:
    ScheduledTask::TaskId getNextTaskId() {
        return task_id_++;
    }

private:
    std::priority_queue<ScheduledTask> task_pq_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::unique_ptr<ThreadPool> pool_;
    ScheduledTask::TaskId task_id_;
    std::unordered_set<ScheduledTask::TaskId> cancelled_task_ids_;
};
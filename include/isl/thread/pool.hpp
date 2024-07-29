#ifndef ISL_PROJECT_POOL_HPP
#define ISL_PROJECT_POOL_HPP

#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <isl/coroutine/task.hpp>
#include <isl/isl.hpp>
#include <isl/thread/lazy_list.hpp>
#include <isl/thread/spin_lock.hpp>
#include <stop_token>
#include <thread>

namespace isl::thread
{
    struct CoGetTaskId
    {
        Id taskId;

        [[nodiscard]] auto await_ready() const noexcept -> bool
        {
            return false;
        }

        [[nodiscard]] auto await_suspend(auto &&task) noexcept -> bool
        {
            taskId = task.promise().get_id();
            return false;
        }

        [[nodiscard]] auto await_resume() const noexcept -> Id
        {
            return taskId;
        }
    };

    class Pool
    {
    private:
        struct Job;
        using JobIterator = typename LazyList<Job>::iterator;

        struct Job
        {
            std::coroutine_handle<> handle;
            JobIterator parent;
            Id taskId;
            std::atomic<ssize_t> referencesCount;
            std::atomic_flag isRunning;
            std::atomic_flag isCompleted;

            auto operator==(const Job &other) const -> bool
            {
                return referencesCount == other.referencesCount;
            }

            auto operator<=>(const Job &other) const -> std::weak_ordering
            {
                return referencesCount <=> other.referencesCount;
            }
        };

        std::unordered_map<Id, JobIterator> handleToJob;
        LazyList<Job> tasks;
        LazyList<Job> readyTasks;

        std::vector<std::thread> threads;
        std::condition_variable hasNewTasks;
        std::mutex tasksMutex;
        std::atomic<bool> runFlag{true};

        template<typename T>
        class TaskFuture
        {
        private:
            friend Pool;
            using task_handle = typename Task<T>::coro_handle;

            Task<T> task;
            Pool *pool;
            JobIterator job;

            explicit TaskFuture(Task<T> created_task, Pool &p, Id parent_id)
              : task{std::move(created_task)}
              , pool{&p}
              , job{pool->submit(task.share_handle(), task.get_id(), parent_id)}
            {}

        public:
            [[nodiscard]] auto await_ready() const -> bool
            {
                return task.has_result();
            }

            auto await_suspend(std::coroutine_handle<> /* unused */) -> bool
            {
                return !task.done();
            }

            [[nodiscard]] auto await_resume() const -> decltype(auto)
            {
                pool->removeFromPool(job);
                return task.get();
            }
        };

    public:
        explicit Pool(std::size_t size);

        Pool(const Pool &) = delete;
        Pool(Pool &&) noexcept = delete;

        auto operator=(const Pool &) -> Pool & = delete;
        auto operator=(Pool &&) noexcept -> Pool & = delete;

        ~Pool();

        template<typename T>
        auto submit(Task<T> task, Id parent_id) -> TaskFuture<T>
        {
            return TaskFuture<T>{std::move(task), *this, parent_id};
        }

        auto stop() -> void;

        auto removeFromPool(JobIterator job) -> void;

    private:
        auto submit(coro::coroutine_handle<> task_handle, Id task_id, Id parent_id) -> JobIterator;

        auto doWork() -> void;

        auto worker(const std::atomic<bool> *run_flag) -> void;

        auto pickJob() -> JobIterator;

        auto getParentJob(Id parent_id) -> JobIterator;

        auto increaseParentsReferencesCount(JobIterator parent) -> void;

        auto onTaskDone(JobIterator job) -> void;

        auto decreaseParentsReferencesCount(JobIterator parent_job) -> void;
    };
}// namespace isl::thread

#endif /* ISL_PROJECT_POOL_HPP */

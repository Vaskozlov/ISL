#ifndef ISL_PROJECT_POOL_HPP
#define ISL_PROJECT_POOL_HPP

#include <condition_variable>
#include <isl/coroutine/task.hpp>
#include <isl/thread/spin_lock.hpp>
#include <mutex>
#include <thread>

namespace isl::thread
{
    class Pool
    {
    private:
        lock_free::Stack tasksWithoutDependencies;
        std::vector<std::thread> threads;
        std::mutex newTasksMutex;
        std::condition_variable hasNewTasks;
        std::atomic<bool> runFlag{true};

    public:
        template<typename T>
        class TaskFuture
        {
        private:
            friend Pool;
            using task_handle = typename Task<T>::coro_handle;

            Task<T> task;
            Job *job{task.get_job_ptr()};
            Pool *pool;

            explicit TaskFuture(Task<T> created_task, Pool &p)
              : task{std::move(created_task)}
              , pool{&p}
            {
                pool->submit(job);
            }

        public:
            auto await() const -> decltype(auto)
            {
                pool->await(job);
                return task.get();
            }

            [[nodiscard]] auto await_ready() const -> bool
            {
                return task.has_result();
            }

            auto await_suspend(auto &&parent) -> bool
            {
                auto has_completed = !job->isCompleted.test();

                job->parent = parent.promise().get_job_ptr();
                job->parent->referencesCount.fetch_add(!has_completed, std::memory_order_release);

                return !has_completed;
            }

            [[nodiscard]] auto await_resume() const -> decltype(auto)
            {
                return await();
            }
        };

        explicit Pool(std::size_t size);

        Pool(const Pool &) = delete;
        Pool(Pool &&) noexcept = delete;

        auto operator=(const Pool &) -> Pool & = delete;
        auto operator=(Pool &&) noexcept -> Pool & = delete;

        ~Pool();

        template<typename T>
        auto submit(Task<T> task) -> TaskFuture<T>
        {
            return TaskFuture<T>{std::move(task), *this};
        }

        auto stop() -> void;

        auto await(Job *job) -> void;

    private:
        auto submit(Job *job) -> void;

        auto runJob(Job *job) -> void;

        auto worker(const std::atomic<bool> *run_flag) -> void;

        auto pickJob() -> Job *;

        auto onTaskDone(Job *job) -> void;

        auto decreaseParentsReferencesCount(Job *parent_job) -> void;

        auto addJobToTheQueueUnique(Job *job) -> void;
    };
}// namespace isl::thread

namespace isl
{
    template<typename T>
    using AsyncTask = thread::Pool::TaskFuture<T>;
}

#endif /* ISL_PROJECT_POOL_HPP */

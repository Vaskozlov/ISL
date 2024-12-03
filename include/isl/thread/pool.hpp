#ifndef ISL_PROJECT_POOL_HPP
#define ISL_PROJECT_POOL_HPP

#include <condition_variable>
#include <isl/coroutine/task.hpp>
#include <isl/thread/spin_lock.hpp>
#include <list>
#include <mutex>
#include <thread>

namespace isl
{
    template<typename T>
    class AsyncTask;
}

namespace isl::thread
{
    class Pool
    {
    private:
        struct ThreadInfo
        {
            std::thread thread;
            std::atomic<bool> runFlag;

            template<typename... Ts>
            explicit ThreadInfo(const bool flag_value, Ts &&...args)
              : thread{std::forward<Ts>(args)...}
              , runFlag{flag_value}
            {}
        };

        lock_free::Stack tasksStack;
        std::mutex newTasksMutex;
        std::mutex threadsManipulationMutex;
        std::condition_variable hasNewTasks;
        std::list<ThreadInfo> threads;

    public:
        explicit Pool(std::size_t count);

        Pool(const Pool &) = delete;
        Pool(Pool &&) noexcept = delete;

        auto operator=(const Pool &) -> Pool & = delete;
        auto operator=(Pool &&) noexcept -> Pool & = delete;

        ~Pool();

        auto wereRunning() -> std::size_t;

        template<typename T>
        auto submit(Task<T> task) -> AsyncTask<T>
        {
            auto async_task = AsyncTask<T>{std::move(task), *this};
            submit(async_task.getJobPtr());

            return async_task;
        }

        auto startThreads(std::size_t count) -> void;

        auto stopOneThread() -> void;

        auto stopAllThreads() -> void;

        auto await(const Job *job) -> void;

    private:
        auto submit(Job *job) -> void;

        static auto runJob(Job *job) -> bool;

        auto worker(const std::atomic<bool> &run_flag) -> void;

        auto pickJob() -> Job *;

        auto waitForNotify() -> void;
    };
}// namespace isl::thread

namespace isl
{
    template<typename T>
    class AsyncTask
    {
    private:
        friend thread::Pool;
        using task_handle = typename Task<T>::coro_handle;

        Task<T> task;
        Job *job{task.get_job_ptr()};
        thread::Pool *pool;

        explicit AsyncTask(Task<T> created_task, thread::Pool &p)
          : task{std::move(created_task)}
          , pool{&p}
        {}

        [[nodiscard]] auto getJobPtr() const noexcept -> Job *
        {
            return job;
        }

    public:
        auto await() -> decltype(auto)
        {
            pool->await(job);
            return task.get();
        }

        auto await() const -> decltype(auto)
        {
            pool->await(job);
            return task.get();
        }

        [[nodiscard]] auto await_ready() const -> bool
        {
            return task.has_result();
        }

        [[nodiscard]] auto await_suspend(coro::coroutine_handle<> /* unused */) const noexcept
            -> bool
        {
            return false;
        }

        [[nodiscard]] auto await_resume() -> decltype(auto)
        {
            return await();
        }

        [[nodiscard]] auto await_resume() const -> decltype(auto)
        {
            return await();
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_POOL_HPP */

#ifndef ISL_PROJECT_POOL_HPP
#define ISL_PROJECT_POOL_HPP

#include <condition_variable>
#include <isl/coroutine/task.hpp>
#include <isl/thread/persistent_storage.hpp>
#include <isl/thread/spin_lock.hpp>
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
            explicit ThreadInfo(bool flag_value, Ts &&...args)
              : runFlag{flag_value}
              , thread{std::forward<Ts>(args)...}
            {}
        };

        using ThreadInfoIterator = PersistentStorage<ThreadInfo>::iterator;

        lock_free::Stack tasksStack;
        std::mutex newTasksMutex;
        std::mutex threadsManipulationMutex;
        std::condition_variable hasNewTasks;
        PersistentStorage<ThreadInfo> threads;

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

        auto await(Job *job) -> void;

    private:
        auto submit(Job *job) -> void;

        auto runJob(Job *job) -> void;

        auto worker(ThreadInfoIterator run_flag) -> void;

        auto pickJob() -> Job *;

        auto onTaskDone(Job *job) -> void;

        auto decreaseParentsReferencesCount(Job *parent_job) -> void;

        auto sendStopSignalToThread(ThreadInfoIterator thread_info_it) -> void;
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

        [[nodiscard]] auto getJobPtr() -> Job *
        {
            return job;
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

        [[nodiscard]] auto await_suspend(coro::coroutine_handle<> /* unused */) -> bool
        {
            return false;
        }

        [[nodiscard]] auto await_resume() const -> decltype(auto)
        {
            return await();
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_POOL_HPP */

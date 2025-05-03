#ifndef ISL_PROJECT_POOL_HPP
#define ISL_PROJECT_POOL_HPP

#include <condition_variable>
#include <isl/coroutine/task.hpp>
#include <list>
#include <thread>

namespace isl
{
    template <typename T>
    class AsyncTask;
} // namespace isl

namespace isl::thread
{
    class Pool
    {
    private:
        struct Thread
        {
            std::thread thread;
            std::atomic<bool> runFlag;

            template <typename... Ts>
            explicit Thread(const bool flag_value, Ts &&...args)
              : thread{std::forward<Ts>(args)...}
              , runFlag{flag_value}
            {}
        };

        lock_free::Stack tasksStack;
        mutable std::mutex newTasksMutex;
        mutable std::mutex threadsManipulationMutex;
        std::condition_variable hasNewTasks;
        std::list<Thread> threads;

    public:
        explicit Pool(std::size_t count);

        Pool(const Pool &) = delete;
        Pool(Pool &&) noexcept = delete;

        auto operator=(const Pool &) -> Pool & = delete;
        auto operator=(Pool &&) noexcept -> Pool & = delete;

        ~Pool();

        [[nodiscard]] auto wereRunning() const -> std::size_t;

        template <typename T>
        [[nodiscard]] auto async(Task<T> task) -> AsyncTask<T>
        {
            submit(task.get_job_ptr());
            return AsyncTask<T>{std::move(task), *this};
        }

        template <typename T>
        auto launch(Task<T> task) -> void
        {
            Job *job = task.release();
            job->shouldBeDestroyedByPool = true;
            submit(job);
        }

        auto startThreads(std::size_t count) -> void;

        auto stopOneThread() -> void;

        auto stopAllThreads() -> void;

        auto await(const Job *job) -> void;

        auto executeOneTask() -> void;

    private:
        auto submit(Job *job) -> void;

        static auto runJob(Job *job) -> bool;

        auto worker(const std::atomic<bool> &run_flag) -> void;

        auto pickJob() -> Job *;

        auto waitForNotify() -> void;
    };
} // namespace isl::thread

#endif /* ISL_PROJECT_POOL_HPP */

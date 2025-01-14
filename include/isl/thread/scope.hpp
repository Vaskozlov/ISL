#ifndef ISL_SCOPE_HPP
#define ISL_SCOPE_HPP

#include <isl/thread/pool.hpp>

namespace isl
{
    class Scope
    {
    private:
        std::atomic<std::size_t> totalJobsCount{0};
        std::atomic<std::size_t> completedJobsCount{0};

        thread::Pool *pool;

    public:
        explicit Scope(thread::Pool *thread_pool)
          : pool{thread_pool}
        {}

        ~Scope()
        {
            while (completedJobsCount.load(std::memory_order_relaxed)
                   != totalJobsCount.load(std::memory_order_relaxed)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        template <typename T>
        auto async(Task<T> task) -> AsyncTask<T>
        {
            Job *job = task.get_promise().get_job_ptr();
            job->scope = this;
            totalJobsCount.fetch_add(1, std::memory_order_relaxed);

            return pool->async(std::move(task));
        }

        template <typename T>
        auto launch(Task<T> task) -> void
        {
            Job *job = task.get_promise().get_job_ptr();
            job->scope = this;
            totalJobsCount.fetch_add(1, std::memory_order_relaxed);
            pool->launch(std::move(task));
        }

        auto onJobCompleted() -> void
        {
            completedJobsCount.fetch_add(1, std::memory_order_relaxed);
        }
    };
} // namespace isl

#endif /* ISL_SCOPE_HPP */

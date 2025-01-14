#ifndef ASYNC_TASK_HPP
#define ASYNC_TASK_HPP

#include <isl/thread/pool.hpp>

namespace isl
{
    template <typename T>
    class AsyncTask
    {
    private:
        friend thread::Pool;
        using task_handle = typename Task<T>::coro_handle;

        Task<T> task;
        Job *job{task.get_job_ptr()};
        thread::Pool *pool;

    public:
        explicit AsyncTask(Task<T> created_task, thread::Pool &p)
          : task{std::move(created_task)}
          , pool{std::addressof(p)}
        {}

        ~AsyncTask()
        {
            if (job == nullptr) {
                return;
            }

            if (job->scope != nullptr) {
            }
        }

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
} // namespace isl

#endif /* ASYNC_TASK_HPP */

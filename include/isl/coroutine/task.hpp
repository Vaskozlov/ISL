#ifndef ISL_PROJECT_TASK_HPP
#define ISL_PROJECT_TASK_HPP

#include <isl/coroutine/defines.hpp>
#include <isl/thread/id_generator.hpp>
#include <isl/thread/lockfree/stack.hpp>
#include <thread>

namespace isl
{
    struct Job : public thread::lock_free::StackNode
    {
        std::coroutine_handle<> handle;
        Job *parent;
        std::atomic<std::size_t> referencesCount{};
        std::atomic_flag isRunning{};
        std::atomic_flag isCompleted{};
        std::atomic_flag isInQueue{};
    };

    template<typename T = void>
    class Task
    {
    public:
        class promise_type;
        friend promise_type;
        using coro_handle = coro::coroutine_handle<promise_type>;

    private:
        coro_handle handle;

    public:
        explicit Task(coro_handle initial_handle)
          : handle{initial_handle}
        {}

        Task(const Task &) = delete;

        Task(Task &&other) noexcept
          : handle{other.handle}
        {
            other.handle = nullptr;
        }

        ~Task()
        {
            if (handle != nullptr) {
                handle.destroy();
            }
        }

        auto operator=(const Task &) -> Task & = delete;

        auto operator=(Task &&other) noexcept -> Task &
        {
            std::swap(handle, other.handle);
            return *this;
        }

        [[nodiscard]] auto await_ready() const noexcept -> bool
        {
            return has_result();
        }

        [[nodiscard]] auto await_suspend(coro::coroutine_handle<> handle) noexcept -> bool
        {
            return !handle.done();
        }

        [[nodiscard]] auto await_resume() const noexcept -> decltype(auto)
        {
            while (!handle.done()) {
                handle.resume();
            }

            return handle.promise().get_value();
        }

        auto resume() -> void
        {
            handle.resume();
        }

        [[nodiscard]] auto get() const -> decltype(auto)
        {
            return handle.promise().get_value();
        }

        [[nodiscard]] auto get_id() const -> Id
        {
            return get_promise().get_id();
        }

        [[nodiscard]] auto get_job_ptr() -> Job *
        {
            return get_promise().get_job_ptr();
        }

        [[nodiscard]] auto get_job_ptr() const -> const Job *
        {
            return get_promise().get_job_ptr();
        }

        [[nodiscard]] auto has_result() const -> bool
        {
            return get_promise().has_result();
        }

        [[nodiscard]] auto share_handle() const noexcept -> coro_handle
        {
            return handle;
        }

        [[nodiscard]] auto get_promise() noexcept -> promise_type &
        {
            return handle.promise();
        }

        [[nodiscard]] auto get_promise() const noexcept -> const promise_type &
        {
            return handle.promise();
        }

        [[nodiscard]] auto done() const noexcept -> bool
        {
            return handle.done();
        }
    };

    class TaskPromiseBase
    {
    protected:
        std::exception_ptr exceptionPtr{nullptr};

    public:
        [[nodiscard]] auto initial_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        [[nodiscard]] auto final_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        [[nodiscard]] auto get_exception() const noexcept -> std::exception_ptr
        {
            return exceptionPtr;
        }

        auto unhandled_exception() -> void
        {
            exceptionPtr = std::current_exception();
        }
    };

    template<>
    class Task<void>::promise_type : public TaskPromiseBase
    {
    private:
        std::exception_ptr exceptionPtr{nullptr};
        bool hasCompleted{};

        Job job{
            .handle = coro_handle::from_promise(*this),
            .parent = nullptr,
        };

    public:
        auto return_void() noexcept -> void
        {
            hasCompleted = true;
        }

        [[nodiscard]] auto get_return_object() -> Task<void>
        {
            return Task<void>{coro_handle::from_promise(*this)};
        }

        [[nodiscard]] auto get_job_ptr() noexcept -> Job *
        {
            return &job;
        }

        [[nodiscard]] auto get_job_ptr() const noexcept -> const Job *
        {
            return &job;
        }

        auto get_value() const noexcept(false) ISL_LIFETIMEBOUND -> void
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            if (!hasCompleted) {
                throw std::runtime_error{"Task has not finished yet"};
            }
        }

        [[nodiscard]] auto has_result() const noexcept -> bool
        {
            return hasCompleted || (exceptionPtr != nullptr);
        }
    };

    template<typename T>
    class Task<T>::promise_type : public TaskPromiseBase
    {
    private:
        std::optional<T> value{std::nullopt};
        std::exception_ptr exceptionPtr{nullptr};

        Job job{
            .handle = coro_handle::from_promise(*this),
            .parent = nullptr,
        };

    public:
        auto return_value(T new_value) noexcept -> void
        {
            value = std::move(new_value);
        }

        [[nodiscard]] auto get_return_object() -> Task<T>
        {
            return Task<T>{coro_handle::from_promise(*this)};
        }

        [[nodiscard]] auto get_job_ptr() noexcept -> Job *
        {
            return &job;
        }

        [[nodiscard]] auto get_job_ptr() const noexcept -> const Job *
        {
            return &job;
        }

        [[nodiscard]] auto get_value() const noexcept(false) ISL_LIFETIMEBOUND -> const T &
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            if (!value.has_value()) {
                throw std::runtime_error{"Task has not finished yet"};
            }

            return *value;
        }

        [[nodiscard]] auto has_result() const noexcept -> bool
        {
            return value.has_value() || (exceptionPtr != nullptr);
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_TASK_HPP */

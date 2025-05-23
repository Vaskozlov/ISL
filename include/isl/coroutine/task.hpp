#ifndef ISL_PROJECT_TASK_HPP
#define ISL_PROJECT_TASK_HPP

#include <isl/coroutine/defines.hpp>
#include <isl/id_generator.hpp>
#include <isl/thread/lockfree/stack.hpp>

namespace isl
{
    class Scope;

    struct Job : public thread::lock_free::StackNode
    {
        std::coroutine_handle<> handle;
        Scope *scope{nullptr};
        std::atomic_flag isCompleted;
        bool shouldBeDestroyedByPool{false};

        auto run() const -> void
        {
            handle.resume();
        }
    };

    template <typename T = void>
    class Task
    {
    public:
        class promise_type;
        friend promise_type;
        using coro_handle = coro::coroutine_handle<promise_type>;

    private:
        coro_handle handle{nullptr};

    public:
        Task() = default;

        explicit Task(coro_handle initial_handle)
          : handle{initial_handle}
        {}

        Task(const Task &) = delete;

        Task(Task &&other) noexcept
          : handle{std::exchange(other.handle, nullptr)}
        {}

        ~Task()
        {
            std::atomic_thread_fence(std::memory_order_acquire);

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

        [[nodiscard]] auto await_suspend(coro::coroutine_handle<> /* unused */) noexcept -> bool
        {
            return !handle.done();
        }

        auto resume() const -> void
        {
            handle.resume();
        }

        [[nodiscard]] auto done() const noexcept -> bool
        {
            return handle.done();
        }

        auto await() const -> decltype(auto)
        {
            while (!done()) {
                resume();
            }

            return get();
        }

        [[nodiscard]] auto await_resume() const noexcept -> decltype(auto)
        {
            await();
            return handle.promise().get_value();
        }

        [[nodiscard]] auto get() -> decltype(auto)
        {
            return handle.promise().get_value();
        }

        [[nodiscard]] auto get() const -> decltype(auto)
        {
            return handle.promise().get_value();
        }

        [[nodiscard]] auto get_job_ptr() ISL_LIFETIMEBOUND -> Job *
        {
            return get_promise().get_job_ptr();
        }

        [[nodiscard]] auto get_job_ptr() const ISL_LIFETIMEBOUND -> const Job *
        {
            return get_promise().get_job_ptr();
        }

        [[nodiscard]] auto has_result() const -> bool
        {
            return get_promise().has_result();
        }

        [[nodiscard]] auto get_promise() noexcept ISL_LIFETIMEBOUND -> promise_type &
        {
            return handle.promise();
        }

        [[nodiscard]] auto get_promise() const noexcept ISL_LIFETIMEBOUND -> const promise_type &
        {
            return handle.promise();
        }

        [[nodiscard]] auto release() -> Job *
        {
            Job *job = get_promise().get_job_ptr();
            handle = nullptr;
            return job;
        }
    };

    template <>
    class Task<>::promise_type
    {
    private:
        Job job{.handle = coro_handle::from_promise(static_cast<promise_type &>(*this))};
        std::exception_ptr exceptionPtr{nullptr};
        std::atomic<bool> hasCompleted;

    public:
        [[nodiscard]] auto get_return_object() -> Task<>
        {
            return Task<>{coro_handle::from_promise(*this)};
        }

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

        [[nodiscard]] auto get_job_ptr() noexcept ISL_LIFETIMEBOUND -> Job *
        {
            return std::addressof(job);
        }

        [[nodiscard]] auto get_job_ptr() const noexcept ISL_LIFETIMEBOUND -> const Job *
        {
            return std::addressof(job);
        }

        auto return_void() noexcept -> void
        {
            hasCompleted.store(true, std::memory_order_release);
        }

        auto get_value() const noexcept(false) ISL_LIFETIMEBOUND -> void
        {
            if (get_exception() != nullptr) {
                std::rethrow_exception(get_exception());
            }

            if (!hasCompleted.load(std::memory_order_acquire)) {
                throw std::runtime_error{"Task has not finished yet"};
            }
        }

        [[nodiscard]] auto has_result() const noexcept -> bool
        {
            return hasCompleted.load(std::memory_order_acquire) || get_exception() != nullptr;
        }
    };

    template <typename T>
    class Task<T>::promise_type
    {
    private:
        Job job{.handle = coro_handle::from_promise(static_cast<promise_type &>(*this))};
        std::exception_ptr exceptionPtr{nullptr};
        std::optional<T> value{std::nullopt};

    public:
        [[nodiscard]] auto get_return_object() -> Task
        {
            return Task{coro_handle::from_promise(static_cast<promise_type &>(*this))};
        }

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

        [[nodiscard]] auto get_job_ptr() noexcept ISL_LIFETIMEBOUND -> Job *
        {
            return std::addressof(job);
        }

        [[nodiscard]] auto get_job_ptr() const noexcept ISL_LIFETIMEBOUND -> const Job *
        {
            return std::addressof(job);
        }

        auto return_value(T new_value) noexcept -> void
        {
            value = std::move(new_value);
        }

        [[nodiscard]] auto get_value() noexcept(false) ISL_LIFETIMEBOUND -> T &
        {
            if (get_exception() != nullptr) {
                std::rethrow_exception(get_exception());
            }

            std::atomic_thread_fence(std::memory_order_acquire);

            if (!value.has_value()) {
                throw std::runtime_error{"Task has not finished yet"};
            }

            return *value;
        }

        [[nodiscard]] auto get_value() const noexcept(false) ISL_LIFETIMEBOUND -> const T &
        {
            if (get_exception() != nullptr) {
                std::rethrow_exception(get_exception());
            }

            if (!value.has_value()) {
                throw std::runtime_error{"Task has not finished yet"};
            }

            return *value;
        }

        [[nodiscard]] auto has_result() const noexcept -> bool
        {
            return value.has_value() || get_exception() != nullptr;
        }
    };
} // namespace isl

#endif /* ISL_PROJECT_TASK_HPP */

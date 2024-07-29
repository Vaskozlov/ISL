#ifndef ISL_PROJECT_TASK_HPP
#define ISL_PROJECT_TASK_HPP

#include <isl/coroutine/defines.hpp>
#include <isl/thread/id_generator.hpp>
#include <thread>

#define ISL_GET_CURRENT_TASK_ID (co_await isl::Task<>::get_task_id)

namespace isl
{
    constinit thread::IdGenerator TaskIdGenerator{1};

    template<typename T = void>
    class Task
    {
    public:
        class promise_type;
        friend promise_type;
        using coro_handle = coro::coroutine_handle<promise_type>;

        static constexpr struct get_promise_tag
        {
        } get_task_id{};

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

    template<typename T>
    class Task<T>::promise_type
    {
    private:
        std::optional<T> value{std::nullopt};
        std::exception_ptr exceptionPtr{nullptr};
        Id taskId{TaskIdGenerator.next()};

    public:
        [[nodiscard]] auto initial_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        [[nodiscard]] auto final_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        auto return_value(T new_value) noexcept -> void
        {
            value = std::move(new_value);
        }

        [[nodiscard]] auto get_exception() const noexcept -> std::exception_ptr
        {
            return exceptionPtr;
        }

        auto unhandled_exception() -> void
        {
            exceptionPtr = std::current_exception();
        }

        [[nodiscard]] auto get_return_object() -> Task<T>
        {
            return Task<T>{coro_handle::from_promise(*this)};
        }

        [[nodiscard]] auto get_id() const noexcept -> Id
        {
            return taskId;
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

        template<typename U>
        auto await_transform(U &&awaitable) noexcept -> U &&
        {
            return std::forward<U &&>(awaitable);
        }

        template<typename U = void>
        [[nodiscard]] auto await_transform(Task<U>::get_promise_tag /*unused*/)
        {
            struct awaiter : std::suspend_never
            {
                promise_type &promise;

                auto await_resume() -> Id
                {
                    return promise.get_id();
                }
            };

            return awaiter{{}, *this};
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_TASK_HPP */

#ifndef ISL_PROJECT_GENERATOR_HPP
#define ISL_PROJECT_GENERATOR_HPP

#include <isl/isl.hpp>

#if __has_include(<coroutine>)
#    include <coroutine>
#    define ISL_EXPERIMENTAL_COROUTINE false
#else
#    include <experimental/coroutine>
#    define ISL_EXPERIMENTAL_COROUTINE true
#endif

namespace isl
{
    namespace detail
    {
#if ISL_EXPERIMENTAL_COROUTINE
        namespace coroutine = std::experimental;
#else
        namespace coroutine = std;
#endif /* ISL_EXPERIMENTAL_COROUTINE */
    }// namespace detail

    template<typename T>
    class Generator
    {
    public:
        class promise_type;
        class iterator;

        friend iterator;
        friend promise_type;
        using coro_handle = detail::coroutine::coroutine_handle<promise_type>;

        coro_handle handle;

        explicit Generator(coro_handle initial_handle)
          : handle{initial_handle}
        {}

        Generator(const Generator &) = delete;

        Generator(Generator &&) noexcept = default;

        ~Generator()
        {
            handle.destroy();
        }

        auto operator=(const Generator &) -> Generator & = delete;
        auto operator=(Generator &&) noexcept -> Generator & = default;

        auto yield() noexcept(false) ISL_LIFETIMEBOUND->T &
        {
            handle.resume();

            if (done()) {
                throw std::runtime_error{"attempt to yield value from finished generator"};
            }

            promise_type &promise = getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto done() const noexcept -> bool
        {
            return handle.done();
        }

        [[nodiscard]] auto begin() -> iterator
        {
            resume();
            return iterator{*this};
        }

        [[nodiscard]] static constexpr auto end() noexcept -> std::default_sentinel_t
        {
            return std::default_sentinel;
        }

    private:
        auto resume() -> void
        {
            handle.resume();

            if (done()) {
                promise_type &promise = getPromise();
                promise.setValuePtr(nullptr);
            }
        }

        [[nodiscard]] auto getPromise() noexcept -> promise_type &
        {
            return handle.promise();
        }
    };

    template<typename T>
    class Generator<T>::promise_type
    {
    public:
        using suspend_always = detail::coroutine::suspend_always;
        using suspend_never = detail::coroutine::suspend_never;

    private:
        T *value;
        std::exception_ptr exceptionPtr;

    public:
        [[nodiscard]] auto get_return_object() -> Generator<T>
        {
            return Generator<T>{coro_handle::from_promise(*this)};
        }

        [[nodiscard]] auto initial_suspend() const noexcept -> suspend_always
        {
            return {};
        }

        [[nodiscard]] auto final_suspend() const noexcept -> suspend_always
        {
            return {};
        }

        template<typename U>
        auto yield_value(U &&new_value) -> suspend_always
            requires(std::is_same_v<T, std::remove_cvref_t<U>>)
        {
            value = std::addressof(new_value);
            return {};
        }

        auto unhandled_exception() -> void
        {
            exceptionPtr = std::current_exception();
        }

        auto return_void() const noexcept -> void
        {}

        [[nodiscard]] auto getValue() noexcept(false) ISL_LIFETIMEBOUND->T &
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            return *value;
        }

        [[nodiscard]] auto getValue() const noexcept(false) ISL_LIFETIMEBOUND->const T &
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            return *value;
        }

        auto setValuePtr(T *ptr) noexcept -> void
        {
            value = ptr;
        }

        [[nodiscard]] auto getValuePtr() noexcept -> T *
        {
            return value;
        }

        [[nodiscard]] auto getValuePtr() const noexcept -> const T *
        {
            return value;
        }
    };

    template<typename T>
    class Generator<T>::iterator
    {
    private:
        Generator<T> *generatorPtr{nullptr};

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;

        explicit iterator(Generator<T> &generator)
          : generatorPtr{std::addressof(generator)}
        {}

        [[nodiscard]] auto owner() const noexcept -> const Generator<T> &
        {
            return *generatorPtr;
        }

        [[nodiscard]] auto operator*() -> T &
        {
            promise_type &promise = generatorPtr->getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            const promise_type &promise = generatorPtr->getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto operator==([[maybe_unused]] std::default_sentinel_t _) const noexcept
            -> bool
        {
            const promise_type &promise = generatorPtr->getPromise();
            return promise.getValuePtr() == nullptr;
        }

        auto operator++() ISL_LIFETIMEBOUND->iterator &
        {
            generatorPtr->resume();
            return *this;
        }

        auto operator++(int) -> void
        {
            ++(*this);
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_GENERATOR_HPP */

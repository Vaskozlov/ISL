#ifndef ISL_PROJECT_GENERATOR_HPP
#define ISL_PROJECT_GENERATOR_HPP

#include <isl/coroutine/defines.hpp>
#include <isl/isl.hpp>

namespace isl
{
    template<typename T>
    class Generator
    {
    public:
        class promise_type;
        class iterator;

        friend iterator;
        friend promise_type;
        using coro_handle = coro::coroutine_handle<promise_type>;

        coro_handle handle;

        explicit Generator(coro_handle initial_handle)
          : handle{initial_handle}
        {}

        Generator(const Generator &) = delete;

        Generator(Generator &&other) noexcept
          : handle{std::exchange(other.handle, nullptr)}
        {}

        ~Generator()
        {
            if (handle != nullptr) {
                handle.destroy();
            }
        }

        auto operator=(const Generator &) -> Generator & = delete;

        auto operator=(Generator &&other) noexcept -> Generator &
        {
            std::swap(handle, other.handle);
            return *this;
        }

        auto yield() noexcept(false) ISL_LIFETIMEBOUND -> T &
        {
            handle.resume();

            if (done()) {
                throw std::runtime_error{"attempt to yield value from finished generator"};
            }

            auto &promise = getPromise();
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
    private:
        coro::ValueStorageType<T> value{};
        std::exception_ptr exceptionPtr{nullptr};

    public:
        [[nodiscard]] auto get_return_object() -> Generator
        {
            return Generator{coro_handle::from_promise(*this)};
        }

        [[nodiscard]] auto initial_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        [[nodiscard]] auto final_suspend() const noexcept -> coro::suspend_always
        {
            return coro::suspend_always{};
        }

        template<typename U>
        auto yield_value(U &&new_value) -> coro::suspend_always
            requires(std::constructible_from<T, U>)
        {
            if constexpr (std::is_trivial_v<T>) {
                value = new_value;
            } else {
                value = std::addressof(new_value);
            }

            return {};
        }

        auto unhandled_exception() -> void
        {
            exceptionPtr = std::current_exception();
        }

        auto return_void() const noexcept -> void
        {}

        [[nodiscard]] auto getValue() noexcept(false) ISL_LIFETIMEBOUND -> T &
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            if (!hasValue()) {
                throw std::runtime_error{"No value is stored in generator"};
            }

            return *value;
        }

        [[nodiscard]] auto getValue() const noexcept(false) ISL_LIFETIMEBOUND -> const T &
        {
            if (exceptionPtr != nullptr) {
                std::rethrow_exception(exceptionPtr);
            }

            if (!hasValue()) {
                throw std::runtime_error{"No value is stored in generator"};
            }

            return *value;
        }

        auto setValuePtr(T *ptr) noexcept -> void
        {
            if constexpr (std::is_trivial_v<T>) {
                value = *ptr;
            } else {
                value = ptr;
            }
        }

        [[nodiscard]] auto getValuePtr() noexcept -> T *
        {
            if constexpr (std::is_trivial_v<T>) {
                if (!hasValue()) {
                    return nullptr;
                }

                return &(*value);
            } else {
                return value;
            }
        }

        [[nodiscard]] auto getValuePtr() const noexcept -> const T *
        {
            if constexpr (std::is_trivial_v<T>) {
                if (!hasValue()) {
                    return nullptr;
                }

                return &(*value);
            } else {
                return value;
            }
        }

    private:
        [[nodiscard]] ISL_INLINE auto hasValue() const noexcept -> bool
        {
            if constexpr (std::is_trivial_v<T>) {
                return value.has_value();
            } else {
                return value != nullptr;
            }
        }
    };

    template<typename T>
    class Generator<T>::iterator
    {
    private:
        Generator *generatorPtr{nullptr};

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;

        explicit iterator(Generator &generator)
          : generatorPtr{std::addressof(generator)}
        {}

        [[nodiscard]] auto owner() const noexcept -> const Generator &
        {
            return *generatorPtr;
        }

        [[nodiscard]] auto operator*() -> T &
        {
            auto &promise = generatorPtr->getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            const auto &promise = generatorPtr->getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto
            operator==([[maybe_unused]] std::default_sentinel_t _) const noexcept -> bool
        {
            const auto &promise = generatorPtr->getPromise();
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

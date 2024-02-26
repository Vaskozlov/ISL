module;

#include <isl/detail/defines.hpp>
#include <isl/std.hpp>

export module isl:generator;

export namespace isl {
    template<typename T>
    class Generator {
    public:
        class promise_type {
        public:
            using suspend_always = std::suspend_always;
            using suspend_never = std::suspend_never;

            static constexpr bool TrivialTypeStored = std::is_trivial_v<T>;
            using ValueStorageType = std::conditional_t<TrivialTypeStored, std::optional<T>, T *>;

        private:
            ValueStorageType value{};
            std::exception_ptr exceptionPtr{nullptr};

        public:
            [[nodiscard]] auto get_return_object() -> Generator<T> {
                return Generator<T>{coro_handle::from_promise(*this)};
            }

            [[nodiscard]] auto initial_suspend() const noexcept -> suspend_always {
                return {};
            }

            [[nodiscard]] auto final_suspend() const noexcept -> suspend_always {
                return {};
            }

            template<typename U>
            auto yield_value(U&&new_value) -> suspend_always requires(std::is_same_v<T, std::remove_cvref_t<U>>) {
                if constexpr (TrivialTypeStored) {
                    value = new_value;
                }
                else {
                    value = std::addressof(new_value);
                }

                return {};
            }

            auto unhandled_exception() -> void {
                exceptionPtr = std::current_exception();
            }

            auto return_void() const noexcept -> void {
            }

            [[nodiscard]] auto getValue() noexcept(false) ISL_LIFETIMEBOUND -> T& {
                if (exceptionPtr != nullptr) {
                    std::rethrow_exception(exceptionPtr);
                }

                if (!hasValue()) {
                    throw std::runtime_error{"No value is stored in generator"};
                }

                return *value;
            }

            [[nodiscard]] auto getValue() const noexcept(false) ISL_LIFETIMEBOUND -> const T& {
                if (exceptionPtr != nullptr) {
                    std::rethrow_exception(exceptionPtr);
                }

                if (!hasValue()) {
                    throw std::runtime_error{"No value is stored in generator"};
                }

                return *value;
            }

            auto setValuePtr(T* ptr) noexcept -> void {
                if constexpr (TrivialTypeStored) {
                    value = *ptr;
                }
                else {
                    value = ptr;
                }
            }

            [[nodiscard]] auto getValuePtr() noexcept -> T* {
                if constexpr (TrivialTypeStored) {
                    if (!hasValue()) {
                        return nullptr;
                    }

                    return &(*value);
                }
                else {
                    return value;
                }
            }

            [[nodiscard]] auto getValuePtr() const noexcept -> const T* {
                if constexpr (TrivialTypeStored) {
                    if (!hasValue()) {
                        return nullptr;
                    }

                    return &(*value);
                }
                else {
                    return value;
                }
            }

        private:
            [[nodiscard]] ISL_INLINE auto hasValue() const noexcept -> bool {
                if constexpr (TrivialTypeStored) {
                    return value.has_value();
                }
                else {
                    return value != nullptr;
                }
            }
        };

        class iterator {
        private:
            Generator* generatorPtr{nullptr};

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;

            explicit iterator(Generator&generator)
                : generatorPtr{std::addressof(generator)} {
            }

            [[nodiscard]] auto owner() const noexcept -> const Generator& {
                return *generatorPtr;
            }

            [[nodiscard]] auto operator*() -> T& {
                promise_type&promise = generatorPtr->getPromise();
                return promise.getValue();
            }

            [[nodiscard]] auto operator*() const -> const T& {
                const promise_type&promise = generatorPtr->getPromise();
                return promise.getValue();
            }

            [[nodiscard]] auto
            operator==([[maybe_unused]] std::default_sentinel_t _) const noexcept -> bool {
                const promise_type&promise = generatorPtr->getPromise();
                return promise.getValuePtr() == nullptr;
            }

            auto operator++() ISL_LIFETIMEBOUND -> iterator& {
                generatorPtr->resume();
                return *this;
            }

            auto operator++(int) -> void {
                ++(*this);
            }
        };

        friend promise_type;
        using coro_handle = std::coroutine_handle<promise_type>;

        coro_handle handle;

        explicit Generator(coro_handle initial_handle)
            : handle{initial_handle} {
        }

        Generator(const Generator&) = delete;

        Generator(Generator&&) noexcept = default;

        ~Generator() {
            handle.destroy();
        }

        auto operator=(const Generator&) -> Generator& = delete;

        auto operator=(Generator&&) noexcept -> Generator& = default;

        auto yield() noexcept(false) ISL_LIFETIMEBOUND -> T& {
            handle.resume();

            if (done()) {
                throw std::runtime_error{"attempt to yield value from finished generator"};
            }

            promise_type&promise = getPromise();
            return promise.getValue();
        }

        [[nodiscard]] auto done() const noexcept -> bool {
            return handle.done();
        }

        [[nodiscard]] auto begin() -> iterator {
            resume();
            return iterator{*this};
        }

        [[nodiscard]] static constexpr auto end() noexcept -> std::default_sentinel_t {
            return std::default_sentinel;
        }

    private:
        auto resume() -> void {
            handle.resume();

            if (done()) {
                promise_type&promise = getPromise();
                promise.setValuePtr(nullptr);
            }
        }

        [[nodiscard]] auto getPromise() noexcept -> promise_type& {
            return handle.promise();
        }
    };
} // namespace isl

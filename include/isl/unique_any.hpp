#ifndef ISL_PROJECT_UNIQUE_ANY_HPP
#define ISL_PROJECT_UNIQUE_ANY_HPP

#include <functional>
#include <isl/memory.hpp>
#include <typeindex>

namespace isl
{
    ISL_EXCEPTION(bad_unique_any_cast, std::runtime_error, runtime_error);

    class UniqueAny
    {
        using Deleter = void (*)(void *);

        void *pointer{};
        Deleter deleter{};
        std::type_index typeIndex{typeid(std::nullopt_t)};

    public:
        template<typename T>
        explicit UniqueAny(T &&object)
        {
            emplace<T>(std::forward<T>(object));
        }

        UniqueAny(const UniqueAny &) = delete;

        UniqueAny(UniqueAny &&other) noexcept
          : pointer{std::exchange(other.pointer, nullptr)}
          , deleter{std::exchange(other.deleter, nullptr)}
        {}

        auto operator=(const UniqueAny &) -> void = delete;

        auto operator=(UniqueAny &&other) noexcept -> UniqueAny &
        {
            std::swap(pointer, other.pointer);
            std::swap(deleter, other.deleter);
            std::swap(typeIndex, other.typeIndex);

            return *this;
        }

        [[nodiscard]] auto hasValue() const noexcept -> bool
        {
            return pointer != nullptr;
        }

        [[nodiscard]] auto getTypeIndex() const noexcept -> std::type_index
        {
            return typeIndex;
        }

        template<typename T, typename... Ts>
        auto emplace(Ts &&...args) -> void
        {
            pointer = static_cast<void *>(new T{std::forward<Ts>(args)...});
            typeIndex = std::type_index{typeid(T)};

            deleter = [](void *p) {
                delete static_cast<T *>(p);
            };
        }

        template<typename T>
        [[nodiscard]] auto get() -> isl::UniquePtr<T>
        {
            if (std::type_index{typeid(T)} == typeIndex) {
                deleter = nullptr;
                typeIndex = std::type_index{typeid(std::nullopt_t)};
                return isl::UniquePtr<T>{static_cast<T *>(std::exchange(pointer, nullptr))};
            }

            return nullptr;
        }

        template<typename T>
        [[nodiscard]] auto getAsBaseClass() -> isl::UniquePtr<T>
        {
            if (auto *casted_ptr = dynamic_cast<T *>(pointer); casted_ptr != nullptr) {
                deleter = nullptr;
                typeIndex = std::type_index{typeid(std::nullopt_t)};
                return isl::UniquePtr<T>{static_cast<T *>(std::exchange(pointer, nullptr))};
            }

            return nullptr;
        }

        ~UniqueAny()
        {
            if (deleter != nullptr) {
                deleter(pointer);
            }
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_UNIQUE_ANY_HPP */

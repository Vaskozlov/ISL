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
        UniqueAny() = default;

        UniqueAny(std::nullopt_t)
        {}

        template<typename T>
        [[nodiscard]] explicit UniqueAny(isl::UniquePtr<T> unique_ptr)
        {
            emplace<T>(std::move(unique_ptr));
        }

        template<typename T>
        [[nodiscard]] explicit UniqueAny(T &&object)
        {
            emplace<T>(std::forward<T>(object));
        }

        template<typename T, typename... Ts>
        [[nodiscard]] explicit UniqueAny(std::in_place_type_t<T>, Ts &&...args)
        {
            emplace<T>(std::forward<Ts>(args)...);
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

        template<typename T>
        auto emplace(isl::UniquePtr<T> unique_ptr) -> void
        {
            pointer = static_cast<void *>(unique_ptr.release());
            typeIndex = std::type_index{typeid(T)};
            deleter = [](void *p) {
                delete static_cast<T *>(p);
            };
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
        [[nodiscard]] auto getNoThrow() -> isl::UniquePtr<T>
        {
            if (std::type_index{typeid(T)} == typeIndex) {
                deleter = nullptr;
                typeIndex = std::type_index{typeid(std::nullopt_t)};
                return isl::UniquePtr<T>{static_cast<T *>(std::exchange(pointer, nullptr))};
            }

            return nullptr;
        }

        template<typename T>
        [[nodiscard]] auto get() -> isl::UniquePtr<T>
        {
            auto result = getNoThrow<T>();

            if (result == nullptr) {
                throw bad_unique_any_cast{
                    std::string{"An attempt to get object of type "} + typeid(T).name() +
                    ", but stored object has type {}" + typeIndex.name()};
            }

            return result;
        }

        ~UniqueAny()
        {
            if (deleter != nullptr) {
                deleter(pointer);
            }
        }
    };


    template<typename T>
    [[nodiscard]] auto anyCast(UniqueAny &unique_any) -> isl::UniquePtr<T>
    {
        return unique_any.template get<T>();
    }

    template<typename T, typename... Ts>
    [[nodiscard]] auto makeAny(Ts &&...args) -> UniqueAny
    {
        return UniqueAny{std::in_place_type<T>, std::forward<Ts>(args)...};
    }
}// namespace isl

#endif /* ISL_PROJECT_UNIQUE_ANY_HPP */

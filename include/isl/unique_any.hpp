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

        struct PointerAndDeleter
        {
            void *pointer{};
            Deleter deleter{};
        };

        union {
            PointerAndDeleter deleterAndPointer{};
            std::array<std::byte, sizeof(PointerAndDeleter)> rowBuffer;
        };

        std::type_index typeIndex{typeid(std::nullopt_t)};
        bool storesTrivialObject{};

    public:
        UniqueAny() = default;

        UniqueAny(std::nullopt_t)
          : storesTrivialObject{true}
        {}

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
          : rowBuffer{other.rowBuffer}
          , typeIndex{std::move(other.typeIndex)}
          , storesTrivialObject{other.storesTrivialObject}
        {
            other.clearInternalStorage();
        }

        ~UniqueAny()
        {
            if (!storesTrivialObject && deleterAndPointer.deleter != nullptr) {
                deleterAndPointer.deleter(deleterAndPointer.pointer);
            }
        }

        auto operator=(const UniqueAny &) -> void = delete;

        auto operator=(UniqueAny &&other) noexcept -> UniqueAny &
        {
            std::swap(rowBuffer, other.rowBuffer);
            std::swap(typeIndex, other.typeIndex);
            std::swap(storesTrivialObject, other.storesTrivialObject);

            return *this;
        }

        [[nodiscard]] auto hasValue() const noexcept -> bool
        {
            return typeIndex != std::type_index(typeid(std::nullopt_t));
        }

        [[nodiscard]] auto getTypeIndex() const noexcept -> std::type_index
        {
            return typeIndex;
        }

        template<typename T, typename... Ts>
        auto emplace(Ts &&...args) -> void
        {
            typeIndex = std::type_index{typeid(T)};

            if constexpr (std::is_trivial_v<T> && sizeof(T) <= sizeof(PointerAndDeleter)) {
                storesTrivialObject = true;
                std::construct_at(
                    reinterpret_cast<T *>(rowBuffer.data()), std::forward<Ts>(args)...);
            } else {
                storesTrivialObject = false;
                deleterAndPointer.pointer = static_cast<void *>(new T{std::forward<Ts>(args)...});
                deleterAndPointer.deleter = [](void *p) {
                    delete static_cast<T *>(p);
                };
            }
        }

        template<typename T>
        [[nodiscard]] auto get() -> T
        {
            storesTrivialObject = false;

            if (std::type_index{typeid(T)} != typeIndex) {
                throw bad_unique_any_cast{
                    std::string{"An attempt to get object of type "} + typeid(T).name() +
                    ", but stored object has type {}" + typeIndex.name()};
            }

            if constexpr (std::is_trivial_v<T> && sizeof(T) <= sizeof(PointerAndDeleter)) {
                auto result = T{*reinterpret_cast<T *>(rowBuffer.data())};
                clearInternalStorage();
                return result;
            } else {
                T result = std::move(*static_cast<T *>(deleterAndPointer.pointer));
                clearInternalStorage();
                return result;
            }
        }

        template<typename T>
        [[nodiscard]] auto observe() -> T *
        {
            if (std::type_index{typeid(T)} != typeIndex) {
                throw bad_unique_any_cast{
                    std::string{"An attempt to get object of type "} + typeid(T).name() +
                    ", but stored object has type {}" + typeIndex.name()};
            }

            if constexpr (std::is_trivial_v<T> && sizeof(T) <= sizeof(PointerAndDeleter)) {
                return reinterpret_cast<T *>(rowBuffer.data());
            } else {
                return static_cast<T *>(deleterAndPointer.pointer);
            }
        }

    private:
        auto clearInternalStorage() -> void
        {
            if (!storesTrivialObject && deleterAndPointer.deleter != nullptr) {
                deleterAndPointer.deleter(deleterAndPointer.pointer);
            }

            rowBuffer.fill(std::byte{0});
            typeIndex = std::type_index{typeid(std::nullopt_t)};
            storesTrivialObject = false;
        }
    };


    template<typename T>
    [[nodiscard]] auto get(UniqueAny &unique_any) -> T
    {
        return unique_any.template get<T>();
    }

    template<typename T>
    [[nodiscard]] auto get(UniqueAny &&unique_any) -> T
    {
        return unique_any.template get<T>();
    }

    template<typename T>
    [[nodiscard]] auto observe(UniqueAny &unique_any) -> T *
    {
        return unique_any.template observe<T>();
    }

    template<typename T, typename... Ts>
    [[nodiscard]] auto makeAny(Ts &&...args) -> UniqueAny
    {
        return UniqueAny{std::in_place_type<T>, std::forward<Ts>(args)...};
    }
}// namespace isl

#endif /* ISL_PROJECT_UNIQUE_ANY_HPP */

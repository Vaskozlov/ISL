#ifndef ISL_PROJECT_UNIQUE_ANY_HPP
#define ISL_PROJECT_UNIQUE_ANY_HPP

#include <functional>
#include <isl/detail/defines.hpp>
#include <typeindex>

namespace isl
{
    ISL_EXCEPTION(bad_unique_any_cast, std::runtime_error, runtime_error);

    namespace detail
    {
        struct PointerAndDeleter
        {
            void *pointer = nullptr;
            void (*deleter)(void *) = nullptr;
        };

        template<typename T>
        concept UniqueAnyCanStoreInside =
            std::is_trivial_v<T> && sizeof(T) <= sizeof(PointerAndDeleter);
    }// namespace detail

    class UniqueAny
    {
        union {
            detail::PointerAndDeleter pointerAndDeleter{};
            std::array<std::byte, sizeof(detail::PointerAndDeleter)> rowBuffer;
        };

        std::type_index typeIndex{typeid(std::nullopt_t)};
        bool storesTrivialObject{};

    public:
        UniqueAny() = default;

        // NOLINTNEXTLINE
        UniqueAny(std::nullopt_t)
          : storesTrivialObject{true}
        {}

        template<typename T>
        [[nodiscard]] explicit UniqueAny(T &&object)
        {
            emplace<T>(std::forward<T>(object));
        }

        template<typename T, typename... Ts>
            requires(std::constructible_from<T, Ts...>)
        [[nodiscard]] explicit UniqueAny(std::in_place_type_t<T> /*unused*/, Ts &&...args)
        {
            emplace<T>(std::forward<Ts>(args)...);
        }

        UniqueAny(const UniqueAny &) = delete;

        UniqueAny(UniqueAny &&other) noexcept
          : rowBuffer{other.rowBuffer}
          , typeIndex{other.typeIndex}
          , storesTrivialObject{other.storesTrivialObject}
        {
            other.pointerAndDeleter.pointer = nullptr;
            other.pointerAndDeleter.deleter = nullptr;
            other.clearTypeInfo();
        }

        ~UniqueAny()
        {
            deleteStoredObject();
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

        template<detail::UniqueAnyCanStoreInside T, typename... Ts>
            requires(std::constructible_from<T, Ts...>)
        auto emplace(Ts &&...args) -> void
        {
            deleteStoredObject();
            typeIndex = std::type_index{typeid(T)};

            storesTrivialObject = true;
            std::construct_at(reinterpret_cast<T *>(rowBuffer.data()), std::forward<Ts>(args)...);
        }

        template<typename T, typename... Ts>
        auto emplace(Ts &&...args) -> void
            requires(std::constructible_from<T, Ts...>)
        {
            deleteStoredObject();
            typeIndex = std::type_index{typeid(T)};

            storesTrivialObject = false;
            pointerAndDeleter.pointer = static_cast<void *>(new T{std::forward<Ts>(args)...});
            pointerAndDeleter.deleter = [](void *p) {
                delete static_cast<T *>(p);
            };
        }

        template<detail::UniqueAnyCanStoreInside T>
        [[nodiscard]] auto get() -> T
        {
            checkTypeMatch<T>();

            auto result = T{*reinterpret_cast<T *>(rowBuffer.data())};
            clearInternalStorage();

            return result;
        }

        template<typename T>
        [[nodiscard]] auto get() -> T
        {
            checkTypeMatch<T>();

            T result = std::move(*static_cast<T *>(pointerAndDeleter.pointer));
            clearInternalStorage();

            return result;
        }

        template<detail::UniqueAnyCanStoreInside T>
        [[nodiscard]] auto observe() -> T *
        {
            checkTypeMatch<T>();
            return reinterpret_cast<T *>(rowBuffer.data());
        }

        template<typename T>
        [[nodiscard]] auto observe() -> T *
        {
            checkTypeMatch<T>();
            return static_cast<T *>(pointerAndDeleter.pointer);
        }

        template<typename T>
            requires(!detail::UniqueAnyCanStoreInside<T>)
        [[nodiscard]] auto release() -> std::unique_ptr<T>
        {
            checkTypeMatch<T>();

            auto *ptr = static_cast<T *>(std::exchange(pointerAndDeleter.pointer, nullptr));
            pointerAndDeleter.deleter = nullptr;
            clearTypeInfo();

            return std::unique_ptr<T>{ptr};
        }

    private:
        auto deleteStoredObject() const -> void
        {
            if (!storesTrivialObject && pointerAndDeleter.deleter != nullptr) {
                pointerAndDeleter.deleter(pointerAndDeleter.pointer);
            }
        }

        auto clearInternalStorage() -> void
        {
            deleteStoredObject();
            rowBuffer.fill(std::byte{0});
            clearTypeInfo();
        }

        auto clearTypeInfo() -> void
        {
            typeIndex = std::type_index{typeid(std::nullopt_t)};
            storesTrivialObject = false;
        }

        template<typename T>
        auto checkTypeMatch() const -> void
        {
            if (std::type_index{typeid(T)} != typeIndex) {
                throw bad_unique_any_cast{
                    std::string{"An attempt to get object of type "} + typeid(T).name() +
                    ", but stored object has type {}" + typeIndex.name()};
            }
        }
    };

    template<typename T>
    [[nodiscard]] auto get(UniqueAny &unique_any) -> T
    {
        return unique_any.get<T>();
    }

    template<typename T>
    [[nodiscard]] auto get(UniqueAny &&unique_any) -> T
    {
        return unique_any.get<T>();
    }

    template<typename T>
    [[nodiscard]] auto observe(UniqueAny &unique_any) -> T *
    {
        return unique_any.observe<T>();
    }

    template<typename T, typename... Ts>
    [[nodiscard]] auto makeAny(Ts &&...args) -> UniqueAny
    {
        return UniqueAny{std::in_place_type<T>, std::forward<Ts>(args)...};
    }
}// namespace isl

#endif /* ISL_PROJECT_UNIQUE_ANY_HPP */

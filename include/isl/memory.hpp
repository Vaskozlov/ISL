#ifndef ISL_PROJECT_MEMORY_HPP
#define ISL_PROJECT_MEMORY_HPP

#include <isl/detail/object_owner.hpp>
#include <isl/isl.hpp>
#include <memory>

namespace isl
{
    template<typename T>
    using UniquePtr = std::unique_ptr<T, std::default_delete<T>>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T, typename... Ts>
    ISL_DECL auto makeUnique(Ts &&...args) -> UniquePtr<T>
        requires std::constructible_from<T, Ts...>
    {
        return std::make_unique<T>(std::forward<Ts>(args)...);
    }

    template<typename Target, typename Constructed, typename... Ts>
    ISL_DECL auto makeUnique(Ts &&...args) -> UniquePtr<Target>
        requires std::derived_from<Constructed, Target> &&
                 std::constructible_from<Constructed, Ts...>
    {
        return UniquePtr<Target>{as<Target *>(new Constructed{std::forward<Ts>(args)...})};
    }

    template<typename T, typename... Ts>
    ISL_DECL auto makeShared(Ts &&...args) -> std::shared_ptr<T>
        requires std::constructible_from<T, Ts...>
    {
        return std::make_shared<T>(std::forward<Ts>(args)...);
    }

    template<typename Target, typename Constructed, typename... Ts>
    ISL_DECL auto makeShared(Ts &&...args) -> std::shared_ptr<Target>
        requires std::derived_from<Constructed, Target> &&
                 std::constructible_from<Constructed, Ts...>
    {
        return SharedPtr<Target>{as<Target *>(new Constructed{std::forward<Ts>(args)...})};
    }
}// namespace isl

#endif /* ISL_PROJECT_MEMORY_HPP */

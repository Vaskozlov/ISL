module;

#include <isl/detail/defines.hpp>
#include <memory>

export module isl:memory;

export namespace isl
{
    template<typename T, typename... Ts>
    ISL_DECL auto makeUnique(Ts &&...args) -> std::unique_ptr<T>
        requires std::constructible_from<T, Ts...>
    {
        return std::make_unique<T>(std::forward<Ts>(args)...);
    }

    template<typename Target, typename Constructed, typename... Ts>
    ISL_DECL auto makeUnique(Ts &&...args) -> std::unique_ptr<Target>
        requires std::derived_from<Constructed, Target> &&
                 std::constructible_from<Constructed, Ts...>
    {
        return std::unique_ptr<Target>{as<Target *>(new Constructed{std::forward<Ts>(args)...})};
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
        return std::shared_ptr<Target>{as<Target *>(new Constructed{std::forward<Ts>(args)...})};
    }
}// namespace isl

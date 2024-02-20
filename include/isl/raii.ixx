module;

#include <fmt/format.h>
#include <isl/detail/defines.hpp>
#include <optional>
#include <utility>

export module isl:raii;

export namespace isl
{
    template<std::invocable DeleterFunction, std::invocable ConstructorFunction = void(*)()>
    class Raii
    {
    private:
        std::optional<ConstructorFunction> constructorFunction;
        std::optional<DeleterFunction> deleterFunction;

    public:
        ISL_DECL explicit Raii(DeleterFunction &&deleter) noexcept(
            std::is_nothrow_move_constructible_v<DeleterFunction>)
          : deleterFunction{std::move(deleter)}
        {}

        ISL_DECL Raii(ConstructorFunction &&constructor, DeleterFunction &&deleter) noexcept(
            std::is_nothrow_move_constructible_v<DeleterFunction> &&
            std::is_nothrow_move_constructible_v<ConstructorFunction>)
          : constructorFunction{std::move(constructor)}
          , deleterFunction{std::move(deleter)}
        {
            (*constructorFunction)();
        }

        ISL_DECL Raii(Raii &&other) noexcept
          : constructorFunction{std::exchange(other.constructorFunction, std::nullopt)}
          , deleterFunction{std::exchange(other.deleterFunction, std::nullopt)}
        {}

        Raii(const Raii &) = delete;

        constexpr ~Raii() noexcept(std::is_nothrow_invocable_v<DeleterFunction>)
        {
            if (deleterFunction.has_value()) {
                (*deleterFunction)();
            }
        }

        auto operator=(const Raii &) -> Raii & = delete;

        constexpr auto operator=(Raii &&other) noexcept -> Raii &
        {
            std::swap(constructorFunction, other.constructorFunction);
            std::swap(deleterFunction, other.deleterFunction);
            return *this;
        }
    };
}// namespace isl

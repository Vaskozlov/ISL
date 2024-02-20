module;

#include <isl/detail/defines.hpp>
#include <utility>

export module isl:container_guard;

export namespace isl::detail
{
    template<typename T>
    class ContainerGuard : public T
    {
    public:
        using T::T;

        template<typename... Ts>
        ISL_DECL ISL_INLINE auto operator[](Ts &&...args) ISL_LIFETIMEBOUND->decltype(auto)
        {
            return this->operator[](std::forward<Ts>(args)...);
        }
    };
}// namespace isl::detail

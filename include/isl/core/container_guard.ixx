module;
#include <isl/core/defines.hpp>
export module isl.core:container_guard;

export import std;

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

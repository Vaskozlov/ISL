#ifndef ISL_PROJECT_CONTAINER_GUARD_HPP
#define ISL_PROJECT_CONTAINER_GUARD_HPP

#include <isl/detail/defines.hpp>
#include <utility>

namespace isl::detail
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

#endif /* ISL_PROJECT_CONTAINER_GUARD_HPP */

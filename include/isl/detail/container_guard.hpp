#ifndef ISL_PROJECT_CONTAINER_GUARD_HPP
#define ISL_PROJECT_CONTAINER_GUARD_HPP

#include <isl/detail/defines.hpp>
#include <ranges>
#include <utility>

#define CONTAINER_GUARD(RESULT_NAME, CONTAINER, CONTAINER_NAME, ...)                       \
    class RESULT_NAME : public CONTAINER<__VA_ARGS__>                                              \
    {                                                                                              \
    public:                                                                                        \
        using CONTAINER<__VA_ARGS__>::CONTAINER_NAME;                                              \
        template<typename... Ts>                                                                   \
        ISL_DECL auto operator[](Ts &&...args) ISL_LIFETIMEBOUND->decltype(auto)                   \
        {                                                                                          \
            return this->at(std::forward<Ts>(args)...);                                            \
        }                                                                                          \
                                                                                                   \
        template<std::ranges::range R>                                                             \
        explicit RESULT_NAME(R &&range)                                                         \
          : CONTAINER<__VA_ARGS__>::CONTAINER_NAME{range.begin(), range.end()}                                                          \
        {}                                                                                         \
    };

namespace isl::detail
{
    template<typename T>
    class ContainerGuard : public T
    {
    public:
        using T::T;

        template<typename... Ts>
        ISL_DECL auto operator[](Ts &&...args) ISL_LIFETIMEBOUND->decltype(auto)
        {
            return this->at(std::forward<Ts>(args)...);
        }

        template<std::ranges::range R>
        explicit ContainerGuard(R &&range)
          : T{range.begin(), range.end()}
        {}
    };
}// namespace isl::detail

#endif /* ISL_PROJECT_CONTAINER_GUARD_HPP */

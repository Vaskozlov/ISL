#ifndef ISL_PROJECT_CONTAINER_GUARD_HPP
#define ISL_PROJECT_CONTAINER_GUARD_HPP

#include <algorithm>
#include <concepts>
#include <isl/detail/defines.hpp>
#include <ranges>
#include <utility>

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


        [[nodiscard]] auto operator==(const ContainerGuard<T> &other) const noexcept -> bool
        {
            return static_cast<const T &>(*this) == static_cast<const T &>(other);
        }

        [[nodiscard]] auto
            operator<=>(const ContainerGuard<T> &other) const noexcept -> decltype(auto)
        {
            return static_cast<const T &>(*this) <=> static_cast<const T &>(other);
        }
    };
}// namespace isl::detail

#endif /* ISL_PROJECT_CONTAINER_GUARD_HPP */

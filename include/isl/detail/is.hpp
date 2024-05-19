#ifndef ISL_IS_HPP
#define ISL_IS_HPP

#include <isl/detail/as.hpp>
#include <utility>

namespace isl
{
    template<typename T, typename U>
    ISL_DECL auto is(U *value) noexcept(false) -> bool
        requires std::is_pointer_v<T> && (!detail::DerivedOrSame<U, T>);

    template<typename T, typename U>
    ISL_DECL auto is(U && /*value*/) noexcept -> bool
        requires(!std::is_reference_v<T> && !std::is_pointer_v<T>) && detail::DerivedOrSame<U, T>
    {
        return true;
    }

    template<typename T, typename U>
    ISL_DECL auto is(U && /*value*/) noexcept -> bool
        requires(!std::is_reference_v<T> && !std::is_pointer_v<T> && !detail::DerivedOrSame<U, T>)
    {
        return false;
    }

    template<typename T, typename U>
    ISL_DECL auto is(U &value) noexcept -> bool
        requires std::is_reference_v<T> && (!detail::DerivedOrSame<U, T>)
    {
        return is<std::remove_reference_t<T> *>(&value);
    }

    template<typename T, typename U>
    ISL_DECL auto is(U & /*value*/) noexcept -> bool
        requires std::is_reference_v<T> && (detail::DerivedOrSame<U, T>)
    {
        return true;
    }

    template<typename T, typename U>
    ISL_DECL auto is(U * /*value*/) noexcept -> bool
        requires std::is_pointer_v<T> && detail::DerivedOrSame<U, T>
    {
        return true;
    }

    template<typename T, typename U>
    ISL_DECL auto is(U *value) noexcept(false) -> bool
        requires std::is_pointer_v<T> && (!detail::DerivedOrSame<U, T>)
    {
        if constexpr (!detail::CanDoUpcastingOrDownCasting<
                          std::remove_pointer_t<U>, std::remove_pointer_t<T>>) {
            return false;
        } else {
            return dynamic_cast<T>(value) != nullptr;
        }
    }
}// namespace isl


#endif /* ISL_IS_HPP */

#ifndef ISL_IS_HPP
#define ISL_IS_HPP

#include <isl/detail/as.hpp>
#include <utility>

namespace isl
{
    template<typename T, typename U>
    ISL_DECL auto is(const U *value) noexcept(false) -> bool
        requires std::is_pointer_v<T> && (!DerivedOrSame<U, T>);

    template<typename T, typename U>
    ISL_DECL auto is(const U &value) noexcept -> bool
        requires(!std::is_pointer_v<T> && !DerivedOrSame<U, T>)
    {
        return is<std::remove_reference_t<T> *>(&value);
    }

    template<typename T, typename U>
    ISL_DECL auto is(const U & /*value*/) noexcept -> bool
        requires(!std::is_pointer_v<T>) && DerivedOrSame<U, T>
    {
        return true;
    }

    template<typename T, typename U>
    ISL_DECL auto is(const U * /*value*/) noexcept -> bool
        requires std::is_pointer_v<T> && DerivedOrSame<U, T>
    {
        return true;
    }

    template<typename T, typename U>
    ISL_DECL auto is(const U *value) noexcept(false) -> bool
        requires std::is_pointer_v<T> && (!DerivedOrSame<U, T>)
    {
        if constexpr (!CanDoUpcastingOrDownCasting<
                          std::remove_pointer_t<U>, std::remove_pointer_t<T>>) {
            return false;
        } else {
            return dynamic_cast<const std::remove_cvref_t<std::remove_pointer_t<T>> *>(value) !=
                   nullptr;
        }
    }
}// namespace isl


#endif /* ISL_IS_HPP */

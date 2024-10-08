#ifndef ISL_AS_HPP
#define ISL_AS_HPP

#include <isl/detail/defines.hpp>
#include <type_traits>
#include <concepts>

namespace isl
{
    template<typename Dp, typename Bp>
    concept DerivedOrSame = std::same_as<std::remove_cvref_t<Dp>, std::remove_cvref_t<Bp>> ||
                            std::derived_from<std::remove_cvref_t<Dp>, std::remove_cvref_t<Bp>>;

    template<typename T, typename U>
    ISL_DECL auto as(U &&value) ISL_NOEXCEPT_IF(static_cast<T>(std::forward<U>(value))) -> T
    {
        return static_cast<T>(std::forward<U>(value));
    }

    template<typename T, typename U>
    ISL_DECL auto as(U &value) ISL_NOEXCEPT_IF(static_cast<T>(value)) -> T
        requires std::is_reference_v<T> && DerivedOrSame<U, T>
    {
        return static_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U &value) noexcept(false) -> T
        requires std::is_reference_v<T> && (!DerivedOrSame<U, T>)
    {
        return dynamic_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U *value) ISL_NOEXCEPT_IF(static_cast<T>(value)) -> T
        requires std::is_pointer_v<T> && DerivedOrSame<U, std::remove_pointer_t<T>>
    {
        return static_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U *value) noexcept(false) -> T
        requires(std::is_pointer_v<T> && !DerivedOrSame<U, std::remove_pointer_t<T>>)
    {
        return dynamic_cast<T>(value);
    }
}// namespace isl


#endif /* ISL_AS_HPP */

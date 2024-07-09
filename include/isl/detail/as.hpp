#ifndef ISL_AS_HPP
#define ISL_AS_HPP

#include <concepts>
#include <isl/detail/defines.hpp>
#include <type_traits>
#include <utility>

namespace isl
{
    template<typename From, typename To>
    concept CanDoUpcastingOrDownCasting =
        requires(const From *from) { static_cast<const To *>(from); };

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
        requires std::is_pointer_v<T> && (!DerivedOrSame<U, std::remove_pointer_t<T>>)
    {
        static_assert(
            CanDoUpcastingOrDownCasting<std::remove_pointer_t<U>, std::remove_pointer_t<T>>,
            "Cast from U to T failed. Upcast is impossible and downcast is impossible too, "
            "because U and T have different base classes.");

        return dynamic_cast<T>(value);
    }
}// namespace isl


#endif /* ISL_AS_HPP */

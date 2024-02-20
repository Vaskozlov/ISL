module;

#include <isl/detail/defines.hpp>
#include <utility>
#include <type_traits>

export module isl:as;

export namespace isl
{
    namespace detail
    {
        template<typename From, typename To>
        concept CanDoUpcastingOrDowncasting =
            requires(const From *from) { static_cast<const To *>(from); };
    }// namespace detail

    template<typename T, typename U>
    ISL_DECL auto as(U &&value) ISL_NOEXCEPT_IF(static_cast<T>(std::forward<U>(value))) -> T
    {
        return static_cast<T>(std::forward<U>(value));
    }

    template<typename T, typename U>
    ISL_DECL auto as(U &value) ISL_NOEXCEPT_IF(static_cast<T>(value)) -> T
        requires std::is_reference_v<T> &&
                 std::derived_from<std::remove_const_t<U>, std::remove_cvref_t<T>>
    {
        return static_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U &value) noexcept(false) -> T
        requires std::is_reference_v<T> &&
                 (!std::derived_from<std::remove_const_t<U>, std::remove_reference_t<T>>)
    {
        return dynamic_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U *value) ISL_NOEXCEPT_IF(static_cast<T>(value)) -> T
        requires std::is_pointer_v<T> &&
                 std::derived_from<std::remove_const_t<U>, std::remove_pointer_t<T>>
    {
        return static_cast<T>(value);
    }

    template<typename T, typename U>
    ISL_DECL auto as(U *value) noexcept(false) -> T
        requires std::is_pointer_v<T> &&
                 (!std::derived_from<std::remove_const_t<U>, std::remove_pointer_t<T>>)
    {
        static_assert(
            detail::CanDoUpcastingOrDowncasting<std::remove_pointer_t<U>, std::remove_pointer_t<T>>,
            "Cast from U to T failed. Upcast is impossible and downcast is impossible too, "
            "because U and T have different base classes.");

        return dynamic_cast<T>(value);
    }
}// namespace isl

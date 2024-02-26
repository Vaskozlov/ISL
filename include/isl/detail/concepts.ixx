module;
#include <isl/std.hpp>
export module isl:concepts;

export namespace isl
{
    template<typename T>
    concept Trivial = std::is_trivial_v<T>;

    template<typename T>
    concept Boolean = std::is_convertible_v<std::remove_cvref_t<T>, bool>;

    template<typename T>
    concept Exception = std::is_base_of_v<std::exception, T>;

    template<typename T>
    concept Iterable = requires(const T &value) { (++value.begin()) != value.end(); };

    template<typename T, typename... Ts>
    concept IsSameToAny = (... || std::is_same_v<T, Ts>);

    template<typename T>
    concept CharacterLiteral =
        IsSameToAny<std::remove_cvref_t<T>, char, char8_t, char16_t, char32_t, wchar_t>;

    template<typename T>
    concept CharacterArray = std::is_pointer_v<T> || std::is_array_v<T>;

    template<typename Fn, typename... Args>
    concept Invocable = std::is_invocable_v<Fn, Args...>;

    template<typename T>
    concept Integral = std::is_integral_v<T>;

    template<typename T>
    concept FloatingPoint = std::is_floating_point_v<T>;
}// namespace isl

#ifndef ISL_PROJECT_CONCEPTS_HPP
#define ISL_PROJECT_CONCEPTS_HPP

#include <concepts>
#include <exception>
#include <ranges>

#define ISL_RANGE_OVER_CONCEPT(C)                                                                  \
    []<C> {                                                                                        \
    } >

namespace isl
{
    template<typename T, template<class> class... Ts>
    concept Trait = (... && Ts<T>::value);

    template<typename T, template<class> class... Ts>
    concept AnyTrait = (... || Ts<T>::value);

    template<typename T>
    concept Boolean = std::is_convertible_v<std::remove_cvref_t<T>, bool>;

    template<typename T>
    concept Exception = std::is_base_of_v<std::exception, T>;

    template<typename T>
    concept Iterable = requires(const T &value) { ++value.begin() != value.end(); };

    template<typename T, typename... Ts>
    concept IsSameToAny = (... || std::same_as<T, Ts>);

    template<typename T>
    concept CharacterLiteral =
        IsSameToAny<std::remove_cvref_t<T>, char, char8_t, char16_t, char32_t, wchar_t>;

    template<typename T, typename U>
    concept RangeOf = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, U>;

    template<typename T, auto C>
    concept RangeOver = std::ranges::range<T> &&
                        requires { C.template operator()<std::ranges::range_value_t<T>>(); };
}// namespace isl

#endif /* ISL_PROJECT_CONCEPTS_HPP */

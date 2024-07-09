#ifndef ISL_PROJECT_JOIN_HPP
#define ISL_PROJECT_JOIN_HPP

#include <isl/string_view.hpp>

namespace isl
{

    template<Iterable Container, typename Function, StringLike<char> Separator>
    [[nodiscard]] auto join(
        const Container &container, Function &&function, const Separator &separator) -> std::string
    {
        auto result = std::string{};
        auto begin = std::begin(container);
        const auto end = std::end(container);
        const auto converted_separator = as<std::string_view>(separator);

        if (begin == end) [[unlikely]] {
            return result;
        }

        result.append(function(*begin));

        for (++begin; begin != end; ++begin) {
            result.append(converted_separator);
            result.append(function(*begin));
        }

        return result;
    }

    template<Iterable Container, typename Function, std::size_t N>
    [[nodiscard]] ISL_INLINE auto join(
        Container &&container, Function &&function, const CArray<char, N> &separator) -> std::string
    {
        return join(
            std::forward<Container>(container), std::forward<Function>(function),
            std::string_view{separator}

        );
    }

    template<Iterable Container, StringLike<char> Separator>
    [[nodiscard]] auto join(Container &&container, Separator &&separator) -> std::string
    {
        if constexpr (StringLike<decltype(*container.begin()), char>) {
            return join(
                std::forward<decltype(container)>(container),

                []<typename T>(T &&elem) -> decltype(auto) {
                    return std::forward<T>(elem);
                },
                std::forward<Separator>(separator));
        } else {
            return join(
                std::forward<decltype(container)>(container),
                []<typename T>(T &&elem) -> decltype(auto) {
                    return std::to_string(std::forward<T>(elem));
                },
                std::forward<Separator>(separator));
        }
    }

    template<Iterable Container, std::size_t N>
    [[nodiscard]] ISL_INLINE auto
        join(Container &&container, const CArray<char, N> &separator) -> std::string
    {
        return join(std::forward<Container>(container), as<std::string_view>(separator));
    }
}// namespace isl

#endif /* ISL_PROJECT_JOIN_HPP */

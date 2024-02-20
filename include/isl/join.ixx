module;

#include <fmt/format.h>
#include <isl/detail/defines.hpp>

export module isl:join;

import :string_view;
import :types;
import :iterator;
import :concepts;
import :as;

export namespace isl {
    template<Iterable Container, typename Function, StringLike<char> Separator>
    [[nodiscard]] auto join(
        const Container&container, Function&&function, const Separator&separator) -> std::string {
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

    template<Iterable Container, typename Function, size_t N>
    [[nodiscard]] ISL_INLINE auto join(
        Container&&container, Function&&function, const CArray<char, N>&separator) -> std::string {
        return join(
            std::forward<Container>(container), std::forward<Function>(function),
            std::string_view{separator}

        );
    }

    template<Iterable Container, StringLike<char> Separator>
    [[nodiscard]] auto join(Container&&container, Separator&&separator) -> std::string {
        if constexpr (StringLike<decltype(*container.begin()), char>) {
            return join(
                std::forward<decltype(container)>(container),

                []<typename T>(T&&elem) -> decltype(auto) {
                    return std::forward<T>(elem);
                },
                std::forward<Separator>(separator));
        }
        else {
            return join(
                std::forward<decltype(container)>(container),
                []<typename T>(T&&elem) -> decltype(auto) {
                    return fmt::to_string(std::forward<T>(elem));
                },
                std::forward<Separator>(separator));
        }
    }

    template<Iterable Container, size_t N>
    [[nodiscard]] ISL_INLINE auto
    join(Container&&container, const CArray<char, N>&separator) -> std::string {
        return join(std::forward<Container>(container), as<std::string_view>(separator));
    }
} // namespace isl

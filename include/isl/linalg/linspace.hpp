#ifndef ISL_LINALG_LINSPACE_HPP
#define ISL_LINALG_LINSPACE_HPP

#include <isl/isl.hpp>
#include <valarray>

namespace isl::linalg
{
    template<std::floating_point T>
    [[nodiscard]] auto linspace(const T begin, const T end, const std::size_t intervals)
        -> std::valarray<T>
    {
        std::valarray<T> result(intervals + 1);

        auto current_interval = begin;
        const auto step = (end - begin) / static_cast<T>(intervals);

        for (std::size_t i = 0; i <= intervals; ++i) {
            result[i] = current_interval;
            current_interval += step;
        }

        return result;
    }
}

#endif /* ISL_LINALG_LINSPACE_HPP */

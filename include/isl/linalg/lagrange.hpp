#ifndef ISL_LAGRANGE_HPP
#define ISL_LAGRANGE_HPP

#include <isl/isl.hpp>
#include <span>

namespace isl
{
    namespace interpolation
    {
        template <std::floating_point T>
        auto lagrange(const std::span<const T> x, const std::span<const T> y, const T x_point) -> T
        {
            assert(x.size() == y.size());

            const auto n = x.size();
            T result{};

            for (std::size_t i = 0; i < n; ++i) {
                auto term = y[i];

                for (std::size_t j = 0; j < n; ++j) {
                    if (i != j) {
                        term *= (x_point - x[j]) / (x[i] - x[j]);
                    }
                }

                result += term;
            }

            return result;
        }
    } // namespace interpolation
} // namespace isl

#endif /* ISL_LAGRANGE_HPP */

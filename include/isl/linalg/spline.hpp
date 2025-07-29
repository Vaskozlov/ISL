#ifndef ISL_SPLINE_HPP
#define ISL_SPLINE_HPP

#include <isl/linalg/thomas.hpp>
#include <span>

namespace isl
{
    namespace interpolation
    {
        template <std::floating_point T>
        struct CubicSplineCoefficients
        {
            T a;
            T b;
            T c;
            T d;
        };

        template <std::floating_point T>
        auto createCubicSpline(const std::span<const T> x, const std::span<const T> y)
            -> std::vector<CubicSplineCoefficients<T>>
        {
            assert(x.size() == y.size() && x.size() >= 1);

            const auto n = x.size();
            std::vector<T> m_a(n);
            std::vector<T> m_b(n);
            std::vector<T> m_c(n);
            std::vector<T> m_d(n);

            m_b.front() = m_b.back() = 1;

            for (std::size_t i = 1; i < n - 1; ++i) {
                m_a[i] = x[i] - x[i - 1];
                m_b[i] = 2 * (x[i + 1] - x[i - 1]);
                m_c[i] = x[i + 1] - x[i];

                m_d[i] = static_cast<T>(3)
                         * ((y[i + 1] - y[i]) / (x[i + 1] - x[i])
                            - (y[i] - y[i - 1]) / (x[i] - x[i - 1]));
            }

            auto c = linalg::solveThomas3(
                m_a, std::move(m_b), m_c, std::move(m_d));

            std::vector<CubicSplineCoefficients<T>> result(n - 1);

            for (std::size_t i = 0; i < n - 1; ++i) {
                const auto h = x[i + 1] - x[i];

                result[i].a = y[i];

                result[i].b = (y[i + 1] - y[i]) / h
                              - h * (c[i + 1] + static_cast<T>(2) * c[i]) / static_cast<T>(3);

                result[i].c = c[i];

                result[i].d = (c[i + 1] - c[i]) / (static_cast<T>(3) * h);
            }

            return result;
        }

        template <std::floating_point T>
        auto countSplineValueInPoint(
            const std::span<const T> x,
            const std::vector<CubicSplineCoefficients<T>> &spline_coefficients,
            const T point) -> T
        {
            const auto it = std::ranges::lower_bound(x, point);
            const auto index = static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(
                std::distance(x.begin(), it) - 1, 0, spline_coefficients.size() - 1));

            const auto &coefficient = spline_coefficients[index];
            const auto h = point - x[index];

            return coefficient.a + coefficient.b * h + coefficient.c * h * h
                   + coefficient.d * h * h * h;
        }
    } // namespace interpolation
} // namespace isl

#endif /* ISL_SPLINE_HPP */

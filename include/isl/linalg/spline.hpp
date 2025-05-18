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
            assert(x.size() == y.size());

            const auto n = x.size();
            std::vector<T> m_a(n);
            std::vector<T> m_b(n);
            std::vector<T> m_c(n);
            std::vector<T> m_d(n);

            m_b.front() = m_b.back() = 1;

            for (std::size_t i = 1; i < n - 1; ++i) {
                m_a[i] = x[i] - x[i - 1];
                m_b[i] = static_cast<T>(2) * (x[i + 1] - x[i - 1]);
                m_c[i] = x[i + 1] - x[i];
                m_d[i] = static_cast<T>(3)
                         * ((y[i + 1] - y[i]) / (x[i + 1] - x[i])
                            - (y[i] - y[i - 1]) / (x[i] - x[i - 1]));
            }

            auto c = linalg::solveThomas3(
                std::move(m_a), std::move(m_b), std::move(m_c), std::move(m_d));

            auto a = std::vector<T>(y.begin(), y.end());

            std::vector<CubicSplineCoefficients<T>> result;
            result.reserve(n - 1);

            for (std::size_t i = 0; i < n - 1; ++i) {
                const auto h = x[i + 1] - x[i];

                const auto b = (a[i + 1] - a[i]) / h
                               - h * (static_cast<T>(2) * c[i] + c[i + 1]) / static_cast<T>(3);

                const auto d = (c[i + 1] - c[i]) / (static_cast<T>(3) * h);

                result.emplace_back(a[i], b, c[i], d);
            }

            return result;
        }
    } // namespace interpolation
} // namespace isl

#endif /* ISL_SPLINE_HPP */

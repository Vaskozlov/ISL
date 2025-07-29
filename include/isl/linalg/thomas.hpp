#ifndef ISL_THOMAS_HPP
#define ISL_THOMAS_HPP

#include <isl/detail/types.hpp>
#include <valarray>
#include <vector>

namespace isl
{
    namespace linalg
    {

        template <std::floating_point T>
        auto solveThomas3(
            const std::vector<T> &a, std::vector<T> b, const std::vector<T> &c, std::vector<T> d)
            -> std::vector<T>
        {
            assert(a.size() == b.size());
            assert(a.size() == c.size());
            assert(a.size() == d.size());

            const auto n = a.size();
            std::vector<T> x(n);

            for (std::size_t i = 1; i < n; ++i) {
                const auto m = a[i] / b[i - 1];
                b[i] -= m * c[i - 1];
                d[i] -= m * d[i - 1];
            }

            x.back() = d.back() / b.back();

            for (ssize_t i = static_cast<ssize_t>(n) - 2; i >= 0; --i) {
                const auto idx = static_cast<std::size_t>(i);
                x[idx] = (d[idx] - c[idx] * x[idx + 1]) / b[idx];
            }

            return x;
        }
    } // namespace linalg
} // namespace isl

#endif /* ISL_THOMAS_HPP */

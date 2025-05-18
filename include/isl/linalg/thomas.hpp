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
        std::vector<T>
            solveThomas3(std::vector<T> a, std::vector<T> b, std::vector<T> c, std::vector<T> d)
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
                x[i] = (d[i] - c[i] * x[i + 1]) / b[i];
            }

            return x;
        }
    } // namespace linalg
} // namespace isl

#endif /* ISL_THOMAS_HPP */

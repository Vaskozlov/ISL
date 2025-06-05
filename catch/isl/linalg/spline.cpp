#include <cmath>
#include <isl/detail/debug/debug.hpp>
#include <isl/linalg/spline.hpp>
#include <limits>

TEST_CASE("IslSpline", "IslLinalg")
{
    using namespace isl::interpolation;

    static constexpr std::array<float, 10> dataX{
        0.0F,
        1.0F,
        2.0F,
        3.0F,
        4.0F,
        4.2F,
        7.0F,
        8.0F,
        9.0F,
        9.5F,
    };

    static constexpr std::array<float, 10> dataY{
        0.0F,
        1.5F,
        0.3F,
        2.8F,
        1.1F,
        3.0F,
        -5.0F,
        0.0F,
        -2.0F,
        2.0F,
    };

    constexpr static std::array<CubicSplineCoefficients<float>, 9> expected_coefficients{
        CubicSplineCoefficients<float>{
                                       .a = 0.0F,
                                       .b = 2.67435188089F,
                                       .c = -2.22044604e-16F,
                                       .d = -1.174351880F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 1.5F,
                                       .b = -0.8487037617F,
                                       .c = -3.523055642F,
                                       .d = 3.17175940441F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 0.3F,
                                       .b = 1.62046316623F,
                                       .c = 5.992222570F,
                                       .d = -5.11268573691F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 2.8F,
                                       .b = -1.733148903F,
                                       .c = -9.3458346400F,
                                       .d = 9.37898354322F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 1.1F,
                                       .b = 7.7121324463F,
                                       .c = 18.791115989F,
                                       .d = -49.2588911072F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 3.0F,
                                       .b = 9.3175119093F,
                                       .c = -10.76421867F,
                                       .d = 2.291474173838F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = -5.0F,
                                       .b = 2.9333598992F,
                                       .c = 8.4841643854F,
                                       .d = -6.4175242847F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = 0.0F,
                                       .b = 0.6491158159F,
                                       .c = -10.7684084F,
                                       .d = 8.119292652F,
                                       },
        CubicSplineCoefficients<float>{
                                       .a = -2.0F,
                                       .b = 3.470176836F,
                                       .c = 13.589469489F,
                                       .d = -9.059646326F,
                                       },
    };

    constexpr static std::array<float, 10> test_points{
        1.1903819F,
        0.5913541F,
        1.9692015F,
        0.7693398F,
        4.888343F,
        1.9556046F,
        -4.632073F,
        -2.214469F,
        -1.3526326F,
        2.0,
    };

    const auto spline_coefficients = isl::interpolation::createCubicSpline<float>(dataX, dataY);

    constexpr static auto precision = 1e-4F;

    for (std::size_t i = 0; i < spline_coefficients.size(); ++i) {
        REQUIRE(std::abs(spline_coefficients.at(i).a - expected_coefficients.at(i).a) < precision);
        REQUIRE(std::abs(spline_coefficients.at(i).b - expected_coefficients.at(i).b) < precision);
        REQUIRE(std::abs(spline_coefficients.at(i).c - expected_coefficients.at(i).c) < precision);
        REQUIRE(std::abs(spline_coefficients.at(i).d - expected_coefficients.at(i).d) < precision);
    }

    float x = 0.5F;

    for (std::size_t i = 0; i < test_points.size(); ++i, x += 1.0F) {
        REQUIRE(
            std::abs(test_points[i] - countSplineValueInPoint<float>(dataX, spline_coefficients, x))
            < precision);
    }
}

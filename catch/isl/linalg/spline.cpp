#include <cmath>
#include <isl/detail/debug/debug.hpp>
#include <isl/linalg/spline.hpp>
#include <limits>

TEST_CASE("IslSpline", "IslLinalg")
{
    using namespace isl::interpolation;

    std::vector<float> dataX{0.0, 1.0, 2.0, 3.0, 4.0, 4.2, 7.0, 8.0, 9.0, 9.5};
    std::vector<float> dataY{0.0, 1.5, 0.3, 2.8, 1.1, 3.0, -5.0, 0.0, -2.0, 2.0};

    constexpr static std::array<CubicSplineCoefficients<float>, 9> expected_coefficients{
        CubicSplineCoefficients{0.0F,  2.6743518808904394F, -2.220446049250e-16F, -1.1743518808904F  },
        CubicSplineCoefficients{1.5F,  -0.84870376178087F,  -3.5230556426713F,    3.1717594044521F   },
        CubicSplineCoefficients{0.3F,  1.6204631662330753F, 5.992222570685272F,   -5.112685736918348F},
        CubicSplineCoefficients{2.8F,  -1.73314890315142F,  -9.34583464006977F,   9.3789835432216F   },
        CubicSplineCoefficients{1.1F,  7.712132446372618F,  18.791115989593784F,  -49.25889110728461F},
        CubicSplineCoefficients{3.0F,  9.317511909335975F,  -10.76421867477701F,  2.2914741738388775F},
        CubicSplineCoefficients{-5.0F, 2.933359899275105F,  8.484164385469555F,   -6.41752428474466F },
        CubicSplineCoefficients{0.0F,  0.64911581598023F,   -10.7684084687644F,   8.1192926527841F   },
        CubicSplineCoefficients{-2.0F, 3.47017683680396F,   13.5894694895881F,    -9.0596463263920F  },
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

    for (std::size_t i = 0; i < spline_coefficients.size(); ++i) {
        REQUIRE(std::abs(spline_coefficients[i].a - expected_coefficients[i].a) < 1e-4F);
        REQUIRE(std::abs(spline_coefficients[i].b - expected_coefficients[i].b) < 1e-4F);
        REQUIRE(std::abs(spline_coefficients[i].c - expected_coefficients[i].c) < 1e-4F);
        REQUIRE(std::abs(spline_coefficients[i].d - expected_coefficients[i].d) < 1e-4F);
    }

    float x = 0.5F;

    for (std::size_t i = 0; i < test_points.size(); ++i, x += 1.0F) {
        REQUIRE(
            std::abs(test_points[i] - countSplineValueInPoint<float>(dataX, spline_coefficients, x))
            < 1e-4F);
    }
}

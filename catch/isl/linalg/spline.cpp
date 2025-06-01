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
        CubicSplineCoefficients<float>{0.0F,  2.67435188089F,   -2.2204460492e-16F, -1.17435188089F  },
        CubicSplineCoefficients<float>{1.5F,  -0.848703761780F, -3.52305564267F,    3.17175940441F   },
        CubicSplineCoefficients<float>{0.3F,  1.6204631662330F, 5.99222257068F,     -5.1126857369183F},
        CubicSplineCoefficients<float>{2.8F,  -1.73314890315F,  -9.345834640069F,   9.37898354322F   },
        CubicSplineCoefficients<float>{1.1F,  7.712132446372F,  18.7911159895937F,  -49.258891107284F},
        CubicSplineCoefficients<float>{3.0F,  9.317511909335F,  -10.764218674777F,  2.29147417383887F},
        CubicSplineCoefficients<float>{-5.0F, 2.933359899275F,  8.4841643854695F,   -6.417524284744F },
        CubicSplineCoefficients<float>{0.0F,  0.649115815980F,  -10.76840846876F,   8.11929265278F   },
        CubicSplineCoefficients<float>{-2.0F, 3.47017683680F,   13.58946948958F,    -9.05964632639F  },
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

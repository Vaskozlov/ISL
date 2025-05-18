#include <cmath>
#include <isl/detail/debug/debug.hpp>
#include <isl/linalg/spline.hpp>
#include <limits>

TEST_CASE("IslSpline", "IslLinalg")
{
    constexpr static auto x = std::array{0.0F, 1.0F, 2.0F};
    constexpr static auto y = std::array{1.0F, 3.0F, 2.0F};

    auto spline_coefficients = isl::interpolation::createCubicSpline<float>(
        std::span{x.data(), x.size()}, std::span{y.data(), y.size()});

    constexpr static auto first_expected = std::array{1.0F, 2.75F, 0.0F, -0.75F};
    constexpr static auto second_expected = std::array{3.0F, 0.5F, -2.25F, 0.75F};

    REQUIRE(spline_coefficients.size() == 2);

    REQUIRE(std::abs(first_expected[0] - spline_coefficients[0].a) < 1e-4F);
    REQUIRE(std::abs(first_expected[1] - spline_coefficients[0].b) < 1e-4F);
    REQUIRE(std::abs(first_expected[2] - spline_coefficients[0].c) < 1e-4F);
    REQUIRE(std::abs(first_expected[3] - spline_coefficients[0].d) < 1e-4F);

    REQUIRE(std::abs(second_expected[0] - spline_coefficients[1].a) < 1e-4F);
    REQUIRE(std::abs(second_expected[1] - spline_coefficients[1].b) < 1e-4F);
    REQUIRE(std::abs(second_expected[2] - spline_coefficients[1].c) < 1e-4F);
    REQUIRE(std::abs(second_expected[3] - spline_coefficients[1].d) < 1e-4F);
}

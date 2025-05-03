#include <cmath>
#include <isl/detail/debug/debug.hpp>
#include <isl/float16.hpp>
#include <limits>

TEST_CASE("Fp16Value", "IslFloat16")
{
    REQUIRE(static_cast<float>(isl::float16{42.0F}) == 42.0F);
}

TEST_CASE("Fp16NaN", "IslFloat16")
{
    constexpr auto nan = std::numeric_limits<float>::quiet_NaN();

    REQUIRE(std::isnan(static_cast<float>(isl::float16{nan})));
    REQUIRE(std::isnan(static_cast<float>(isl::float16{-nan})));
}

TEST_CASE("Fp16Inf", "IslFloat16")
{
    constexpr auto inf = std::numeric_limits<float>::infinity();

    REQUIRE(std::isinf(static_cast<float>(isl::float16{inf})));
    REQUIRE(std::isinf(static_cast<float>(isl::float16{-inf})));
}

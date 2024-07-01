#include "test_structures.hpp"
#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>

// NOLINTBEGIN

TEST_CASE("TestIsOnSimilarTypes", "[IsConversion]")
{
    using namespace isl;
    constexpr auto value = as<i64>(42);

    STATIC_REQUIRE(isl::is<i64>(value));
}

TEST_CASE("TestIsOnDifferentTypes", "[IsConversion]")
{
    using namespace isl;
    constexpr auto value = as<i64>(42);

    STATIC_REQUIRE(!isl::is<i32>(value));
}

TEST_CASE("TestIsOnPointer", "[IsConversion]")
{
    using namespace isl;

    auto b = B{};
    auto *a = as<const A *>(&b);
    auto *b_from_a = as<const B *>(a);

    REQUIRE(is< A *>(&b));
    REQUIRE(is< B *>(&b));
    REQUIRE(is< A *>(a));
    REQUIRE(is< B *>(a));
    REQUIRE(is< A *>(b_from_a));
    REQUIRE(is< B *>(b_from_a));

    REQUIRE(is< C *>(a));
    REQUIRE(is< C *>(&b));
    REQUIRE(is< C *>(b_from_a));
}

TEST_CASE("TestIsOnReferences", "[IsConversion]")
{
    using namespace isl;

    constexpr static auto b = B{};

    constexpr static auto &a = as<const A &>(b);
    constexpr static auto &b_from_a = as<const B &>(a);

    STATIC_REQUIRE(is<const A &>(b));
    STATIC_REQUIRE(is<const B &>(b));
    STATIC_REQUIRE(is<const A &>(a));
    STATIC_REQUIRE(is<const B &>(a));
    STATIC_REQUIRE(is<const A &>(b_from_a));
    STATIC_REQUIRE(is<const B &>(b_from_a));

    STATIC_REQUIRE_FALSE(is<C &>(a));
    STATIC_REQUIRE_FALSE(is<C &>(b));
    STATIC_REQUIRE_FALSE(is<C &>(b_from_a));
}

// NOLINTEND

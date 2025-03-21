#include "test_structures.hpp"

#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>

// NOLINTBEGIN

TEST_CASE("TestIsOnSimilarTypes", "[IsConversion]")
{
    using namespace isl;
    auto value = as<i64>(42);

    REQUIRE(isl::is<i64>(value));
}

TEST_CASE("TestIsOnPointer", "[IsConversion]")
{
    using namespace isl;

    const auto b = B{};
    const auto *a = as<const A *>(&b);
    const auto *b_from_a = as<const B *>(a);

    REQUIRE(is<A *>(&b));
    REQUIRE(is<B *>(&b));
    REQUIRE(is<A *>(a));
    REQUIRE(is<B *>(a));
    REQUIRE(is<A *>(b_from_a));
    REQUIRE(is<B *>(b_from_a));

    REQUIRE_FALSE(is<C *>(a));
    REQUIRE_FALSE(is<C *>(&b));
    REQUIRE_FALSE(is<C *>(b_from_a));
}

TEST_CASE("TestIsOnReferences", "[IsConversion]")
{
    using namespace isl;

    const auto b = B{};

    const auto &a = as<const A &>(b);
    const auto &b_from_a = as<const B &>(a);

    REQUIRE(is<const A &>(b));
    REQUIRE(is<const B &>(b));
    REQUIRE(is<const A &>(a));
    REQUIRE(is<const B &>(a));
    REQUIRE(is<const A &>(b_from_a));
    REQUIRE(is<const B &>(b_from_a));

    REQUIRE_FALSE(is<C &>(a));
    REQUIRE_FALSE(is<C &>(b));
    REQUIRE_FALSE(is<C &>(b_from_a));
}

// NOLINTEND

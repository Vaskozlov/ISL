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

    constexpr static auto b = B{};
    constexpr auto *a = as<const A *>(&b);
    constexpr auto *b_from_a = as<const B *>(a);

    STATIC_REQUIRE(is<A *>(&b));
    STATIC_REQUIRE(is<B *>(&b));
    STATIC_REQUIRE(is<A *>(a));
    STATIC_REQUIRE(is<B *>(a));
    STATIC_REQUIRE(is<A *>(b_from_a));
    STATIC_REQUIRE(is<B *>(b_from_a));

    STATIC_REQUIRE_FALSE(is<C *>(a));
    STATIC_REQUIRE_FALSE(is<C *>(&b));
    STATIC_REQUIRE_FALSE(is<C *>(b_from_a));
}

TEST_CASE("TestIsOnReferences", "[IsConversion]")
{
    using namespace isl;

    constexpr static auto b = B{};

    constexpr auto &a = as<const A &>(b);
    constexpr auto &b_from_a = as<const B &>(a);

    STATIC_REQUIRE(is<A &>(b));
    STATIC_REQUIRE(is<B &>(b));
    STATIC_REQUIRE(is<A &>(a));
    STATIC_REQUIRE(is<B &>(a));
    STATIC_REQUIRE(is<A &>(b_from_a));
    STATIC_REQUIRE(is<B &>(b_from_a));

    STATIC_REQUIRE_FALSE(is<C &>(a));
    STATIC_REQUIRE_FALSE(is<C &>(b));
    STATIC_REQUIRE_FALSE(is<C &>(b_from_a));
}

// NOLINTEND

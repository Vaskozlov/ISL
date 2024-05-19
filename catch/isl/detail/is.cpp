#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>
#include "test_structures.hpp"

// NOLINTBEGIN

TEST_CASE("TestIsOnSimilarTypes", "[IsConversion]")
{
    using namespace isl;
    auto value = as<i64>(42);

    REQUIRE(isl::is<i64>(value));
}

TEST_CASE("TestIsOnDifferentTypes", "[IsConversion]")
{
    using namespace isl;
    auto value = as<i64>(42);

    REQUIRE(!isl::is<i32>(value));
}

TEST_CASE("TestIsOnPointer", "[IsConversion]")
{
    using namespace isl;

    auto b = B{};
    auto *a = as<A *>(&b);
    auto *b_from_a = as<B *>(a);

    REQUIRE(is<A*>(&b));
    REQUIRE(is<B*>(&b));
    REQUIRE(is<A*>(a));
    REQUIRE(is<B*>(a));
    REQUIRE(is<A*>(b_from_a));
    REQUIRE(is<B*>(b_from_a));
}

TEST_CASE("TestIsOnReferences", "[IsConversion]")
{
    using namespace isl;

    auto b = B{};

    auto &a = as<A &>(b);
    auto &b_from_a = as<B &>(a);

    REQUIRE(is<A&>(b));
    REQUIRE(is<B&>(b));
    REQUIRE(is<A&>(a));
    REQUIRE(is<B&>(a));
    REQUIRE(is<A&>(b_from_a));
    REQUIRE(is<B&>(b_from_a));
}

// NOLINTEND

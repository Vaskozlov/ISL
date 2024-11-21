#include "test_structures.hpp"
#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>

// NOLINTBEGIN

TEST_CASE("TestAsOnInts", "[AsConversion]")
{
    using namespace isl;

    constexpr auto forty_two = as<i64>(42);

    static_assert(std::same_as<const i64, decltype(forty_two)>);
    STATIC_REQUIRE(forty_two == 42);
}

TEST_CASE("TestAsOnFloats", "[AsConversion]")
{
    using namespace isl;

    constexpr auto forty_two = as<f32>(42);

    static_assert(std::same_as<const f32, decltype(forty_two)>);
    STATIC_REQUIRE((forty_two - 42.0F) <= 1e-10F);
}

TEST_CASE("TestAsOnPointer", "[AsConversion]")
{
    using namespace isl;

    auto b = B{};
    auto *a = as<A *>(&b);

    REQUIRE(a != nullptr);
    static_assert(std::same_as<decltype(a), A *>);

    auto *b_from_a = as<B *>(a);

    REQUIRE(b_from_a != nullptr);
    static_assert(std::same_as<decltype(b_from_a), B *>);

    REQUIRE(a->getId() == 20);
    REQUIRE(b_from_a->getId() == 20);
}

TEST_CASE("TestAsOnReferences", "[AsConversion]")
{
    using namespace isl;

    auto b = B{};

    auto &a = as<const A &>(b);
    static_assert(std::same_as<decltype(a), const A &>);

    const auto &b_from_a = as<const B &>(a);
    static_assert(std::same_as<decltype(b_from_a), const B &>);

    REQUIRE(a.getId() == 20);
    REQUIRE(b_from_a.getId() == 20);
}

// NOLINTEND

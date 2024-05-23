#include "test_structures.hpp"
#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>

// NOLINTBEGIN

TEST_CASE("TestAsOnInts", "[AsConversion]")
{
    using namespace isl;

    constexpr auto forty_two = as<i64>(42);

    static_assert(std::is_same_v<const i64, decltype(forty_two)>);
    STATIC_REQUIRE(forty_two == 42);
}

TEST_CASE("TestAsOnFloats", "[AsConversion]")
{
    using namespace isl;

    constexpr auto forty_two = as<f32>(42);

    static_assert(std::is_same_v<const f32, decltype(forty_two)>);
    STATIC_REQUIRE((forty_two - 42.0F) <= 1e-10F);
}

TEST_CASE("TestAsOnPointer", "[AsConversion]")
{
    using namespace isl;

    static constexpr auto b = B{};
    constexpr auto *a = as<const A *>(&b);

    STATIC_REQUIRE(a != nullptr);
    static_assert(std::is_same_v<decltype(a), const A *const>);

    constexpr auto *b_from_a = as<const B *>(a);

    STATIC_REQUIRE(b_from_a != nullptr);
    static_assert(std::is_same_v<decltype(b_from_a), const B *const>);

    STATIC_REQUIRE(a->getId() == 20);
    STATIC_REQUIRE(b_from_a->getId() == 20);
}

TEST_CASE("TestAsOnReferences", "[AsConversion]")
{
    using namespace isl;

    constexpr static auto b = B{};

    constexpr auto &a = as<const A &>(b);
    static_assert(std::is_same_v<decltype(a), const A &>);

    const auto &b_from_a = as<const B &>(a);
    static_assert(std::is_same_v<decltype(b_from_a), const B &>);

    STATIC_REQUIRE(a.getId() == 20);
    STATIC_REQUIRE(b_from_a.getId() == 20);
}

// NOLINTEND

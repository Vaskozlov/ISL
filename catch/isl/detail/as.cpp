#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>

// NOLINTBEGIN

struct A
{
    A() = default;

    virtual ~A() = default;

    virtual auto getId() const -> int
    {
        return 10;
    }
};

struct B : A
{
    auto getId() const -> int override
    {
        return 20;
    }
};

TEST_CASE("TestAsOnInts", "[AsConversion]")
{
    using namespace isl;

    auto forty_two = as<i64>(42);

    static_assert(std::is_same_v<i64, decltype(forty_two)>);
    REQUIRE(forty_two == 42);
}

TEST_CASE("TestAsOnFloats", "[AsConversion]")
{
    using namespace isl;

    auto forty_two = as<f32>(42);

    static_assert(std::is_same_v<f32, decltype(forty_two)>);
    REQUIRE((forty_two - 42.0F) <= std::numeric_limits<f32>::epsilon());
}

TEST_CASE("TestAsOnPointer", "[AsConversion]")
{
    using namespace isl;

    auto b = B{};
    auto *a = as<A *>(&b);

    REQUIRE(a != nullptr);
    static_assert(std::is_same_v<decltype(a), A *>);

    auto *b_from_a = as<B *>(a);

    REQUIRE(b_from_a != nullptr);
    static_assert(std::is_same_v<decltype(b_from_a), B *>);

    REQUIRE(a->getId() == 20);
    REQUIRE(b_from_a->getId() == 20);
}

TEST_CASE("TestAsOnReferences", "[AsConversion]")
{
    using namespace isl;

    auto b = B{};

    auto &a = as<A &>(b);
    static_assert(std::is_same_v<decltype(a), A &>);

    auto &b_from_a = as<B &>(a);
    static_assert(std::is_same_v<decltype(b_from_a), B &>);

    REQUIRE(a.getId() == 20);
    REQUIRE(b_from_a.getId() == 20);
}

// NOLINTEND

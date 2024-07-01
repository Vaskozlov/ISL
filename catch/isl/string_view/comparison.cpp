#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewComparisonEqual", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "Hello, World!";

    STATIC_REQUIRE(own_string == std_string);
}

TEST_CASE("StringViewComparisonLessFirstLess", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "hello, World!";

    STATIC_REQUIRE(own_string > std_string);
    STATIC_REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonLessFirstGreater", "[StringView]")
{
    constexpr static std::string_view std_string = "hello, World!";
    constexpr static isl::string_view own_string = "Hello, World!";

    STATIC_REQUIRE(own_string < std_string);
    STATIC_REQUIRE(own_string <= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstLess", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "hello, World!";

    STATIC_REQUIRE(own_string > std_string);
    STATIC_REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstGreater", "[StringView]")
{
    constexpr std::string_view std_string = "Hello, World";
    constexpr isl::string_view own_string = "Hello, World!";

    STATIC_REQUIRE(own_string > std_string);
    STATIC_REQUIRE(own_string >= std_string);
}

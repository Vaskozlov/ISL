#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("StringViewComparisonEqual", "[StringView]")
{
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(own_string == std_string);
}

TEST_CASE("StringViewComparisonLessFirstLess", "[StringView]")
{
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "hello, World!";

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonLessFirstGreater", "[StringView]")
{
    std::string_view std_string = "hello, World!";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(own_string < std_string);
    REQUIRE(own_string <= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstLess", "[StringView]")
{
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "hello, World!";

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstGreater", "[StringView]")
{
    std::string_view std_string = "Hello, World";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

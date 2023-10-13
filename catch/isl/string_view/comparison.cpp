#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewComparisonEqual", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    REQUIRE(own_string == std_string);
}

TEST_CASE("StringViewComparisonLessFirstLess", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "hello, World!"_sv;

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonLessFirstGreater", "[StringView]")
{
    DEBUG_VAR std_string = "hello, World!"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    REQUIRE(own_string < std_string);
    REQUIRE(own_string <= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstLess", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "hello, World!"_sv;

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

TEST_CASE("StringViewComparisonGreaterFirstGreater", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    REQUIRE(own_string > std_string);
    REQUIRE(own_string >= std_string);
}

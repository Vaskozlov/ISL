#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace std::string_literals;

TEST_CASE("StringViewConstructionEmptyArray", "[StringView]")
{
    DEBUG_VAR string = string_view{""};
    REQUIRE(string.empty());
}

TEST_CASE("StringViewConstructionArray", "[StringView]")
{
    DEBUG_VAR string = string_view{"Hello, World!"};
    REQUIRE(string.size() == 13);
}

TEST_CASE("StringViewConstructionEmptyPointer", "[StringView]")
{
    const char *str = "";
    DEBUG_VAR string = string_view{str};
    REQUIRE(string.empty());
}

TEST_CASE("StringViewConstructionPointer", "[StringView]")
{
    const char *str = "Hello, World!";
    DEBUG_VAR string = string_view{str};
    REQUIRE(string.size() == 13);
}

TEST_CASE("StringViewConstructionFromTwoPointers", "[StringView]")
{
    auto str = "Hello, World!"s;
    DEBUG_VAR string = string_view{str.data(), str.data() + str.size()};// NOLINT
    REQUIRE(string.size() == 13);
}

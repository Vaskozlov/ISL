#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>


TEST_CASE("StringViewConstructionEmptyArray", "[StringView]")
{
    isl::string_view string{""};
    REQUIRE(string.empty());
}

TEST_CASE("StringViewConstructionArray", "[StringView]")
{
    isl::string_view string{"Hello, World!"};
    REQUIRE(string.size() == 13);
}

TEST_CASE("StringViewConstructionEmptyPointer", "[StringView]")
{
    const char *str = "";
    isl::string_view string{str};
    REQUIRE(string.empty());
}

TEST_CASE("StringViewConstructionPointer", "[StringView]")
{
    const char *str = "Hello, World!";
    isl::string_view string{str};
    REQUIRE(string.size() == 13);
}

TEST_CASE("StringViewConstructionFromTwoPointers", "[StringView]")
{
    std::string str = "Hello, World!";
    isl::string_view string{str.data(), str.data() + str.size()};// NOLINT
    REQUIRE(string.size() == 13);
}

#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>


TEST_CASE("StringViewConstructionEmptyArray", "[StringView]")
{
    constexpr static isl::string_view string{""};
    STATIC_REQUIRE(string.empty());
}

TEST_CASE("StringViewConstructionArray", "[StringView]")
{
    constexpr static isl::string_view string{"Hello, World!"};
    STATIC_REQUIRE(string.size() == 13);
}

TEST_CASE("StringViewConstructionFromTwoPointers", "[StringView]")
{
    std::string str = "Hello, World!";
    isl::string_view string{str.data(), str.data() + str.size()};// NOLINT
    REQUIRE(string.size() == 13);
}

#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewSubstr", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "Hello, World!";

    STATIC_REQUIRE(own_string.substr(0, 3) == std_string.substr(0, 3));
    STATIC_REQUIRE(own_string.substr(3, 3) == std_string.substr(3, 3));
    STATIC_REQUIRE(own_string.substr(3, 20) == std_string.substr(3, 20));
    STATIC_REQUIRE(own_string.substr(10, 20) == std_string.substr(10, 20));
}

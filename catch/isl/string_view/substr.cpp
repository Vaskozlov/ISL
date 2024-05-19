#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewSubstr", "[StringView]")
{
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(own_string.substr(0, 3) == std_string.substr(0, 3));
    REQUIRE(own_string.substr(3, 3) == std_string.substr(3, 3));
    REQUIRE(own_string.substr(3, 20) == std_string.substr(3, 20));
    REQUIRE(own_string.substr(10, 20) == std_string.substr(10, 20));
}

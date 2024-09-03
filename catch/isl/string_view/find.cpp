#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewFind", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "Hello, World!";

    STATIC_REQUIRE(own_string.find('H') == std_string.find('H'));
    STATIC_REQUIRE(own_string.find(',') == std_string.find(','));
    STATIC_REQUIRE(own_string.find('!') == std_string.find('!'));

    STATIC_REQUIRE(own_string.find('A') == isl::string_view::npos);

    STATIC_REQUIRE(own_string.find('l', 6) == std_string.find('l', 6));
    STATIC_REQUIRE(own_string.find('l', own_string.begin() + 6) == std_string.find('l', 6));
}

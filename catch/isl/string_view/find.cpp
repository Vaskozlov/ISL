#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("StringViewFind", "[StringView]") {
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(own_string.find('H').value() == std_string.find('H'));
    REQUIRE(own_string.find(',').value() == std_string.find(','));
    REQUIRE(own_string.find('!').value() == std_string.find('!'));

    REQUIRE(!own_string.find('A').has_value());

    REQUIRE(own_string.find('l', 6).value() == std_string.find('l', 6));
    REQUIRE(own_string.find('l', own_string.begin() + 6).value() == std_string.find('l', 6));
}

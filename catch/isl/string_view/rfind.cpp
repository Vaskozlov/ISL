#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("StringViewRfind", "[StringView]") {
    std::string_view std_string = "Hello, World!";
    isl::string_view isl_string = "Hello, World!";

    REQUIRE(isl_string.rfind('H').value() == std_string.rfind('H'));
    REQUIRE(isl_string.rfind(',').value() == std_string.rfind(','));
    REQUIRE(isl_string.rfind('!').value() == std_string.rfind('!'));
    REQUIRE(!isl_string.rfind('A').has_value());
}

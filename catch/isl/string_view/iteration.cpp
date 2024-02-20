#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("StringViewIteration", "[StringView]") {
    std::string_view std_string = "Hello, World!";
    isl::string_view isl_string = "Hello, World!";

    REQUIRE(
        std::equal(std_string.cbegin(), std_string.cend(), isl_string.cbegin(), isl_string.cend()));
    REQUIRE(std::equal(std_string.begin(), std_string.end(), isl_string.begin()));
}

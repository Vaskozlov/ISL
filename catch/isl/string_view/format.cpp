#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("StringViewFormat", "[StringView]")
{
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "Hello, World!";

    REQUIRE(std::format("{}", std_string) == std::format("{}", own_string));
}

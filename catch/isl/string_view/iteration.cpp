#include <algorithm>
#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewIteration", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view isl_string = "Hello, World!";

    STATIC_REQUIRE(std::ranges::equal(std_string, isl_string));
}

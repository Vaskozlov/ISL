#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewRfind", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view isl_string = "Hello, World!";

    STATIC_REQUIRE(isl_string.rfind('H').value() == std_string.rfind('H'));
    STATIC_REQUIRE(isl_string.rfind(',').value() == std_string.rfind(','));
    STATIC_REQUIRE(isl_string.rfind('!').value() == std_string.rfind('!'));
    STATIC_REQUIRE(!isl_string.rfind('A').has_value());
}

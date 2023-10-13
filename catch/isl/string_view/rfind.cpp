#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl::string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewRfind", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR isl_string = "Hello, World!"_sv;

    REQUIRE(isl_string.rfind('H').value() == std_string.rfind('H'));
    REQUIRE(isl_string.rfind(',').value() == std_string.rfind(','));
    REQUIRE(isl_string.rfind('!').value() == std_string.rfind('!'));
    REQUIRE(!isl_string.rfind('A').has_value());
}

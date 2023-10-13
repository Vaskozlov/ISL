#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewFind", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    REQUIRE(own_string.find('H').value() == std_string.find('H'));
    REQUIRE(own_string.find(',').value() == std_string.find(','));
    REQUIRE(own_string.find('!').value() == std_string.find('!'));

    REQUIRE(!own_string.find('A').has_value());

    REQUIRE(own_string.find('l', 6).value() == std_string.find('l', 6));
    REQUIRE(own_string.find('l', own_string.begin() + 6).value() == std_string.find('l', 6));
}

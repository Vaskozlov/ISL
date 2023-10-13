#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;

TEST_CASE("StringViewOpenCloseFind", "[StringView]")
{
    DEBUG_VAR own_string = "(Hello(, )World)!"_sv;
    REQUIRE(own_string.findMatchingPair('(', ')').value() == own_string.rfind(')'));
}

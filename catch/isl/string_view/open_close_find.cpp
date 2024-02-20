#include <isl/detail/debug/debug.hpp>
import isl;


TEST_CASE("StringViewOpenCloseFind", "[StringView]") {
    using namespace isl::string_view_literals;

    DEBUG_VAR own_string = "(Hello(, )World)!"_sv;
    REQUIRE(own_string.findMatchingPair('(', ')').value() == own_string.rfind(')'));
}

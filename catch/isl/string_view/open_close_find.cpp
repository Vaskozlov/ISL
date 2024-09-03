#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>


TEST_CASE("StringViewOpenCloseFind", "[StringView]")
{
    using namespace isl::string_view_literals;

    constexpr static auto own_string = "(Hello(, )World)!"_sv;
    STATIC_REQUIRE(own_string.findRangeEnd('(', ')') == own_string.rfind(')'));
}

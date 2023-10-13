#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewLeftStrip", "[StringView]")
{
    DEBUG_VAR own_string = "115223311"_sv;
    REQUIRE(own_string.leftStrip("15") == "223311"_sv);
}

TEST_CASE("StringViewRightStrip", "[StringView]")
{
    DEBUG_VAR own_string = "1152233611"_sv;
    REQUIRE(own_string.rightStrip("16") == "1152233"_sv);
}

TEST_CASE("StringViewStrip", "[StringView]")
{
    DEBUG_VAR own_string = "1152233611"_sv;
    REQUIRE(own_string.strip("156") == "2233"_sv);
}

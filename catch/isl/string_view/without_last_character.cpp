#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewWithoutLastCharacter", "[StringView]")
{
    REQUIRE("Hello, World!"_sv.removeLastCharacter() == "Hello, World"_sv);
    REQUIRE(""_sv.removeLastCharacter() == ""_sv);
}

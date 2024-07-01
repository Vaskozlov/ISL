#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>


TEST_CASE("StringViewWithoutLastCharacter", "[StringView]")
{
    using namespace isl::string_view_literals;

    STATIC_REQUIRE("Hello, World!"_sv.removeLastCharacter() == "Hello, World"_sv);
    STATIC_REQUIRE(""_sv.removeLastCharacter().empty());
}

#include <isl/detail/debug/debug.hpp>
import isl;


TEST_CASE("StringViewWithoutLastCharacter", "[StringView]") {
    using namespace isl::string_view_literals;

    REQUIRE("Hello, World!"_sv.removeLastCharacter() == "Hello, World"_sv);
    REQUIRE(""_sv.removeLastCharacter() == ""_sv);
}

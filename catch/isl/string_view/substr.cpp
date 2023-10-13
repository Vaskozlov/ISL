#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewSubstr", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    REQUIRE(own_string.substr(0, 3) == std_string.substr(0, 3));
    REQUIRE(own_string.substr(3, 3) == std_string.substr(3, 3));
    REQUIRE(own_string.substr(3, 20) == std_string.substr(3, 20));
    REQUIRE(own_string.substr(10, 20) == std_string.substr(10, 20));
}

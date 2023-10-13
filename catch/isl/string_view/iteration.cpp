#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewIteration", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR isl_string = "Hello, World!"_sv;

    REQUIRE(
        std::equal(std_string.cbegin(), std_string.cend(), isl_string.cbegin(), isl_string.cend()));
    REQUIRE(std::equal(std_string.begin(), std_string.end(), isl_string.begin()));
}

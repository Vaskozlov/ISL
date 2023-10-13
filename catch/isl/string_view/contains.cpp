#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

using namespace isl;
using namespace string_view_literals;
using namespace std::string_view_literals;

TEST_CASE("StringViewContains", "[StringView]")
{
    DEBUG_VAR std_string = "Hello, World!"sv;
    DEBUG_VAR own_string = "Hello, World!"_sv;

    for (auto chr : std_string) {
        auto std_string_contains_chr = std_string.find(chr) != std::string::npos;
        REQUIRE(std_string_contains_chr == own_string.contains(chr));
    }
}

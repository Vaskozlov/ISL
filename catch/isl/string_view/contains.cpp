#include <algorithm>
#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewContains", "[StringView]")
{
    constexpr static std::string_view std_string = "Hello, World!";
    constexpr static isl::string_view own_string = "Hello, World!";

    for (auto chr : std_string) {
        auto std_string_contains_chr = std_string.find(chr) != isl::String::npos;
        REQUIRE(std_string_contains_chr == own_string.contains(chr));
    }

    STATIC_REQUIRE(std::ranges::all_of(std_string, [](char chr) {
        auto std_string_contains_chr = std_string.find(chr) != isl::String::npos;
        return std_string_contains_chr == own_string.contains(chr);
    }));
}

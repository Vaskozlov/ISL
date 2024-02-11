#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewContains", "[StringView]") {
    std::string_view std_string = "Hello, World!";
    isl::string_view own_string = "Hello, World!";

    for (auto chr: std_string) {
        auto std_string_contains_chr = std_string.find(chr) != std::string::npos;
        REQUIRE(std_string_contains_chr == own_string.contains(chr));
    }
}

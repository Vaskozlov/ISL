#include <isl/detail/debug/debug.hpp>
#include <isl/utf8.hpp>

TEST_CASE("OneBytesUnicodeConversion", "[UnicodeConversion]") {
    auto test = std::string{};
    isl::utf8::appendUtf32ToUtf8Container(std::back_inserter(test), U'\u000F');
    REQUIRE(test == "\u000F");

    test.clear();
    isl::utf8::appendUtf32ToUtf8Container(std::back_inserter(test), 'A');
    REQUIRE(test == "A");
}

TEST_CASE("TwoBytesUnicodeConversion", "[UnicodeConversion]") {
    auto test = std::string{};
    isl::utf8::appendUtf32ToUtf8Container(std::back_inserter(test), U'\u00FD');
    REQUIRE(test == "\u00FD");
}

TEST_CASE("TreeBytesUnicodeConversion", "[UnicodeConversion]") {
    auto test = std::string{};
    isl::utf8::appendUtf32ToUtf8Container(std::back_inserter(test), U'\uFCAF');
    REQUIRE(test == "\uFCAF");
}

TEST_CASE("FourBytesUnicodeConversion", "[UnicodeConversion]") {
    auto test = std::string{};
    isl::utf8::appendUtf32ToUtf8Container(std::back_inserter(test), U'\U0010FDFE');
    REQUIRE(test == "\U0010FDFE");
}

#include <isl/detail/debug/debug.hpp>
#include <isl/utf8.hpp>

TEST_CASE("OneBytesUnicodeConversion", "[UnicodeConversion]")
{
    using namespace isl;

    auto test = std::string{};
    utf8::appendUtf32ToUtf8Container(test, U'\u000F');
    REQUIRE(test == "\u000F");

    test.clear();
    utf8::appendUtf32ToUtf8Container(test, 'A');
    REQUIRE(test == "A");
}

TEST_CASE("TwoBytesUnicodeConversion", "[UnicodeConversion]")
{
    using namespace isl;

    auto test = std::string{};
    utf8::appendUtf32ToUtf8Container(test, U'\u00FD');
    REQUIRE(test == "\u00FD");
}

TEST_CASE("TreeBytesUnicodeConversion", "[UnicodeConversion]")
{
    using namespace isl;

    auto test = std::string{};
    utf8::appendUtf32ToUtf8Container(test, U'\uFCAF');
    REQUIRE(test == "\uFCAF");
}

TEST_CASE("FourBytesUnicodeConversion", "[UnicodeConversion]")
{
    using namespace isl;

    auto test = std::string{};
    utf8::appendUtf32ToUtf8Container(test, U'\U0010FDFE');
    REQUIRE(test == "\U0010FDFE");
}

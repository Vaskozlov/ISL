#include <isl/detail/debug/debug.hpp>
#include <isl/utf_set.hpp>

// NOLINTBEGIN

TEST_CASE("UtfSetAsciiTest", "[UtfSet]") {
    auto set = isl::UtfSet{};

    set.set(0);
    REQUIRE(set.at(0));

    set.set(0, false);
    REQUIRE(!set.at(0));

    set.set(127);
    REQUIRE(set.at(127));

    set.set(isl::Range<char32_t>{10, 20 + 1}, true);

    for (char32_t i = 10; i <= 20; ++i) {
        REQUIRE(set.at(i));
    }

    REQUIRE(!set.at(21));
    set.set(isl::Range<char32_t>{10, 20 + 1}, false);

    for (char32_t i = 10; i <= 20; ++i) {
        REQUIRE(!set.at(i));
    }
}

TEST_CASE("UtfSetUtfTest", "[UtfSet]") {
    auto utf_set = isl::UtfSet{};

    utf_set.set(U'\uFFFF');
    REQUIRE(utf_set.at(U'\uFFFF'));

    utf_set.set(U'\uFFFF', false);
    REQUIRE(!utf_set.at(U'\uFFFF'));

    utf_set.set({U'\u10FF', U'\u1200' + 1}, true);

    for (char32_t i = U'\u10FF'; i <= U'\u1200'; ++i) {
        REQUIRE(utf_set.at(i));
    }

    REQUIRE(!utf_set.at(U'\u1201'));
    utf_set.set({U'\u10FF', U'\u1200' + 1}, false);

    for (char32_t i = U'\u10FF'; i <= U'\u1200'; ++i) {
        REQUIRE(!utf_set.at(i));
    }
}

// NOLINTEND

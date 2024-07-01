#include <isl/detail/debug/debug.hpp>
#include <isl/string_view.hpp>

TEST_CASE("StringViewLeftStrip", "[StringView]")
{
    using namespace isl::string_view_literals;

    constexpr static auto own_string = "115223311"_sv;
    STATIC_REQUIRE(own_string.leftStrip("15") == "223311"_sv);
}

TEST_CASE("StringViewRightStrip", "[StringView]")
{
    using namespace isl::string_view_literals;

    constexpr static auto own_string = "1152233611"_sv;
    STATIC_REQUIRE(own_string.rightStrip("16") == "1152233"_sv);
}

TEST_CASE("StringViewStrip", "[StringView]")
{
    using namespace isl::string_view_literals;

    constexpr static auto own_string = "1152233611"_sv;
    STATIC_REQUIRE(own_string.strip("156") == "2233"_sv);
}

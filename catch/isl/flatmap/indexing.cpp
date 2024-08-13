#include <isl/detail/debug/debug.hpp>
#include <isl/flat_set.hpp>


TEST_CASE("FlatmapIndexing", "[Flatmap]")
{
    // NOLINTBEGIN

    auto flatmap = isl::StaticFlatmap<int, int, 10>{{10, 20}, {20, 30}, {30, 40}};

    REQUIRE(flatmap[10] == 20);
    REQUIRE(flatmap[20] == 30);
    REQUIRE(flatmap[30] == 40);

    flatmap[40] = 50;
    flatmap[20] = 35;

    REQUIRE(flatmap[40] == 50);
    REQUIRE(flatmap[20] == 35);

    // NOLINTEND
}

TEST_CASE("FlatmapConstIndexing", "[Flatmap]")
{
    static constexpr auto flatmap = isl::StaticFlatmap<int, int, 10>{{10, 20}, {20, 30}, {30, 40}};

    REQUIRE(flatmap[10] == 20);
    REQUIRE(flatmap[20] == 30);
    REQUIRE(flatmap[30] == 40);

    REQUIRE_THROWS_AS(flatmap.at(40), std::out_of_range);
}
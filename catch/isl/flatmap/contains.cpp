#include <isl/detail/debug/debug.hpp>
#include <isl/flatmap.hpp>

TEST_CASE("FlatmapContains", "[Flatmap]")
{
    static constexpr auto flatmap =
            isl::StaticFlatmap<int, int, 10>{{10, 20}, {20, 30}, {30, 40}};// NOLINT

    REQUIRE(flatmap.size() == 3);
    REQUIRE(flatmap.capacity() == 10);

    REQUIRE(flatmap.contains(10));
    REQUIRE(flatmap.contains(20));
    REQUIRE(flatmap.contains(30));

    REQUIRE(!flatmap.contains(0));
    REQUIRE(!flatmap.contains(40));
}

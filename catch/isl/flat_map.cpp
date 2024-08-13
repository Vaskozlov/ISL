#include <isl/detail/debug/debug.hpp>
#include <isl/flat_map.hpp>

TEST_CASE("FlatMap", "[Container]")
{
    auto set = isl::FlatMap<int, int>{};
    auto [it_1, inserted_1] = set.tryEmplace(0, 0);

    REQUIRE(inserted_1);
    REQUIRE(it_1->first == 0);
    REQUIRE(it_1->second == 0);

    auto [it_2, inserted_2] = set.tryEmplace(0, 0);
    REQUIRE_FALSE(inserted_2);
    REQUIRE(it_2->second == 0);
}

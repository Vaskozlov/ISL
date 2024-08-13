#include <isl/detail/debug/debug.hpp>
#include <isl/flat_set.hpp>

TEST_CASE("FlatSet", "[Container]")
{
    auto set = isl::FlatSet<int>{};
    auto [it_1, inserted_1] = set.tryEmplace(0);

    REQUIRE(inserted_1);
    REQUIRE(*it_1 == 0);

    auto [it_2, inserted_2] = set.tryEmplace(0);
    REQUIRE_FALSE(inserted_2);
}

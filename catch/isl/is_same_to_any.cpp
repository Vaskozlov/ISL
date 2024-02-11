#include <isl/detail/concepts.hpp>
#include <isl/detail/debug/debug.hpp>

TEST_CASE("SameToAny", "[Concepts]")
{
    REQUIRE(isl::IsSameToAny<int, float, double, int>);
    REQUIRE(!isl::IsSameToAny<int, float, double, unsigned int>);
}

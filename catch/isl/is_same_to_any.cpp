#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("SameToAny", "[Concepts]")
{
    REQUIRE(isl::IsSameToAny<int, float, double, int>);
    REQUIRE(!isl::IsSameToAny<int, float, double, unsigned int>);
}

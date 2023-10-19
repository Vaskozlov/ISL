#include <isl/detail/concepts.hpp>
#include <isl/detail/debug/debug.hpp>

using namespace isl;

TEST_CASE("SameToAny", "[Concepts]")
{
    REQUIRE(IsSameToAny<int, float, double, int>);
    REQUIRE(!IsSameToAny<int, float, double, unsigned int>);
}

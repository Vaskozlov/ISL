#include <isl/detail/concepts.hpp>
#include <isl/detail/debug/debug.hpp>

TEST_CASE("SameToAny", "[Concepts]")
{
    STATIC_REQUIRE(isl::IsSameToAny<int, float, double, int>);
    STATIC_REQUIRE(!isl::IsSameToAny<int, float, double, unsigned int>);
}

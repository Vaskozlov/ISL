#include <isl/detail/debug/debug.hpp>
#include <isl/isl.hpp>
#include <span>

TEST_CASE("TraitIntTest", "[Trait]")
{
    using namespace isl;
    STATIC_REQUIRE(isl::Trait<int, std::is_integral, std::is_trivial>);
}

TEST_CASE("RangeOf", "[Trait]")
{
    using namespace isl;
    STATIC_REQUIRE(isl::RangeOf<std::span<int>, int>);
}

TEST_CASE("RangeOver", "[Trait]")
{
    using namespace isl;
    STATIC_REQUIRE(isl::RangeOver < std::span<int>, ISL_RANGE_OVER_CONCEPT(std::integral));
}
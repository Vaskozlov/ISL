#include "isl/bit.hpp"
#include "isl/detail/debug/debug.hpp"

TEST_CASE("BitScan", "[Bit]")
{
    using namespace isl::bit;

#ifdef _MSC_VER
    REQUIRE(msvc::runtimeBitScanForward(0) == not_found_result);
#endif

    for (std::size_t i = 0; i != sizeof(std::size_t) * 8; ++i) {
        REQUIRE(msvc::compileTimeBitScanForward(1ULL << i) == i);
        REQUIRE(bitScanForward(1ULL << i) == i);
    }
}

TEST_CASE("BitSet", "[BitSet]")
{
    using namespace isl::bit;
    auto bitset = BitSet<512>{};

    REQUIRE(bitset.findAndSet() == 0);
    REQUIRE(bitset.findAndSet() == 1);

    REQUIRE(bitset.test(0));
    REQUIRE(bitset.test(1));

    for (std::size_t i = 2; i != bitset.size(); ++i) {
        REQUIRE_FALSE(bitset.test(i));
    }

    bitset.reset(0);
    REQUIRE_FALSE(bitset.test(0));

    bitset.clear();

    for (std::size_t i = 0; i != bitset.size(); ++i) {
        REQUIRE_FALSE(bitset.test(i));
    }

    bitset.set(128);
    REQUIRE(bitset.test(128));

    bitset.reset(128);
    REQUIRE_FALSE(bitset.test(128));
}

#include <isl/detail/debug/debug.hpp>
import isl;

static auto intGenerator() -> isl::Generator<std::size_t>
{
    co_yield isl::as<size_t>(0);
}

TEST_CASE("YieldFromEmptyGenerator", "[Coroutine]")
{
    auto generator = intGenerator();

    REQUIRE(generator.yield() == 0);
    REQUIRE_THROWS(generator.yield());
}

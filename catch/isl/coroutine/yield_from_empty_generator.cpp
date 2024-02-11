#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/generator.hpp>

static auto intGenerator() -> isl::Generator<size_t>
{
    co_yield isl::as<size_t>(0);
}

TEST_CASE("YieldFromEmptyGenerator", "[Coroutine]")
{
    auto generator = intGenerator();

    REQUIRE(generator.yield() == 0);
    REQUIRE_THROWS(generator.yield());
}

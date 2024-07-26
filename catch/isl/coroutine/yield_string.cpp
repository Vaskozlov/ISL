#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/generator.hpp>

static auto stringGenerator() -> isl::Generator<isl::String>
{
    co_yield isl::String{"Hello, World!"};
}

TEST_CASE("YieldStringFromGenerator", "[Coroutine]")
{
    auto generator = stringGenerator();

    REQUIRE(generator.yield() == "Hello, World!");
    REQUIRE_THROWS(generator.yield());
}

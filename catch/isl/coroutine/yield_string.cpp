#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/generator.hpp>

static auto someGenerator() -> isl::Generator<std::string>
{
    co_yield std::string{"Hello, World!"};
}

TEST_CASE("YieldStringFromGenerator", "[Coroutine]")
{
    auto generator = someGenerator();

    REQUIRE(generator.yield() == "Hello, World!");
    REQUIRE_THROWS(generator.yield());
}

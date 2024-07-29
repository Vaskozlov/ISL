#include <isl/coroutine/generator.hpp>
#include <iostream>
#include <isl/detail/debug/debug.hpp>

static auto stringGenerator() -> isl::Generator<std::string>
{
    co_yield std::string{"Hello, World!"};
}

TEST_CASE("YieldStringFromGenerator", "[Coroutine]")
{
    auto generator = stringGenerator();

    REQUIRE(generator.yield() == "Hello, World!");
    REQUIRE_THROWS(generator.yield());
}

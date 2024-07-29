#include <iostream>
#include <isl/coroutine/generator.hpp>
#include <isl/detail/debug/debug.hpp>


constexpr static std::array<std::size_t, 20> FibonacciNumbers = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181};

static auto fibonacciGenerator() -> isl::Generator<std::size_t>
{
    std::size_t first = 0;
    std::size_t second = 1;

    co_yield first;
    co_yield second;

    while (true) {
        auto fib_value = first + second;
        co_yield fib_value;

        first = second;
        second = fib_value;
    }
}

TEST_CASE("FibonacciGenerator", "[Coroutine]")
{
    auto gen = fibonacciGenerator();

    for (std::size_t fibonacci_number : FibonacciNumbers) {
        REQUIRE(gen.yield() == fibonacci_number);
    }
}

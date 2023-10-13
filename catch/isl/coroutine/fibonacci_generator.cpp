#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/generator.hpp>


constexpr static std::array<size_t, 20> FibonacciNumbers = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181};

static auto fib() -> isl::Generator<size_t>
{
    size_t first = 0;
    size_t second = 1;

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
    auto gen = fib();

    for (size_t fibonacci_number : FibonacciNumbers) {
        REQUIRE(gen.yield() == fibonacci_number);
    }
}

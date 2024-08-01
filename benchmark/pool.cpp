#include <algorithm>
#include <benchmark/benchmark.h>
#include <isl/thread/pool.hpp>
#include <random>
#include <vector>

static isl::thread::Pool ThreadPool(4);

static auto classicFibonacci(isl::u64 n) -> isl::u64
{
    if (n <= 1) {
        return n;
    }

    return classicFibonacci(n - 1) + classicFibonacci(n - 2);
}

static auto fibonacciWithThreadPool(isl::u64 n, isl::thread::Pool *pool) -> isl::Task<isl::u64>
{
    if (n <= 1) {
        co_return n;
    }

    if (n > 20) {
        auto first = pool->submit(fibonacciWithThreadPool(n - 1, pool));
        auto second = pool->submit(fibonacciWithThreadPool(n - 2, pool));

        auto lhs = co_await first;
        auto rhs = co_await second;
        co_return lhs + rhs;
    }

    co_return classicFibonacci(n - 1) + classicFibonacci(n - 2);
}

static auto createTestNumbers() -> std::vector<isl::u32>
{
    std::vector<isl::u32> v;

    for (std::size_t i = 30; i != 36; ++i) {
        v.emplace_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::ranges::shuffle(v, g);
    return v;
}

static void classicFibonacciBenchmark(benchmark::State &state)
{
    auto numbers = createTestNumbers();
    for (auto _ : state) {
        for (auto v : numbers) {
            benchmark::DoNotOptimize(classicFibonacci(v));
        }
    }
}

BENCHMARK(classicFibonacciBenchmark);

static void fibonacciWithThreadPoolBenchmark(benchmark::State &state)
{
    auto numbers = createTestNumbers();
    for (auto _ : state) {
        for (auto v : numbers) {
            auto task = ThreadPool.submit(fibonacciWithThreadPool(v, std::addressof(ThreadPool)));
            benchmark::DoNotOptimize(task.await());
        }
    }
}

BENCHMARK(fibonacciWithThreadPoolBenchmark)->Threads(1)->Iterations(100);
BENCHMARK_MAIN();

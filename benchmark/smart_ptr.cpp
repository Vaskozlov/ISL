#include <benchmark/benchmark.h>
#include <isl/memory.hpp>

auto PublicUniqueAllocator = isl::FixedSizeAllocatorForUniquePtr<128, std::string, std::size_t>{};
auto PublicSharedAllocator = isl::FixedSizeAllocatorForSharedPtr<128, std::string, std::size_t>{};

using UniquePtr = isl::UniquePtr<std::string, &PublicUniqueAllocator>;
using SharedPtr = isl::SharedPtr<std::string, &PublicSharedAllocator>;

static void islUniquePtrConstruct(benchmark::State &state)
{
    for (auto _ : state) {
        auto pointers = std::array<UniquePtr, 1024>{};

        for (auto &ptr : pointers) {
            ptr = UniquePtr("Hello, World!");
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(islUniquePtrConstruct);

void stdUniquePtrConstruct(benchmark::State &state)
{
    for (auto _ : state) {
        auto pointers = std::array<std::unique_ptr<std::string>, 1024>{};

        for (auto &ptr : pointers) {
            ptr = std::make_unique<std::string>("Hello, World!");
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(stdUniquePtrConstruct);

static void islSharedPtrConstruct(benchmark::State &state)
{
    for (auto _ : state) {
        auto pointers = std::array<SharedPtr, 1024>{};

        for (auto &ptr : pointers) {
            ptr = SharedPtr("Hello, World!");
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(islSharedPtrConstruct);

void stdSharedPtrConstruct(benchmark::State &state)
{
    for (auto _ : state) {
        auto pointers = std::array<std::shared_ptr<std::string>, 1024>{};

        for (auto &ptr : pointers) {
            ptr = std::make_shared<std::string>("Hello, World!");
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(stdSharedPtrConstruct);

static void islSharedPtrCopy(benchmark::State &state)
{
    auto ptr = SharedPtr{"Hello, World!"};

    for (auto _ : state) {
        auto copy = ptr;
        benchmark::DoNotOptimize(copy);
    }
}

BENCHMARK(islSharedPtrCopy);

void stdSharedPtrCopy(benchmark::State &state)
{
    auto ptr = std::make_shared<std::string>("Hello, World!");

    for (auto _ : state) {
        auto copy = ptr;
        benchmark::DoNotOptimize(copy);
    }
}

BENCHMARK(stdSharedPtrCopy);

static void islSharedPtrMove(benchmark::State &state)
{
    for (auto _ : state) {
        auto ptr = SharedPtr{"Hello, World!"};
        benchmark::DoNotOptimize(ptr);

        auto moved = std::move(ptr);
        benchmark::DoNotOptimize(moved);
    }
}

BENCHMARK(islSharedPtrMove);

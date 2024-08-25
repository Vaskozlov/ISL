#include <benchmark/benchmark.h>
#include <isl/fixed_size_allocator.hpp>

static void blockAllocatorAllocate(benchmark::State &state)
{
    auto allocator = isl::FixedSizeAllocator<sizeof(std::size_t), alignof(std::size_t), 1024>{};

    for (auto _ : state) {
        auto *ptr = allocator.allocate();
        benchmark::DoNotOptimize(ptr);
    }
}

BENCHMARK(blockAllocatorAllocate);

void stdAllocatorAllocate(benchmark::State &state)
{
    for (auto _ : state) {
        auto *ptr = ::operator new(sizeof(std::size_t));
        benchmark::DoNotOptimize(ptr);
    }
}

// BENCHMARK(stdAllocatorAllocate);

static void blockAllocatorAllocateAndImmediateDeallocate(benchmark::State &state)
{
    auto allocator = isl::FixedSizeAllocator<sizeof(std::size_t), alignof(std::size_t), 1024>{};

    for (auto _ : state) {
        auto *ptr = allocator.allocate();
        benchmark::DoNotOptimize(ptr);
        allocator.deallocate(ptr);
    }
}

BENCHMARK(blockAllocatorAllocateAndImmediateDeallocate);

static void stdAllocatorAllocateAndImmediateDeallocate(benchmark::State &state)
{
    for (auto _ : state) {
        auto *ptr = ::operator new(sizeof(std::size_t));
        benchmark::DoNotOptimize(ptr);
        ::operator delete(ptr);
    }
}

BENCHMARK(stdAllocatorAllocateAndImmediateDeallocate);

static void blockAllocatorAllocateAndDeallocate(benchmark::State &state)
{
    auto allocator = isl::FixedSizeAllocator<sizeof(std::size_t), alignof(std::size_t), 1024>{};

    for (auto _ : state) {
        auto pointers = std::array<void *, 1024 * 10>{};

        for (auto *&ptr : pointers) {
            ptr = allocator.allocate();
            benchmark::DoNotOptimize(ptr);
        }

        for (auto *ptr : pointers) {
            allocator.deallocate(ptr);
        }
    }
}

BENCHMARK(blockAllocatorAllocateAndDeallocate);

static void stdAllocatorAllocateAndDeallocate(benchmark::State &state)
{
    for (auto _ : state) {
        auto pointers = std::array<void *, 1024 * 10>{};

        for (auto *&ptr : pointers) {
            ptr = ::operator new(sizeof(std::size_t));
            benchmark::DoNotOptimize(ptr);
        }

        for (auto *ptr : pointers) {
            ::operator delete(ptr);
        }
    }
}

BENCHMARK(stdAllocatorAllocateAndDeallocate);

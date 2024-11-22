#include <benchmark/benchmark.h>
#include <isl/memory.hpp>

struct S
{
    virtual auto f() -> int = 0;
    virtual ~S() = default;
};

using FrameForSharedPtr = isl::SharedPtrFrameFor<std::string, std::size_t, S>;

// NOLINTNEXTLINE
static auto PublicUniqueAllocator = isl::FixedSizeAllocator<
    isl::ObjectsMaxSize<std::string, std::size_t, S>,
    isl::ObjectsMaxAlignment<std::string, std::size_t, S>>{};

// NOLINTNEXTLINE
static auto PublicSharedAllocator =
    isl::FixedSizeAllocator<sizeof(FrameForSharedPtr), alignof(FrameForSharedPtr)>{};

template<typename T>
using UniquePtr = isl::UniquePtr<T, &PublicUniqueAllocator>;

template<typename T>
using SharedPtr = isl::SharedPtr<T, FrameForSharedPtr, &PublicSharedAllocator>;

static void islUniquePtrConstruct(benchmark::State &state)
{
    auto s = isl::SharedPtr<S, FrameForSharedPtr, &PublicSharedAllocator>{};

    for (auto _ : state) {
        auto pointers = std::array<UniquePtr<std::string>, 1024>{};

        for (auto &ptr : pointers) {
            ptr = UniquePtr<std::string>{"Hello, World!"};
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(islUniquePtrConstruct);

static void stdUniquePtrConstruct(benchmark::State &state)
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
        auto pointers = std::array<SharedPtr<std::string>, 1024>{};

        for (auto &ptr : pointers) {
            ptr = SharedPtr<std::string>("Hello, World!");
            benchmark::DoNotOptimize(*ptr);
        }
    }
}

BENCHMARK(islSharedPtrConstruct);

static void stdSharedPtrConstruct(benchmark::State &state)
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
    const auto ptr = SharedPtr<std::string>{"Hello, World!"};

    for (auto _ : state) {
        auto copy = ptr;
        benchmark::DoNotOptimize(copy);
    }
}

BENCHMARK(islSharedPtrCopy);

static void stdSharedPtrCopy(benchmark::State &state)
{
    const auto ptr = std::make_shared<std::string>("Hello, World!");

    for (auto _ : state) {
        auto copy = ptr;
        benchmark::DoNotOptimize(copy);
    }
}

BENCHMARK(stdSharedPtrCopy);

static void islSharedPtrMove(benchmark::State &state)
{
    for (auto _ : state) {
        auto ptr = SharedPtr<std::string>{"Hello, World!"};
        benchmark::DoNotOptimize(ptr);

        auto moved = std::move(ptr);
        benchmark::DoNotOptimize(moved);
    }
}

BENCHMARK(islSharedPtrMove);

static void stdSharedPtrMove(benchmark::State &state)
{
    for (auto _ : state) {
        auto ptr = std::make_shared<std::string>("Hello, World!");
        benchmark::DoNotOptimize(ptr);

        auto moved = std::move(ptr);
        benchmark::DoNotOptimize(moved);
    }
}

BENCHMARK(stdSharedPtrMove);

#include "isl/fixed_size_allocator.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("BlockAllocatorSmallTest", "[Alloc]")
{
    auto allocator = isl::FixedSizeAllocator<128, std::size_t>{};
    auto *first = allocator.allocate();
    auto *second = allocator.allocate();

    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);
    REQUIRE(first != second);

    allocator.deallocate(first);

    auto *third = allocator.allocate();

    REQUIRE(third == first);
}


TEST_CASE("BlockAllocator", "[Alloc]")
{
    auto allocator = isl::FixedSizeAllocator<128, std::size_t>{};
    auto allocations = std::vector<void *>{};

    allocations.reserve(allocator.getAllocationBlockSize() * 128);

    for (std::size_t i = 0; i != allocations.capacity(); ++i) {
        auto *ptr_value = allocator.allocate();

        REQUIRE(ptr_value != nullptr);
        REQUIRE(std::ranges::find(allocations, ptr_value) == allocations.end());

        allocations.emplace_back(ptr_value);
    }

    for (auto *ptr : allocations) {
        allocator.deallocate(ptr);
    }

    auto *free_object = allocator.getFirstFreeObject();

    for (std::size_t i = 0; i != allocator.getAllocationBlockSize() * 128; ++i) {
        REQUIRE(std::exchange(free_object, free_object->next) != nullptr);
    }

    REQUIRE(free_object == nullptr);
}

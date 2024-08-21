#include "isl/block_allocator.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("BlockAllocatorSmallTest", "[Alloc]")
{
    auto allocator = isl::alloc::BlockAllocator<std::size_t, 128>{};
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
    auto allocator = isl::alloc::BlockAllocator<std::size_t, 128>{};
    auto allocations = std::vector<std::size_t *>{};

    allocations.reserve(allocator.getBlockSize() * 128);

    for (std::size_t i = 0; i != allocations.capacity(); ++i) {
        auto *ptr_value = allocator.allocate();

        REQUIRE(ptr_value != nullptr);
        REQUIRE(std::ranges::find(allocations, ptr_value) == allocations.end());

        allocations.emplace_back(ptr_value);
    }

    for (auto *ptr : allocations) {
        allocator.deallocate(ptr);
    }

    auto *head = allocator.getFirstBlock();

    while (head != nullptr) {
        for (std::size_t i = 0; i != allocator.getBlockSize(); ++i) {
            REQUIRE_FALSE(head->freeBlocks.test(i));
        }

        head = head->next;
    }
}
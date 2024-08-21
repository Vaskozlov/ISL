#ifndef ISL_PROJECT_BLOCK_ALLOCATOR_HPP
#define ISL_PROJECT_BLOCK_ALLOCATOR_HPP

#include <isl/bit.hpp>

namespace isl::alloc
{
    template<typename T, std::size_t BlockSize>
    class BlockAllocator
    {
    private:
        constexpr static auto registerSize = sizeof(std::size_t) * 8;
        constexpr static auto notFoundValue = std::numeric_limits<std::size_t>::max();

        static_assert(BlockSize % registerSize == 0);

        struct Block
        {
            T storage[BlockSize];
            bit::BitSet<BlockSize> freeBlocks{};
            std::size_t firstFree{};
            Block *next{};
        };

        Block *head{};
        Block *freeBlock{};
        std::size_t blocksCount{};

    public:
        BlockAllocator() = default;

        BlockAllocator(const BlockAllocator &) = delete;

        BlockAllocator(BlockAllocator &&other) noexcept
          : head{std::exchange(other.head, nullptr)}
          , freeBlock{std::exchange(other.freeBlock, nullptr)}
        {}

        ~BlockAllocator()
        {
            while (head != nullptr) {
                deallocateBlock(std::exchange(head, head->next));
            }
        }

        auto operator=(const BlockAllocator &other) -> BlockAllocator & = delete;

        auto operator=(BlockAllocator &&other) noexcept -> BlockAllocator &
        {
            std::swap(head, other.head);
            std::swap(freeBlock, other.freeBlock);

            return *this;
        }

        [[nodiscard]] auto getFirstBlock() const noexcept -> Block *
        {
            return head;
        }

        ISL_DECL auto getBlockSize() const noexcept -> std::size_t
        {
            return BlockSize;
        }

        auto allocate() -> T *
        {
            fixNullHead();
            auto *block = freeBlock;

            do {
                const auto index = block->freeBlocks.findAndSet(0);

                if (index != notFoundValue) {
                    // block->firstFree = index;
                    freeBlock = block;
                    return &block->storage[index];
                }

                if (block->next == nullptr) {
                    block->next = allocateNewBlock();
                    block->next->freeBlocks.set(0);
                    // block->firstFree = 1;
                    return &block->next->storage[0];
                }

                block = block->next;
            } while (true);
        }

        auto deallocate(T *ptr) -> void
        {
            auto *block = head;

            for (; block != nullptr; block = block->next) {
                if (block->storage <= ptr && ptr < block->storage + BlockSize) {
                    const auto index = ptr - block->storage;
                    block->freeBlocks.reset(index);
                    // block->firstFree = std::min<std::size_t>(block->firstFree, index);
                    freeBlock = head;
                    return;
                }
            }

            throw std::runtime_error{"Unable to deallocate pointer"};
        }

    private:
        auto allocateNewBlock() -> Block *
        {
            auto *new_block = static_cast<Block *>(
                ::operator new(sizeof(Block), std::align_val_t{alignof(Block)}));

            new_block->freeBlocks.clear();
            new_block->next = nullptr;
            // new_block->firstFree = 0;

            freeBlock = new_block;
            ++blocksCount;

            return new_block;
        }

        auto deallocateBlock(Block *block) -> void
        {
            --blocksCount;
            ::operator delete(block, sizeof(Block), static_cast<std::align_val_t>(alignof(Block)));
        }

        auto fixNullHead() -> void
        {
            if (head == nullptr) {
                head = allocateNewBlock();
                freeBlock = head;
            }
        }
    };
}// namespace isl::alloc

#endif /* ISL_PROJECT_BLOCK_ALLOCATOR_HPP */

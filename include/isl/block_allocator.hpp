#ifndef ISL_PROJECT_BLOCK_ALLOCATOR_HPP
#define ISL_PROJECT_BLOCK_ALLOCATOR_HPP

#include <isl/bit.hpp>

namespace isl::alloc
{
    template<std::size_t BlockSize, typename... Ts>
    class BlockAllocator
    {
    private:
        static_assert(sizeof...(Ts) > 0);

        constexpr static auto maxTypeSize = std::max({sizeof(Ts)...});
        constexpr static auto maxTypeAlign = std::max({alignof(Ts)...});
        constexpr static auto notFoundValue = std::numeric_limits<std::size_t>::max();

        struct Block;

        struct StoredObject
        {
            alignas(maxTypeAlign) char object[maxTypeSize];
            StoredObject *next;
        };

        struct Block
        {
            StoredObject storage[BlockSize];
            Block *next;

            auto init() -> void
            {
                next = nullptr;

                for (std::size_t i = 0; i != BlockSize - 1; ++i) {
                    storage[i].next = &storage[i + 1];
                }

                storage[BlockSize - 1].next = nullptr;
            }
        };

        Block *head{};
        StoredObject *freeObject{};

    public:
        BlockAllocator()
          : head{allocateNewBlock()}
          , freeObject{&head->storage[0]}
        {}

        BlockAllocator(const BlockAllocator &) = delete;

        BlockAllocator(BlockAllocator &&other) noexcept
          : head{std::exchange(other.head, nullptr)}
          , freeObject{std::exchange(other.freeObject, nullptr)}
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
            std::swap(freeObject, other.freeObject);

            return *this;
        }

        [[nodiscard]] auto getFirstBlock() const noexcept -> Block *
        {
            return head;
        }

        [[nodiscard]] auto getFirstFreeObject() const noexcept -> StoredObject *
        {
            return freeObject;
        }

        ISL_DECL auto getBlockSize() const noexcept -> std::size_t
        {
            return BlockSize;
        }

        auto allocate() -> void *
        {
            if (freeObject == nullptr) {
                auto *new_block = allocateNewBlock();
                freeObject = &new_block->storage[0];
                new_block->next = std::exchange(head, new_block);
            }

            return static_cast<void *>(std::exchange(freeObject, freeObject->next));
        }

        auto deallocate(void *value_ptr) -> void
        {
            auto *ptr = static_cast<StoredObject *>(value_ptr);
            ptr->next = std::exchange(freeObject, ptr);
        }

    private:
        auto allocateNewBlock() -> Block *
        {
            auto *new_block = static_cast<Block *>(
                ::operator new(sizeof(Block), std::align_val_t{alignof(Block)}));

            new_block->init();

            return new_block;
        }

        static auto deallocateBlock(Block *block) -> void
        {
            ::operator delete(block, sizeof(Block), static_cast<std::align_val_t>(alignof(Block)));
        }
    };
}// namespace isl::alloc

#endif /* ISL_PROJECT_BLOCK_ALLOCATOR_HPP */

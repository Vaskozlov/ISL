#ifndef ISL_PROJECT_FIXED_SIZE_ALLOCATOR_HPP
#define ISL_PROJECT_FIXED_SIZE_ALLOCATOR_HPP

#include <isl/bit.hpp>

namespace isl::alloc
{
    template<std::size_t BlockSize, typename... Alternatives>
    class FixedSizeAllocator
    {
    private:
        static_assert(sizeof...(Alternatives) > 0);

        constexpr static auto maxObjectSize = std::max({sizeof(Alternatives)...});
        constexpr static auto maxObjectAlignment = std::max({alignof(Alternatives)...});

        struct AllocationBlock;

        struct ObjectFrame
        {
            alignas(maxObjectAlignment) std::array<char, maxObjectSize> object;
            ObjectFrame *next;
        };

        struct AllocationBlock
        {
            std::array<ObjectFrame, BlockSize> storage;
            AllocationBlock *next;

            auto init() -> void
            {
                next = nullptr;

                for (std::size_t i = 0; i != BlockSize - 1; ++i) {
                    storage[i].next = &storage[i + 1];
                }

                storage[BlockSize - 1].next = nullptr;
            }
        };

        AllocationBlock *head{};
        ObjectFrame *freeObject{};

    public:
        FixedSizeAllocator()
          : head{allocateNewBlock()}
          , freeObject{&head->storage[0]}
        {}

        FixedSizeAllocator(const FixedSizeAllocator &) = delete;

        FixedSizeAllocator(FixedSizeAllocator &&other) noexcept
          : head{std::exchange(other.head, nullptr)}
          , freeObject{std::exchange(other.freeObject, nullptr)}
        {}

        ~FixedSizeAllocator()
        {
            while (head != nullptr) {
                deallocateBlock(std::exchange(head, head->next));
            }
        }

        auto operator=(const FixedSizeAllocator &other) -> FixedSizeAllocator & = delete;

        auto operator=(FixedSizeAllocator &&other) noexcept -> FixedSizeAllocator &
        {
            std::swap(head, other.head);
            std::swap(freeObject, other.freeObject);

            return *this;
        }

        [[nodiscard]] auto getFirstBlock() const noexcept -> AllocationBlock *
        {
            return head;
        }

        [[nodiscard]] auto getFirstFreeObject() const noexcept -> ObjectFrame *
        {
            return freeObject;
        }

        ISL_DECL static auto getAllocationBlockSize() noexcept -> std::size_t
        {
            return BlockSize;
        }

        [[nodiscard]] auto allocate() -> void *
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
            auto *ptr = static_cast<ObjectFrame *>(value_ptr);
            ptr->next = std::exchange(freeObject, ptr);
        }

    private:
        [[nodiscard]] auto allocateNewBlock() -> AllocationBlock *
        {
            auto *new_block = static_cast<AllocationBlock *>(::operator new(
                sizeof(AllocationBlock), std::align_val_t{alignof(AllocationBlock)}));

            new_block->init();

            return new_block;
        }

        static auto deallocateBlock(AllocationBlock *block) -> void
        {
            ::operator delete(
                block, sizeof(AllocationBlock),
                static_cast<std::align_val_t>(alignof(AllocationBlock)));
        }
    };
}// namespace isl::alloc

#endif /* ISL_PROJECT_FIXED_SIZE_ALLOCATOR_HPP */

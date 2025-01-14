#ifndef ISL_PROJECT_POOL_ALLOCATOR_HPP
#define ISL_PROJECT_POOL_ALLOCATOR_HPP

#include <isl/isl.hpp>

namespace isl
{
    template <std::size_t MaxObjectSize, std::size_t Align, std::size_t BlockSize = 128>
    requires(BlockSize % 16 == 0)
    class PoolAllocator
    {
    private:
        struct AllocationBlock;

        struct ObjectFrame
        {
            alignas(Align) std::byte object[MaxObjectSize];
            ObjectFrame *next;
        };

        struct AllocationBlock
        {
            ObjectFrame storage[BlockSize];
            AllocationBlock *next;

            AllocationBlock()
              : next{nullptr}
            {
                ISL_UNROLL_N(16)
                for (std::size_t i = 0; i != BlockSize; ++i) {
                    storage[i].next = &storage[i + 1];
                }

                storage[BlockSize - 1].next = nullptr;
            }
        };

        AllocationBlock *head{};
        ObjectFrame *freeObject{};

    public:
        PoolAllocator() = default;

        PoolAllocator(const PoolAllocator &) = delete;

        PoolAllocator(PoolAllocator &&other) noexcept
          : head{std::exchange(other.head, nullptr)}
          , freeObject{std::exchange(other.freeObject, nullptr)}
        {}

        ~PoolAllocator()
        {
            while (head != nullptr) {
                ::delete std::exchange(head, head->next);
            }
        }

        auto operator=(const PoolAllocator &other) -> PoolAllocator & = delete;

        auto operator=(PoolAllocator &&other) noexcept -> PoolAllocator &
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

        ISL_DECL static auto getMaxObjectSize() noexcept -> std::size_t
        {
            return MaxObjectSize;
        }

        ISL_DECL static auto getAllocationAlignment() noexcept -> std::size_t
        {
            return Align;
        }

        [[nodiscard]] auto allocate() -> void *
        {
            if (freeObject == nullptr) {
                auto *new_block = ::new AllocationBlock();
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

        template <typename T>
        ISL_DECL static auto canAllocate() noexcept -> bool
        {
            return std::is_abstract_v<T> || (sizeof(T) <= MaxObjectSize && alignof(T) <= Align);
        }

    private:
        static auto deallocateBlock(const AllocationBlock *block) -> void
        {
            ::delete block;
        }
    };

    template <typename... Ts>
    constexpr inline auto ObjectsMaxSize =
        std::max({sizeof(std::conditional_t<std::is_abstract_v<Ts>, std::size_t, Ts>)...});

    template <typename... Ts>
    constexpr inline auto ObjectsMaxAlignment = std::max({alignof(Ts)...});
} // namespace isl

#endif /* ISL_PROJECT_POOL_ALLOCATOR_HPP */

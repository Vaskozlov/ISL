#ifndef ISL_PROJECT_MEMORY_HPP
#define ISL_PROJECT_MEMORY_HPP

#include <isl/fixed_size_allocator.hpp>
#include <memory>

namespace isl
{
    template<typename T, auto AllocatorPtr>
    class UniquePtr
    {
        T *ptr{nullptr};

    public:
        UniquePtr() = default;

        template<typename... Ts>
        explicit UniquePtr(Ts &&...args)
            requires(AllocatorPtr->template canAllocate<T>())
          : ptr{static_cast<T *>(AllocatorPtr->allocate())}
        {
            std::construct_at(ptr, std::forward<Ts>(args)...);
        }

        UniquePtr(const UniquePtr &other) = delete;

        template<std::derived_from<T> U = T>
        UniquePtr(UniquePtr<U, AllocatorPtr> &&other) noexcept
          : ptr{other.releae()}
        {}

        ~UniquePtr()
        {
            if (ptr != nullptr) {
                std::destroy_at(ptr);
                AllocatorPtr->deallocate(static_cast<void *>(ptr));
            }
        }

        auto operator=(const UniquePtr &other) -> UniquePtr & = delete;

        auto operator=(UniquePtr &&other) noexcept -> UniquePtr &
        {
            std::swap(ptr, other.ptr);
            return *this;
        }

        [[nodiscard]] auto operator*() -> T &
        {
            return *ptr;
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            return *ptr;
        }

        auto operator->() -> T *
        {
            return ptr;
        }

        auto operator->() const -> const T *
        {
            return ptr;
        }

        [[nodiscard]] auto get() -> T *
        {
            return ptr;
        }

        [[nodiscard]] auto get() const -> const T *
        {
            return ptr;
        }

        auto release() -> T *
        {
            return std::exchange(ptr, nullptr);
        }
    };

    template<typename T, auto AllocatorPtr>
    class SharedPtr
    {
    public:
        struct Frame
        {
            mutable std::atomic<std::size_t> refCount{1};
            T object;

            template<typename... Ts>
            explicit Frame(Ts &&...args)
              : object{std::forward<Ts>(args)...}
            {}
        };

    private:
        Frame *frame{};

    public:
        SharedPtr() = default;

        template<typename... Ts>
        explicit SharedPtr(Ts &&...args)
            requires(AllocatorPtr->template canAllocate<Frame>())
          : frame{static_cast<Frame *>(AllocatorPtr->allocate())}
        {
            std::construct_at(frame, std::forward<Ts>(args)...);
        }

        SharedPtr(const SharedPtr &other)
          : frame{other.frame}
        {
            increaseRefCount();
        }

        SharedPtr(SharedPtr &&other) noexcept
          : frame{std::exchange(other.frame, nullptr)}
        {}

        ~SharedPtr()
        {
            decreaseRefCount();
        }

        auto operator=(const SharedPtr &other) -> SharedPtr &
        {
            if (this != &other) {
                decreaseRefCount();
                frame = other.frame;
                increaseRefCount();
            }

            return *this;
        }

        auto operator=(SharedPtr &&other) noexcept -> SharedPtr &
        {
            std::swap(frame, other.frame);
            return *this;
        }

        [[nodiscard]] auto operator*() -> T &
        {
            return frame->object;
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            return frame->object;
        }

        auto operator->() -> T *
        {
            return frame->object;
        }

        auto operator->() const -> const T *
        {
            return frame->object;
        }

        [[nodiscard]] auto get() -> T *
        {
            return frame->object;
        }

        [[nodiscard]] auto get() const -> const T *
        {
            return frame->object;
        }

    private:
        ISL_INLINE auto increaseRefCount() const -> void
        {
            frame->refCount.fetch_add(1, std::memory_order_acq_rel);
        }

        auto decreaseRefCount() -> void
        {
            if (frame != nullptr && frame->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                std::destroy_at(frame);
                AllocatorPtr->deallocate(static_cast<void *>(frame));
            }
        }
    };

    template<std::size_t BlockSize, typename... Ts>
    using FixedSizeAllocatorForUniquePtr =
        FixedSizeAllocator<BlockSize, std::max({sizeof(Ts)...}), std::max({alignof(Ts)...})>;

    template<std::size_t BlockSize, typename... Ts>
    using FixedSizeAllocatorForSharedPtr = FixedSizeAllocator<
        BlockSize, std::max({sizeof(typename SharedPtr<Ts, nullptr>::Frame)...}),
        std::max({alignof(typename SharedPtr<Ts, nullptr>::Frame)...})>;
}// namespace isl

#endif /* ISL_PROJECT_MEMORY_HPP */

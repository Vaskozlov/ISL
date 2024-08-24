#ifndef ISL_PROJECT_MEMORY_HPP
#define ISL_PROJECT_MEMORY_HPP

#include <isl/fixed_size_allocator.hpp>
#include <memory>

namespace isl
{
    template<typename T, auto AllocatorPtr>
        requires(AllocatorPtr->template canAllocate<T>())
    class UniquePtr
    {
        T *ptr{nullptr};

    public:
        UniquePtr() = default;

        template<typename... Ts>
        explicit UniquePtr(Ts &&...args)
            requires(!std::is_abstract_v<T>)
          : ptr{static_cast<T *>(AllocatorPtr->allocate())}
        {
            std::construct_at(ptr, std::forward<Ts>(args)...);
        }

        UniquePtr(const UniquePtr &other) = delete;

        UniquePtr(UniquePtr &&other) noexcept
          : ptr{other.release()}
        {}

        template<std::derived_from<T> U = T>
        explicit UniquePtr(UniquePtr<U, AllocatorPtr> &&other) noexcept
          : ptr{other.release()}
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

    template<std::size_t MaxObjectSize, std::size_t ObjectAlign>
    struct SharedPtrFrame
    {
        mutable std::atomic<std::size_t> refCount{1};
        alignas(ObjectAlign) std::array<std::byte, MaxObjectSize> objectBuffer;

        template<typename T>
        [[nodiscard]] auto asPtr() -> T *
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<T *>(objectBuffer.data());
        }

        template<typename T>
        [[nodiscard]] auto asPtr() const -> const T *
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<T *>(objectBuffer.data());
        }

        ISL_DECL static auto getMaxObjectSize() noexcept -> std::size_t
        {
            return MaxObjectSize;
        }

        ISL_DECL static auto getObjectAlignment() noexcept -> std::size_t
        {
            return ObjectAlign;
        }

        auto increaseRefCount() const -> std::size_t
        {
            return refCount.fetch_add(1, std::memory_order_acq_rel);
        }

        [[nodiscard]] auto decreaseRefCount() const -> std::size_t
        {
            return refCount.fetch_sub(1, std::memory_order_acq_rel);
        }

        template<typename T, typename... Ts>
        static auto initialize(SharedPtrFrame *frame, Ts &&...args) -> void
        {
            std::construct_at(std::addressof(frame->refCount), 1U);
            std::construct_at(frame->asPtr<T>(), std::forward<Ts>(args)...);
        }
    };

    template<typename... Ts>
    using SharedPtrFrameFor = SharedPtrFrame<MaxObjectSizeOf<Ts...>, MaxObjectsAlignmentOf<Ts...>>;

    template<typename T, typename Frame, auto AllocatorPtr>
        requires(AllocatorPtr->template canAllocate<Frame>())
    class SharedPtr;

    template<typename T, typename Frame, auto AllocatorPtr>
        requires(AllocatorPtr->template canAllocate<Frame>())
    class SharedPtr
    {
    private:
        Frame *frame{};

    public:
        SharedPtr() = default;

        explicit SharedPtr(Frame *shared_frame)
          : frame{shared_frame}
        {
            increaseRefCount();
        }

        template<typename... Ts>
            requires(!std::is_abstract_v<T>)
        explicit SharedPtr(Ts &&...args)
          : frame{static_cast<Frame *>(AllocatorPtr->allocate())}
        {
            Frame::template initialize<T>(frame, std::forward<Ts>(args)...);
        }

        SharedPtr(const SharedPtr &other)
          : frame{other.frame}
        {
            increaseRefCount();
        }

        SharedPtr(SharedPtr &&other) noexcept
          : frame{std::exchange(other.frame, nullptr)}
        {}

        template<std::derived_from<T> U = T>
        SharedPtr(SharedPtr<U, Frame, AllocatorPtr> &&other) noexcept
          : frame{other.releaseFrame()}
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

        auto operator==(std::nullptr_t) const -> bool
        {
            return frame == nullptr;
        }

        auto operator==(const SharedPtr &other) const -> bool = default;

        auto operator<=>(const SharedPtr &other) const -> std::weak_ordering = default;

        [[nodiscard]] auto operator*() -> T &
        {
            return *frame->template asPtr<T>();
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            return *frame->template asPtr<T>();
        }

        auto operator->() -> T *
        {
            return frame->template asPtr<T>();
        }

        auto operator->() const -> const T *
        {
            return frame->template asPtr<T>();
        }

        [[nodiscard]] auto get() -> T *
        {
            return frame->template asPtr<T>();
        }

        [[nodiscard]] auto get() const -> const T *
        {
            return frame->template asPtr<T>();
        }

        [[nodiscard]] auto getFrame() -> Frame *
        {
            return frame;
        }

        [[nodiscard]] auto releaseFrame() -> Frame *
        {
            return std::exchange(frame, nullptr);
        }

    private:
        ISL_INLINE auto increaseRefCount() const -> void
        {
            frame->increaseRefCount();
        }

        auto decreaseRefCount() -> void
        {
            if (frame != nullptr && frame->decreaseRefCount() == 1) {
                std::destroy_at(frame);
                AllocatorPtr->deallocate(static_cast<void *>(frame));
            }
        }
    };

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(const SharedPtr<From, Frame, AllocatorPtr> &ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{ptr.getFrame()};
    }

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(SharedPtr<From, Frame, AllocatorPtr> &&ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{ptr.releaseFrame()};
    }

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto dynamicPointerCast(const SharedPtr<From, Frame, AllocatorPtr> &ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        auto *frame = ptr.getFrame();
        auto *stored_value = frame->template asPtr<From>();

        if (dynamic_cast<To *>(stored_value) != nullptr) {
            return SharedPtr<To, Frame, AllocatorPtr>{frame};
        }

        return SharedPtr<To, Frame, AllocatorPtr>{};
    }

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto dynamicPointerCast(SharedPtr<From, Frame, AllocatorPtr> &&ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        auto *frame = ptr.getFrame();
        auto *stored_value = frame->template asPtr<From>();

        if (dynamic_cast<To *>(stored_value) != nullptr) {
            return SharedPtr<To, Frame, AllocatorPtr>{ptr.releaseFrame()};
        }

        return SharedPtr<To, Frame, AllocatorPtr>{};
    }
}// namespace isl

#endif /* ISL_PROJECT_MEMORY_HPP */

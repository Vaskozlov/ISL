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

        // NOLINTNEXTLINE
        UniquePtr(std::nullptr_t)
          : UniquePtr{}
        {}

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

        template<std::derived_from<T> U = T>// NOLINTNEXTLINE
        UniquePtr(UniquePtr<U, AllocatorPtr> &&other) noexcept
          : ptr{other.release()}
        {}

        ~UniquePtr()
        {
            destroyStoredObject();
        }

        auto operator=(const UniquePtr &other) -> UniquePtr & = delete;

        template<std::derived_from<T> U = T>
        auto operator=(UniquePtr<U, AllocatorPtr> &&other) noexcept -> UniquePtr &
        {
            destroyStoredObject();
            ptr = other.release();
            return *this;
        }

        auto operator=(UniquePtr &&other) noexcept -> UniquePtr &
        {
            std::swap(ptr, other.ptr);
            return *this;
        }

        [[nodiscard]] auto operator==(std::nullptr_t) const noexcept -> bool
        {
            return ptr == nullptr;
        }

        [[nodiscard]] auto operator==(const UniquePtr &other) const noexcept -> bool = default;

        [[nodiscard]] auto
            operator<=>(const UniquePtr &other) const noexcept -> std::weak_ordering = default;

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

        [[nodiscard]] auto get() const -> T *
        {
            return ptr;
        }

        auto release() -> T *
        {
            return std::exchange(ptr, nullptr);
        }

        static auto createDefault() -> UniquePtr
        {
            auto un_ptr = UniquePtr{};

            un_ptr.ptr = static_cast<T *>(AllocatorPtr->allocate());
            std::construct_at(un_ptr.ptr);

            return un_ptr;
        }

    private:
        auto destroyStoredObject() -> void
        {
            if (ptr != nullptr) {
                std::destroy_at(ptr);
                AllocatorPtr->deallocate(static_cast<void *>(ptr));
            }

            ptr = nullptr;
        }
    };

    template<std::size_t MaxObjectSize, std::size_t ObjectAlign>
    struct SharedPtrFrame
    {
        mutable std::atomic<std::size_t> refCount{1};
        alignas(ObjectAlign) std::array<std::byte, MaxObjectSize> objectBuffer;

        template<typename T>
        ISL_DECL static auto canStore() noexcept -> bool
        {
            return sizeof(T) <= MaxObjectSize && alignof(T) <= ObjectAlign;
        }

        template<typename T>
            requires(canStore<T>())
        [[nodiscard]] auto asPtr() -> T *
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<T *>(objectBuffer.data());
        }

        template<typename T>
            requires(canStore<T>())
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
            requires(canStore<T>())
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

    struct FrameMovedT
    {
    };

    struct FrameCopyT
    {
    };

    template<typename T, typename Frame, auto AllocatorPtr>
        requires(AllocatorPtr->template canAllocate<Frame>())
    class SharedPtr
    {
    private:
        Frame *frame{};

    public:
        SharedPtr() = default;

        SharedPtr(std::nullptr_t) noexcept
          : SharedPtr{}
        {}

        explicit SharedPtr(FrameMovedT /* unused */, Frame *shared_frame)
          : frame{shared_frame}
        {}

        explicit SharedPtr(FrameCopyT /* unused */, Frame *shared_frame)
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

        template<std::derived_from<T> U = T>// NOLINTNEXTLINE
        SharedPtr(const SharedPtr<U, Frame, AllocatorPtr> &other)
          : frame{other.getFrame()}
        {
            increaseRefCount();
        }

        SharedPtr(const SharedPtr &other)
          : frame{other.getFrame()}
        {
            increaseRefCount();
        }

        template<std::derived_from<T> U = T>// NOLINTNEXTLINE
        SharedPtr(SharedPtr<U, Frame, AllocatorPtr> &&other) noexcept
          : frame{other.releaseFrame()}
        {}

        SharedPtr(SharedPtr &&other) noexcept
          : frame{other.releaseFrame()}
        {}

        ~SharedPtr()
        {
            decreaseRefCount();
        }

        template<std::derived_from<T> U = T>
        auto operator=(const SharedPtr<U, Frame, AllocatorPtr> &other) -> SharedPtr &
        {
            if (static_cast<const void *>(this) != static_cast<const void *>(&other)) {
                decreaseRefCount();
                frame = other.getFrame();
                increaseRefCount();
            }

            return *this;
        }

        // NOLINTNEXTLINE
        auto operator=(const SharedPtr &other) -> SharedPtr &
        {
            // NOLINTNEXTLINE
            return operator= <T>(other);
        }

        auto operator=(SharedPtr &&other) noexcept -> SharedPtr &
        {
            std::swap(frame, other.frame);
            return *this;
        }

        [[nodiscard]] auto operator==(std::nullptr_t) const noexcept -> bool
        {
            return frame == nullptr;
        }

        [[nodiscard]] auto operator==(const SharedPtr &other) const noexcept -> bool = default;

        [[nodiscard]] auto
            operator<=>(const SharedPtr &other) const noexcept -> std::weak_ordering = default;

        template<typename U>
        ISL_DECL static auto canStore() noexcept -> bool
        {
            return Frame::template canStore<U>();
        }

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

        [[nodiscard]] auto get() noexcept -> T *
        {
            if (frame == nullptr) {
                return nullptr;
            }

            return frame->template asPtr<T>();
        }

        [[nodiscard]] auto get() const noexcept -> T *
        {
            if (frame == nullptr) {
                return nullptr;
            }

            return frame->template asPtr<T>();
        }

        [[nodiscard]] auto getFrame() noexcept -> Frame *
        {
            return frame;
        }

        [[nodiscard]] auto getFrame() const noexcept -> Frame *
        {
            return frame;
        }

        [[nodiscard]] auto releaseFrame() noexcept -> Frame *
        {
            return std::exchange(frame, nullptr);
        }

    private:
        auto increaseRefCount() const -> void
        {
            frame->increaseRefCount();
        }

        auto decreaseRefCount() -> void
        {
            if (frame != nullptr && frame->decreaseRefCount() == 1) {
                std::destroy_at(frame->template asPtr<T>());
                AllocatorPtr->deallocate(static_cast<void *>(frame));
            }
        }
    };

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(const SharedPtr<From, Frame, AllocatorPtr> &ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{FrameCopyT{}, ptr.getFrame()};
    }

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(SharedPtr<From, Frame, AllocatorPtr> &&ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{FrameMovedT{}, ptr.releaseFrame()};
    }

    template<typename To, typename From, typename Frame, auto AllocatorPtr>
    auto dynamicPointerCast(const SharedPtr<From, Frame, AllocatorPtr> &ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        auto *frame = ptr.getFrame();
        auto *stored_value = frame->template asPtr<From>();

        if (dynamic_cast<To *>(stored_value) != nullptr) {
            return SharedPtr<To, Frame, AllocatorPtr>{FrameCopyT{}, frame};
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
            return SharedPtr<To, Frame, AllocatorPtr>{FrameMovedT{}, ptr.releaseFrame()};
        }

        return SharedPtr<To, Frame, AllocatorPtr>{};
    }
}// namespace isl

#endif /* ISL_PROJECT_MEMORY_HPP */

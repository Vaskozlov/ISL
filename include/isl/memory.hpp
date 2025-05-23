#ifndef ISL_PROJECT_MEMORY_HPP
#define ISL_PROJECT_MEMORY_HPP

#include <isl/pool_allocator.hpp>
#include <memory>

namespace isl
{
    template <typename T, auto AllocatorPtr>
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

        template <typename... Ts>
        explicit UniquePtr(Ts &&...args) requires(!std::is_abstract_v<T>)
          : ptr{static_cast<T *>(AllocatorPtr->allocate())}
        {
            std::construct_at(ptr, std::forward<Ts>(args)...);
        }

        UniquePtr(const UniquePtr &other) = delete;

        UniquePtr(UniquePtr &&other) noexcept
          : ptr{other.release()}
        {}

        template <typename U = T>
        requires(std::convertible_to<U *, T *>) // NOLINTNEXTLINE
        UniquePtr(UniquePtr<U, AllocatorPtr> &&other) noexcept
          : ptr{other.release()}
        {}

        ~UniquePtr()
        {
            destroyStoredObject();
        }

        auto operator=(const UniquePtr &other) -> UniquePtr & = delete;

        template <typename U = T>
        requires(std::convertible_to<U *, T *>)
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

    template <std::size_t Capacity, std::size_t Alignment>
    struct SharedPtrFrame
    {
        mutable std::atomic<std::size_t> refCount{1};
        alignas(Alignment) std::byte objectBuffer[Capacity];
        void (*deleter)(void *) = nullptr;

        template <typename T>
        ISL_DECL static auto canStore() noexcept -> bool
        {
            if constexpr (std::same_as<T, void>) {
                return true;
            } else {
                return sizeof(T) <= Capacity && alignof(T) <= Alignment;
            }
        }

        template <typename T>
        requires(canStore<T>())
        [[nodiscard]] auto asPtr() -> T *
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<T *>(std::addressof(objectBuffer[0]));
        }

        template <typename T>
        requires(canStore<T>())
        [[nodiscard]] auto asPtr() const -> const T *
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<T *>(std::addressof(objectBuffer[0]));
        }

        ISL_DECL static auto getCapacity() noexcept -> std::size_t
        {
            return Capacity;
        }

        ISL_DECL static auto getAlignment() noexcept -> std::size_t
        {
            return Alignment;
        }

        auto increaseRefCount() const -> std::size_t
        {
            return refCount.fetch_add(1, std::memory_order_acq_rel);
        }

        [[nodiscard]] auto decreaseRefCount() const -> std::size_t
        {
            return refCount.fetch_sub(1, std::memory_order_acq_rel);
        }

        template <typename T, typename... Ts>
        requires(canStore<T>())
        static auto initialize(SharedPtrFrame *frame, Ts &&...args) -> void
        {
            std::construct_at(std::addressof(frame->refCount), 1U);
            std::construct_at(frame->asPtr<T>(), std::forward<Ts>(args)...);

            frame->deleter = [](void *ptr) { std::destroy_at(static_cast<T *>(ptr)); };
        }
    };

    template <typename... Ts>
    using SharedPtrFrameFor = SharedPtrFrame<ObjectsMaxSize<Ts...>, ObjectsMaxAlignment<Ts...>>;

    template <typename T, typename Frame, auto AllocatorPtr>
    requires(AllocatorPtr->template canAllocate<Frame>())
    class SharedPtr;

    struct FrameMovedT
    {
    };

    struct FrameCopyT
    {
    };

    template <typename T, typename Frame, auto AllocatorPtr>
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

        template <typename... Ts>
        requires(!std::is_abstract_v<T>)
        explicit SharedPtr(Ts &&...args)
          : frame{static_cast<Frame *>(AllocatorPtr->allocate())}
        {
            Frame::template initialize<T>(frame, std::forward<Ts>(args)...);
        }

        template <typename U = T>
        requires(std::convertible_to<U *, T *>) // NOLINTNEXTLINE
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

        template <typename U = T> // NOLINTNEXTLINE
        requires(std::convertible_to<U *, T *>)
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

        template <typename U = T>
        requires(std::convertible_to<U *, T *>)
        auto operator=(const SharedPtr<U, Frame, AllocatorPtr> &other) -> SharedPtr &
        {
            if (static_cast<const void *>(this)
                != static_cast<const void *>(std::addressof(other))) {
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

        template <typename U>
        ISL_DECL static auto canStore() noexcept -> bool
        {
            return Frame::template canStore<U>();
        }

        template <typename U = T>
        requires(std::same_as<T, U>)
        [[nodiscard]] auto operator*() -> U &
        {
            return *frame->template asPtr<U>();
        }

        template <typename U = T>
        requires(std::same_as<T, U>)
        [[nodiscard]] auto operator*() const -> const U &
        {
            return *frame->template asPtr<U>();
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

        template <typename U>
        auto updateDeleter() -> void
        {
            frame->deleter = [](void *ptr) { std::destroy_at(static_cast<U *>(ptr)); };
        }

        template <typename U>
        requires(canStore<U>())
        auto inMemoryCastTo(std::invocable<void *> auto &&cast_function) -> void
        {
            cast_function(frame->template asPtr<U>());
            updateDeleter<U>();
        }

    private:
        auto increaseRefCount() const -> void
        {
            frame->increaseRefCount();
        }

        auto decreaseRefCount() -> void
        {
            if (frame != nullptr && frame->decreaseRefCount() == 1) {
                frame->deleter(frame->template asPtr<T>());
                AllocatorPtr->deallocate(static_cast<void *>(frame));
            }
        }
    };

    template <typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(const SharedPtr<From, Frame, AllocatorPtr> &ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{FrameCopyT{}, ptr.getFrame()};
    }

    template <typename To, typename From, typename Frame, auto AllocatorPtr>
    auto staticPointerCast(SharedPtr<From, Frame, AllocatorPtr> &&ptr)
        -> SharedPtr<To, Frame, AllocatorPtr>
    {
        return SharedPtr<To, Frame, AllocatorPtr>{FrameMovedT{}, ptr.releaseFrame()};
    }

    template <typename To, typename From, typename Frame, auto AllocatorPtr>
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

    template <typename To, typename From, typename Frame, auto AllocatorPtr>
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
} // namespace isl

#endif /* ISL_PROJECT_MEMORY_HPP */

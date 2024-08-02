#ifndef ISL_PROJECT_SPSC_PIPE_HPP
#define ISL_PROJECT_SPSC_PIPE_HPP

#include <array>
#include <atomic>
#include <isl/isl.hpp>

namespace isl::thread::lock_free
{
    template<typename T, std::size_t Size>
    class SpscPipe
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using CursorType = std::atomic<size_type>;

        static_assert(CursorType::is_always_lock_free);

    private:
        class RowBuffer
        {
        private:
            alignas(T) std::array<std::byte, sizeof(T)> rowBuffer;

        public:
            [[nodiscard]] auto getAddress() noexcept -> void *
            {
                return static_cast<void *>(std::addressof(rowBuffer));
            }

            [[nodiscard]] auto getValuePtr() noexcept -> T *
            {
                return static_cast<T *>(getAddress());
            }

            template<typename... Ts>
            auto construct(Ts &&...args) -> void
            {
                std::construct_at(getValuePtr(), std::forward<Ts>(args)...);
            }

            auto destroy() -> void
            {
                std::destroy_at(getValuePtr());
            }
        };

        std::array<RowBuffer, Size> ring;

        ISL_HARDWARE_CACHE_LINE_ALIGN CursorType pushCursor;

        ISL_HARDWARE_CACHE_LINE_ALIGN size_type pushCursorCached{};

        ISL_HARDWARE_CACHE_LINE_ALIGN CursorType popCursor;

        ISL_HARDWARE_CACHE_LINE_ALIGN size_type popCursorCached{};

        ISL_HARDWARE_CACHE_LINE_ALIGN std::atomic_flag hasProducerFinished;

        ISL_HARDWARE_CACHE_LINE_PADDING(size_type);

        [[nodiscard]] static auto isFull(size_type push_cursor, size_t pop_cursor) noexcept -> bool
        {
            return (push_cursor - pop_cursor) == capacity();
        }

        [[nodiscard]] static auto isEmpty(size_type push_cursor, size_t pop_cursor) noexcept -> bool
        {
            return push_cursor == pop_cursor;
        }

    public:
        [[nodiscard]] static auto capacity() noexcept -> size_type
        {
            return Size;
        }

        [[nodiscard]] auto contained() const noexcept -> size_type
        {
            auto push_cursor = pushCursor.load(std::memory_order_relaxed);
            auto pop_cursor = popCursor.load(std::memory_order_relaxed);

            return push_cursor - pop_cursor;
        }

        [[nodiscard]] auto wasEmpty() const noexcept -> bool
        {
            return contained() == 0;
        }

        [[nodiscard]] auto wasFull() const noexcept -> bool
        {
            auto push_cursor = pushCursor.load(std::memory_order_relaxed);
            auto pop_cursor = popCursor.load(std::memory_order_relaxed);

            return isFull(push_cursor, pop_cursor);
        }

        [[nodiscard]] auto isProducerDone() const noexcept -> bool
        {
            return hasProducerFinished.test(std::memory_order_relaxed);
        }

        auto producerDone() noexcept -> void
        {
            hasProducerFinished.test_and_set(std::memory_order_relaxed);
        }

        auto produceResume() noexcept -> void
        {
            hasProducerFinished.clear(std::memory_order_relaxed);
        }

        template<typename... Ts>
        auto tryEmplace(Ts &&...args) -> bool
        {
            auto push_cursor = pushCursor.load(std::memory_order_relaxed);
            auto next_push_cursor = (push_cursor + 1) % Size;

            if (next_push_cursor == popCursorCached) {
                popCursorCached = popCursor.load(std::memory_order_acquire);

                if (next_push_cursor == popCursorCached) {
                    return false;
                }
            }

            ring[push_cursor].construct(std::forward<Ts>(args)...);
            pushCursor.store(next_push_cursor, std::memory_order_release);
            return true;
        }

        auto tryPush(T &value) -> bool
        {
            auto push_cursor = pushCursor.load(std::memory_order_relaxed);
            auto next_push_cursor = (push_cursor + 1) % Size;

            if (next_push_cursor == popCursorCached) {
                popCursorCached = popCursor.load(std::memory_order_acquire);

                if (next_push_cursor == popCursorCached) {
                    return false;
                }
            }

            ring[push_cursor].construct(std::move(value));
            pushCursor.store(next_push_cursor, std::memory_order_release);
            return true;
        }

        auto tryPop(T &value) -> bool
        {
            auto pop_cursor = popCursor.load(std::memory_order_relaxed);

            if (pop_cursor == pushCursorCached) {
                pushCursorCached = pushCursor.load(std::memory_order_acquire);

                if (pop_cursor == pushCursorCached) {
                    return false;
                }
            }

            value = std::move(*ring[pop_cursor].getValuePtr());
            ring[pop_cursor].destroy();

            auto next_pop_cursor = (pop_cursor + 1) % Size;
            popCursor.store(next_pop_cursor, std::memory_order_release);

            return true;
        }
    };
}// namespace isl::thread::lock_free

#endif /* ISL_PROJECT_SPSC_PIPE_HPP */

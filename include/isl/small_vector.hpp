#ifndef ISL_PROJECT_SMALL_VECTOR_HPP
#define ISL_PROJECT_SMALL_VECTOR_HPP

#include <isl/isl.hpp>

namespace isl
{
    template<typename T, std::size_t N, typename Allocator = std::allocator<T>>
    class SmallVector
    {
    private:
        union {
            T *largeStorage{};
            T smallStorage[N];
        };
        u32 vectorSize{};
        u32 vectorCapacity{N};
        Allocator allocator;

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = u32;
        using difference_type = std::ptrdiff_t;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;
        using iterator = T *;
        using const_iterator = const T *;

        SmallVector() = default;

        SmallVector(const std::initializer_list<T> &initializer_list)
        {
            reserve(static_cast<u32>(initializer_list.size()));

            for (const auto &element : initializer_list) {
                emplace_back(element);
            }
        }

        SmallVector(const SmallVector &other)
          : vectorSize{other.vectorSize}
          , allocator{other.allocator}
        {
            auto *buffer = std::add_pointer_t<T>{};

            if (vectorSize <= N) {
                vectorCapacity = N;
                buffer = std::addressof(smallStorage[0]);
            } else {
                vectorCapacity = vectorSize;
                buffer = std::allocator_traits<Allocator>::allocate(allocator, vectorSize);
                largeStorage = buffer;
            }

            const auto *buffer_end = buffer + vectorSize;
            const auto *other_buffer = other.data();

            while (buffer != buffer_end) {
                std::construct_at(buffer, *other_buffer);
                ++buffer;
                ++other_buffer;
            }
        }

        SmallVector(SmallVector &&other) noexcept
          : vectorSize{std::exchange(other.vectorSize, 0)}
          , allocator{other.allocator}
        {
            if (other.capacity() > N) {
                largeStorage = std::exchange(other.largeStorage, nullptr);
                vectorCapacity = std::exchange(other.vectorCapacity, N);
                return;
            }

            vectorCapacity = N;
            auto *buffer = std::addressof(smallStorage[0]);
            const auto *buffer_end = buffer + vectorSize;
            auto *other_buffer = other.data();

            while (buffer != buffer_end) {
                std::construct_at(buffer, std::move(*other_buffer));
                std::destroy_at(other_buffer);
                ++buffer;
                ++other_buffer;
            }
        }

        ~SmallVector()
        {
            auto [buffer, is_small] = dataWithBufferInfo();
            cleanBuffer(buffer, buffer + vectorSize, is_small);
        }

        auto operator=(const SmallVector &other) -> SmallVector &
        {
            if (this == std::addressof(other)) {
                return *this;
            }

            if (capacity() >= other.size()) {
                auto *buffer = data();
                const auto *buffer_end = buffer + vectorSize;
                auto *other_buffer = other.data();
                const auto *other_buffer_end = other_buffer + other.vectorSize;

                while (buffer != buffer_end && other_buffer != other_buffer_end) {
                    *buffer = *other_buffer;
                    ++buffer;
                    ++other_buffer;
                }

                if (vectorSize > other.vectorSize) {
                    while (buffer != buffer_end) {
                        std::destroy_at(buffer);
                        ++buffer;
                    }

                    vectorSize = other.vectorSize;
                    return *this;
                }

                while (other_buffer != other_buffer_end) {
                    std::construct_at(buffer, *other_buffer);
                    ++buffer;
                    ++other_buffer;
                }

                vectorSize = other.vectorSize;
                return *this;
            }

            auto *new_buffer =
                std::allocator_traits<Allocator>::allocate(allocator, other.vectorSize);

            auto *buffer_it = new_buffer;
            auto *buffer_end = new_buffer + other.vectorSize;
            auto *other_buffer_it = other.largeStorage;

            while (buffer_it != buffer_end) {
                std::construct_at(buffer_it, *other_buffer_it);
                ++buffer_it;
                ++other_buffer_it;
            }

            largeStorage = new_buffer;
            vectorSize = other.vectorSize;
            vectorCapacity = vectorSize;
            return *this;
        }

        auto operator=(SmallVector &&other) noexcept -> SmallVector &
        {
            swap(other);
            return *this;
        }

        auto swap(SmallVector &other) noexcept -> void
        {
            if (capacity() > N && other.capacity() > N) {
                std::swap(largeStorage, other.largeStorage);
                std::swap(vectorSize, other.vectorSize);
                std::swap(vectorCapacity, other.vectorCapacity);
                return;
            }

            if (capacity() <= N && other.capacity() <= N) {
                auto *buffer = std::addressof(smallStorage[0]);
                auto *other_buffer = std::addressof(other.smallStorage[0]);
                auto *buffer_end = buffer + vectorSize;
                auto *other_buffer_end = other_buffer + other.vectorSize;

                while (buffer != buffer_end && other_buffer != other_buffer_end) {
                    std::swap(*buffer, *other_buffer);
                    ++buffer;
                    ++other_buffer;
                }

                if (vectorSize > other.vectorSize) {
                    while (buffer != buffer_end) {
                        std::construct_at(other_buffer, std::move(*buffer));
                        std::destroy_at(buffer);
                        ++buffer;
                        ++other_buffer;
                    }

                    std::swap(vectorSize, other.vectorSize);
                    return;
                }

                while (other_buffer != other_buffer_end) {
                    std::construct_at(buffer, std::move(*other_buffer));
                    std::destroy_at(other_buffer);
                    ++buffer;
                    ++other_buffer;
                }

                std::swap(vectorSize, other.vectorSize);
                return;
            }

            if (capacity() <= N) {
                other.swap(*this);
                return;
            }

            // capacity() > N
            auto *self_memory = largeStorage;
            auto *other_buffer = std::addressof(other.smallStorage[0]);

            for (u32 i = 0; i != other.vectorSize; ++i) {
                std::construct_at(std::addressof(smallStorage[i]), std::move(other_buffer[i]));
                std::destroy_at(std::addressof(other_buffer[i]));
            }

            other.largeStorage = self_memory;
            std::swap(vectorSize, other.vectorSize);
            std::swap(vectorCapacity, other.vectorCapacity);
        }

        [[nodiscard]] auto operator==(const SmallVector &other) const -> bool
        {
            if (vectorSize != other.vectorSize) {
                return false;
            }

            return std::ranges::equal(*this, other);
        }

        [[nodiscard]] auto operator<=>(const SmallVector &other) const -> std::weak_ordering
        {
            auto *buffer = data();
            auto *other_buffer = other.data();

            for (u32 i = 0; i != vectorSize && i != other.vectorSize; ++i) {
                if (const auto cmp = buffer[i] <=> other_buffer[i]; cmp != 0) {
                    return cmp;
                }
            }

            return vectorSize <=> other.vectorSize;
        }

        [[nodiscard]] auto front() -> T &
        {
            return *data();
        }

        [[nodiscard]] auto front() const -> const T &
        {
            return *data();
        }

        [[nodiscard]] auto back() -> T &
        {
            return *(data() + vectorSize - 1);
        }

        [[nodiscard]] auto back() const -> const T &
        {
            return *(data() + vectorSize - 1);
        }

        auto operator[](const u32 index) -> T &
        {
            return data()[index];
        }

        auto operator[](const u32 index) const -> const T &
        {
            return data()[index];
        }

        auto at(const u32 index) -> T &
        {
            if (index >= vectorSize) {
                throw std::out_of_range{"SmallVector::at"};
            }

            return operator[](index);
        }

        auto at(const u32 index) const -> const T &
        {
            if (index >= vectorSize) {
                throw std::out_of_range{"SmallVector::at"};
            }

            return operator[](index);
        }

        auto clean() -> void
        {
            std::ranges::destroy(*this);
            vectorSize = 0;
        }

        auto erase(iterator it) -> void
        {
            for (auto *current = it; current != end() - 1; ++current) {
                *current = std::move(*(current + 1));
            }

            vectorSize -= 1;
        }

        template<typename... Ts>
            requires(std::constructible_from<T, Ts...>)
        auto emplace_back(Ts &&...args) -> T &
        {
            auto *buffer = std::add_pointer_t<T>{};

            if (vectorSize == vectorCapacity) {
                reserveNoCheck(vectorCapacity * 2);
                buffer = largeStorage;
            } else {
                buffer = data();
            }

            auto *construction_place = buffer + vectorSize;
            std::construct_at(construction_place, std::forward<Ts>(args)...);
            vectorSize += 1;

            return *construction_place;
        }

        auto pop_back() -> void
        {
            if (vectorSize == 0) {
                return;
            }

            vectorSize -= 1;

            auto *element = data() + vectorSize;
            std::destroy_at(element);
        }

        auto reserve(const u32 new_capacity) -> void
        {
            if (new_capacity <= N) {
                return;
            }

            reserveNoCheck(new_capacity);
        }

        [[nodiscard]] auto size() const noexcept -> u32
        {
            return vectorSize;
        }

        [[nodiscard]] auto empty() const noexcept -> u32
        {
            return size() == 0;
        }

        [[nodiscard]] auto capacity() const noexcept -> u32
        {
            return vectorCapacity;
        }

        [[nodiscard]] auto begin() -> T *
        {
            return data();
        }

        [[nodiscard]] auto begin() const -> const T *
        {
            return data();
        }

        [[nodiscard]] auto cbegin() const -> const T *
        {
            return begin();
        }

        [[nodiscard]] auto end() -> T *
        {
            return data() + vectorSize;
        }

        [[nodiscard]] auto end() const -> const T *
        {
            return data() + vectorSize;
        }

        [[nodiscard]] auto cend() const -> const T *
        {
            return end();
        }

        auto data() -> T *
        {
            if (vectorCapacity <= N) {
                return std::addressof(smallStorage[0]);
            }

            return largeStorage;
        }

        auto data() const -> const T *
        {
            if (vectorCapacity <= N) {
                return std::addressof(smallStorage[0]);
            }

            return largeStorage;
        }

    private:
        auto cleanBuffer(T *buffer_begin, T *buffer_end, bool is_small) -> void
        {
            std::ranges::destroy(buffer_begin, buffer_end);

            if (!is_small) {
                std::allocator_traits<Allocator>::deallocate(
                    allocator, buffer_begin, vectorCapacity);
            }
        }

        auto reserveNoCheck(const u32 new_capacity) -> void
        {
            auto *new_memory = std::allocator_traits<Allocator>::allocate(allocator, new_capacity);
            auto [buffer, is_small] = dataWithBufferInfo();

            auto *buffer_it = buffer;
            auto *buffer_end = buffer + vectorSize;
            auto *new_memory_copy = new_memory;

            while (buffer_it != buffer_end) {
                std::construct_at(new_memory_copy, std::move(*buffer_it));
                ++buffer_it;
                ++new_memory_copy;
            }

            cleanBuffer(buffer, buffer_end, is_small);
            largeStorage = new_memory;
            vectorCapacity = new_capacity;
        }

        auto dataWithBufferInfo() -> std::pair<T *, bool>
        {
            const auto is_small = vectorCapacity <= N;
            auto *buffer_ptr = is_small ? std::addressof(smallStorage[0]) : largeStorage;

            return {buffer_ptr, is_small};
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_SMALL_VECTOR_HPP */

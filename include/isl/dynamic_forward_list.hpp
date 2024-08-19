#ifndef ISL_PROJECT_FORWARD_LIST_HPP
#define ISL_PROJECT_FORWARD_LIST_HPP

#include <isl/isl.hpp>

namespace isl
{
    struct DynamicForwardListNode;

    template<std::derived_from<DynamicForwardListNode> T = DynamicForwardListNode>
    class DynamicForwardList;

    struct DynamicForwardListNode
    {
    private:
        template<std::derived_from<DynamicForwardListNode> T>
        friend class DynamicForwardList;

        DynamicForwardListNode *islDynamicForwardListNext{};

    public:
        template<
            std::derived_from<DynamicForwardListNode> U = DynamicForwardListNode, typename... Ts>
            requires std::constructible_from<U, Ts...>
        auto emplaceAfter(Ts &&...args) -> U *
        {
            auto *new_node = new U{std::forward<Ts>(args)...};

            new_node->islDynamicForwardListNext = std::exchange(
                islDynamicForwardListNext, static_cast<DynamicForwardListNode *>(new_node));

            return new_node;
        }

        template<std::derived_from<DynamicForwardListNode> U = DynamicForwardListNode>
        auto insertAfter(U *new_node) -> U *
        {
            new_node->islDynamicForwardListNext = std::exchange(
                islDynamicForwardListNext, static_cast<DynamicForwardListNode *>(new_node));

            return new_node;
        }
    };

    template<std::derived_from<DynamicForwardListNode> T>
    class DynamicForwardList
    {
    private:
        DynamicForwardListNode *front{};

        template<bool isConst>
        class iterator_base
        {
        private:
            using Pointer = std::conditional_t<
                isConst, const DynamicForwardListNode *, DynamicForwardListNode *>;

            Pointer node;

        public:
            using value_type = T *;
            using reference = T *;
            using pointer = T *;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            iterator_base() = default;

            explicit iterator_base(Pointer list_node)
              : node{list_node}
            {}

            auto operator==(const iterator_base &other) const noexcept -> bool = default;

            auto operator++() -> iterator_base &
            {
                node = node->islDynamicForwardListNext;
                return *this;
            }

            [[nodiscard("Use prefix increment instead")]] auto operator++(int) -> iterator_base
            {
                auto copy = *this;
                this->operator++();

                return copy;
            }

            [[nodiscard]] auto operator*() const -> const T *
            {
                return static_cast<const T *>(node);
            }

            [[nodiscard]] auto operator->() -> T *
            {
                return static_cast<T *>(node);
            }

            [[nodiscard]] auto operator->() const -> const T *
            {
                return static_cast<const T *>(node);
            }
        };

    public:
        using pointer = T *;
        using value_type = T **;

        using iterator = iterator_base<false>;
        using const_iterator = iterator_base<true>;

        DynamicForwardList() = default;

        DynamicForwardList(const DynamicForwardList &) = delete;

        DynamicForwardList(DynamicForwardList &&other) noexcept
          : front{std::exchange(other.front, nullptr)}
        {}

        auto operator=(const DynamicForwardList &other) -> void = delete;

        auto operator=(DynamicForwardList &&other) noexcept -> DynamicForwardList &
        {
            std::swap(front, other.front);
            return *this;
        }

        ~DynamicForwardList()
        {
            while (front != nullptr) {
                popFront();
            }
        }

        template<std::derived_from<T> U = T, typename... Ts>
            requires std::constructible_from<U, Ts...>
        auto emplaceFront(Ts &&...args) -> U *
        {
            auto *new_node = new U{std::forward<Ts>(args)...};
            new_node->islDynamicForwardListNext = std::exchange(front, new_node);

            return new_node;
        }

        auto popFront() -> void
        {
            if (front != nullptr) {
                delete static_cast<T *>(std::exchange(front, front->islDynamicForwardListNext));
            }
        }

        [[nodiscard]] auto begin() -> iterator
        {
            return iterator{front};
        }

        [[nodiscard]] auto begin() const -> const_iterator
        {
            return const_iterator{front};
        }

        [[nodiscard]] auto cbegin() const -> const_iterator
        {
            return begin();
        }

        [[nodiscard]] auto end() -> iterator
        {
            return iterator{nullptr};
        }

        [[nodiscard]] auto end() const -> const_iterator
        {
            return const_iterator{nullptr};
        }

        [[nodiscard]] auto cend() const -> const_iterator
        {
            return end();
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_FORWARD_LIST_HPP */

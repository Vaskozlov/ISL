#ifndef ISL_PROJECT_LAZY_LIST_HPP
#define ISL_PROJECT_LAZY_LIST_HPP

#include <atomic>
#include <isl/isl.hpp>
#include <isl/iterator.hpp>
#include <isl/memory.hpp>

namespace isl::thread
{
    template<typename T>
    class LazyList
    {
    private:
        struct Node
        {
            T data;
            UniquePtr<Node> next;
            Node *prev;

            template<typename... Ts>
            Node(UniquePtr<Node> next_node, Node *prev_node, Ts &&...args)
              : data{std::forward<Ts>(args)...}
              , next{std::move(next_node)}
              , prev{prev_node}
            {}
        };

        isl::UniquePtr<Node> head;

    public:
        using value_type = T;
        using reference = T &;
        using pointer = T *;

        class iterator
        {
        private:
            Node *currentNode{};

        public:
            using value_type = T;
            using reference = T &;
            using pointer = T *;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            iterator() = default;

            explicit iterator(Node *node)
              : currentNode{node}
            {}

            template<typename... Ts>
            auto store(Ts &&...args) -> void
            {
                currentNode->data = T{std::forward<Ts>(args)...};
            }

            [[nodiscard]] auto getNodePtr() -> Node *
            {
                return currentNode;
            }

            [[nodiscard]] auto getNodePtr() const -> const Node *
            {
                return currentNode;
            }

            [[nodiscard]] auto operator*() -> T &
            {
                return currentNode->data;
            }

            [[nodiscard]] auto operator*() const -> const T &
            {
                return currentNode->data;
            }

            auto operator->() -> T *
            {
                return &currentNode->data;
            }

            auto operator->() const -> const T *
            {
                return &currentNode->data;
            }

            auto operator++() -> iterator &
            {
                currentNode = currentNode == nullptr ? nullptr : currentNode->next.get();
                return *this;
            }

            [[nodiscard]] auto operator++(int) -> iterator
            {
                auto copy = *this;
                this->operator++();
                return copy;
            }

            [[nodiscard]] auto operator==(const iterator &other) const noexcept -> bool = default;
        };

        template<typename... Ts>
        auto emplaceFront(Ts &&...args) -> iterator
        {
            auto *old_head = head.get();
            auto new_node = makeUnique<Node>(std::move(head), nullptr, std::forward<Ts>(args)...);
            auto *new_node_ptr = new_node.get();

            if (old_head != nullptr) {
                old_head->prev = new_node.get();
            }

            head = std::move(new_node);
            return iterator{new_node_ptr};
        }

        auto erase(iterator object) -> void
        {
            release(object);
        }

        auto release(iterator object) -> UniquePtr<Node>
        {
            auto *erased_node = object.getNodePtr();
            auto *prev_node = erased_node->prev;
            auto *next_node = erased_node->next.get();
            auto self = UniquePtr<Node>{};

            if (prev_node != nullptr) {
                self = std::move(prev_node->next);
                prev_node->next = std::move(self->next);
            } else {
                self = std::move(head);
                head = std::move(self->next);
            }

            if (next_node != nullptr) {
                next_node->prev = prev_node;
            }

            self->next = nullptr;
            self->prev = nullptr;
            return self;
        }

        auto insertFront(UniquePtr<Node> new_node) -> void
        {
            if (head != nullptr) {
                head->prev = new_node.get();
            }

            new_node->next = std::move(head);
            head = std::move(new_node);
        }

        [[nodiscard]] auto front() -> T &
        {
            return head->data;
        }

        [[nodiscard]] auto front() const -> const T &
        {
            return head->data;
        }

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return head == nullptr;
        }

        [[nodiscard]] auto begin() -> iterator
        {
            return iterator{head.get()};
        }

        [[nodiscard]] auto begin() const -> iterator
        {
            return iterator{head.get()};
        }

        [[nodiscard]] auto cbegin() const -> iterator
        {
            return begin();
        }

        [[nodiscard]] auto end() -> iterator
        {
            return iterator{nullptr};
        }

        [[nodiscard]] auto end() const -> iterator
        {
            return iterator{nullptr};
        }

        [[nodiscard]] auto cend() const -> iterator
        {
            return end();
        }
    };
}// namespace isl::thread

#endif /* ISL_PROJECT_LAZY_LIST_HPP */
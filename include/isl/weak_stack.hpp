#ifndef ISL_PROJECT_WEAK_STACK_HPP
#define ISL_PROJECT_WEAK_STACK_HPP

#include <isl/isl.hpp>
#include <isl/iterator.hpp>
#include <isl/memory.hpp>

namespace isl
{
    template<typename T>
    class WeakStack
    {
    private:
        struct Node
        {
            std::shared_ptr<Node> previous;
            T value;
        };

        std::shared_ptr<Node> tail;

    public:
        using value_type = T;
        using reference = T &;
        using pointer = T *;

        class iterator;

        WeakStack() = default;

        WeakStack(const std::initializer_list<T> &initial_values)
        {
            for (const T &value : initial_values) {
                emplace_back(value);
            }
        }

        [[nodiscard]] auto top() -> T &
        {
            return tail->value;
        }

        [[nodiscard]] auto top() const -> const T &
        {
            return tail->value;
        }

        auto push(const T &value) -> void
        {
            return emplace(value);
        }

        auto push(T &&value) -> void
        {
            return emplace(std::move(value));
        }

        template<typename... Ts>
        auto emplace(Ts &&...args) -> void
        {
            tail = makeShared<Node>(tail, std::forward<Ts>(args)...);
        }

        auto pop() -> void
        {
            auto tmp = std::move(tail);
            tail = tmp->previous;
        }

        [[nodiscard]] auto begin() -> iterator
        {
            return iterator{tail};
        }

        [[nodiscard]] auto begin() const -> iterator
        {
            return iterator{tail};
        }

        [[nodiscard]] auto cbegin() const -> iterator
        {
            return iterator{tail};
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
            return iterator{nullptr};
        }
    };

    template<typename T>
    class WeakStack<T>::iterator
    {
    private:
        std::shared_ptr<Node> currentNode;

    public:
        using value_type = T;
        using reference = T &;
        using pointer = T *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator() = default;

        explicit iterator(std::shared_ptr<Node> current_node)
          : currentNode{std::move(current_node)}
        {}

        auto operator++() -> iterator &
        {
            currentNode = currentNode->previous;
            return *this;
        }

        auto operator++(int) -> iterator
        {
            auto iterator_copy = *this;
            this->operator++();
            return iterator_copy;
        }

        auto operator==(const iterator &other) const -> bool
        {
            return currentNode.get() == other.currentNode.get();
        }

        [[nodiscard]] auto operator*() const -> const T &
        {
            return currentNode->value;
        }

        [[nodiscard]] auto operator->() -> T *
        {
            return &currentNode->value;
        }

        [[nodiscard]] auto operator->() const -> const T &
        {
            return &currentNode->value;
        }
    };
}// namespace isl


#endif /* ISL_PROJECT_WEAK_STACK_HPP */

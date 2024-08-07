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
            std::vector<std::shared_ptr<Node>> previous;
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
                emplace(value);
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

        auto joinWith(WeakStack<T> &other) -> void
        {
            for (auto &node : other.tail->previous) {
                tail->previous.emplace_back(std::move(node));
            }
        }

        template<typename... Ts>
        auto emplace(Ts &&...args) -> void
        {
            tail = makeShared<Node>(
                std::vector<std::shared_ptr<Node>>{tail}, std::forward<Ts>(args)...);
        }

        auto pop() -> void
        {
            auto tmp = std::move(tail);

            if (tmp->previous.size() != 1) {
                throw std::runtime_error("Unable to pop");
            }

            tail = tmp->previous.front();
        }
    };
}// namespace isl


#endif /* ISL_PROJECT_WEAK_STACK_HPP */

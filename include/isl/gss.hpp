#ifndef ISL_PROJECT_GSS_HPP
#define ISL_PROJECT_GSS_HPP

#include <isl/io.hpp>
#include <isl/memory.hpp>

namespace isl
{
    template<Trait<std::is_copy_assignable, std::is_copy_constructible> T>
    class GSStack
    {
    public:
        struct Node;

        using GSSNode = std::shared_ptr<Node>;

        struct Node
        {
            std::vector<GSSNode> previous;
            T value;

            template<typename... Ts>
            explicit Node(Ts &&...args)
              : value{std::forward<Ts>(args)...}
            {}
        };

    private:
        std::vector<GSSNode> head;
        std::size_t level{};
        std::size_t state{};
        std::size_t stackDepth{};

    public:
        [[nodiscard]] auto getLevel() const noexcept -> std::size_t
        {
            return level;
        }

        [[nodiscard]] auto getState() const noexcept -> std::size_t
        {
            return state;
        }

        [[nodiscard]] auto getHead() const ISL_LIFETIMEBOUND -> const std::vector<GSSNode> &
        {
            return head;
        }

        template<typename... Ts>
        auto push(std::size_t new_level, std::size_t new_state, Ts &&...args) -> GSSNode
        {
            level = new_level;
            state = new_state;
            ++stackDepth;

            auto new_node = makeShared<Node>(std::forward<Ts>(args)...);
            std::swap(head, new_node->previous);
            head.emplace_back(new_node);

            return new_node;
        }

        auto pop() -> void
        {
            auto old_head = std::move(head);
            head.clear();

            for (auto &node : old_head) {
                for (auto &prev : node->previous) {
                    head.emplace_back(prev);
                }
            }
        }

        [[nodiscard]] auto dup() const -> GSStack
        {
            return *this;
        }

        [[nodiscard]] auto canMerge(const GSStack &other) const noexcept -> bool
        {
            return this != std::addressof(other) && level == other.level && state == other.state &&
                   stackDepth == other.stackDepth;
        }

        auto merge(GSStack &other) ISL_LIFETIMEBOUND -> GSStack &
        {
            if (!canMerge(other)) {
                throw std::runtime_error{"Unable to merge stack"};
            }

            for (auto &node : other.head) {
                head.emplace_back(std::move(node));
            }

            other.head.clear();
            other.level = 0;
            other.state = 0;
            other.stackDepth = 0;

            return *this;
        }

        template<typename F>
            requires(std::is_invocable_r_v<T, F, std::vector<T>>)
        auto reduce(
            std::size_t new_level, std::size_t new_state, std::size_t number_of_elements,
            F &&function) -> void
        {
            level = new_level;
            state = new_state;
            stackDepth -= number_of_elements - 1;

            auto reduced_alternatives = std::vector<GSSNode>{};

            for (auto &node : head) {
                reduce(number_of_elements - 1, function, node, {}, reduced_alternatives);
            }

            head = std::move(reduced_alternatives);
        }

    private:
        auto reduce(
            std::size_t elements_remaining, auto &&function, GSSNode node, std::vector<T> in_build,
            std::vector<GSSNode> &reduced_alternatives) -> void
        {
            in_build.emplace_back(node->value);

            if (elements_remaining == 0) {
                auto new_node = makeShared<Node>(function(std::move(in_build)));
                new_node->previous = node->previous;
                reduced_alternatives.emplace_back(new_node);
                return;
            }

            for (auto &prev : node->previous) {
                reduce(elements_remaining - 1, function, prev, in_build, reduced_alternatives);
            }
        }
    };

    template<typename T>
    using GSSNode = typename GSStack<T>::GSSNode;
}// namespace isl

#endif /* ISL_PROJECT_GSS_HPP */

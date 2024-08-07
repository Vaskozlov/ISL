#ifndef ISL_PROJECT_GSS_HPP
#define ISL_PROJECT_GSS_HPP

#include <isl/memory.hpp>

namespace isl
{
    template<typename T>
    class ManualGSS
    {
    private:
        struct Node
        {
            std::vector<SharedPtr<Node>> parents;
            T value;

            template<typename... Ts>
            explicit Node(Ts &&...args)
              : value{std::forward<Ts>(args)...}
            {}
        };

    public:
        using value_type = T;
        using reference = T &;
        using pointer = T *;

        template<typename... Ts>
        auto emplace(Node *stack_top, Ts &&...args) -> SharedPtr<Node>
        {
            auto new_node = makeShared<Node>(std::forward<Ts>(args)...);
            stack_top->parents.emplace_back(new_node);

            return new_node;
        }

        auto pop(Node *stack_top) -> SharedPtr<Node>
        {
            if (stack_top->parents.size() != 1) {
                throw std::runtime_error("Unable to pop");
            }

            auto parent = std::move(stack_top->parents.front());
            stack_top->parents.clear();

            return parent;
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_GSS_HPP */

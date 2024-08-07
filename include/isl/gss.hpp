#ifndef ISL_PROJECT_GSS_HPP
#define ISL_PROJECT_GSS_HPP

#include <isl/memory.hpp>

namespace isl
{
    template<typename T>
    class GSSNode
    {
    public:
        using value_type = T;
        using reference = T &;
        using pointer = T *;

        std::vector<SharedPtr<GSSNode<T>>> parents;
        T value;

        GSSNode() = default;

        template<typename... Ts>
        explicit GSSNode(Ts &&...args)
          : value{std::forward<Ts>(args)...}
        {}

        template<typename... Ts>
        auto emplace(Ts &&...args) -> SharedPtr<GSSNode<T>>
        {
            auto new_node = makeShared<GSSNode<T>>(std::forward<Ts>(args)...);
            parents.emplace_back(new_node);

            return new_node;
        }

        auto pop() -> SharedPtr<GSSNode<T>>
        {
            if (parents.size() != 1) {
                throw std::runtime_error("Unable to pop");
            }

            return std::move(parents.front());
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_GSS_HPP */

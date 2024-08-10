#ifndef ISL_PROJECT_GSS_HPP
#define ISL_PROJECT_GSS_HPP

#include <isl/memory.hpp>

namespace isl
{
    template<typename T, auto Comparator = std::equal_to<T>{}>
    class GSS
    {
    public:
        struct GSSNode
        {
            T value;
            std::vector<SharedPtr<GSSNode>> previous;
            std::size_t level{};
            std::size_t state{};

            template<typename... Ts>
            explicit GSSNode(std::size_t node_level, std::size_t node_state, Ts &&...args)
              : value{std::forward<Ts>(args)...}
              , level{node_level}
              , state{node_state}
            {}
        };

    private:
        std::vector<std::vector<SharedPtr<GSSNode>>> levels;

    public:
        [[nodiscard]] auto
            getLevels() const noexcept -> const std::vector<std::vector<SharedPtr<GSSNode>>> &
        {
            return levels;
        }

        template<typename... Ts>
        auto emplace(std::size_t level, std::size_t state, SharedPtr<GSSNode> parent, Ts &&...args)
            -> std::pair<SharedPtr<GSSNode>, bool>
        {
            if (level >= levels.size()) {
                levels.resize(level + 1);
            }

            auto has_inserted = true;
            auto new_node = makeShared<GSSNode>(level, state, std::forward<Ts>(args)...);
            auto possibly_similar_node = findNodeSameNodeAtItsLevelWithSameState(new_node.get());

            if (possibly_similar_node != nullptr) {
                has_inserted = false;
                new_node = std::move(possibly_similar_node);
            } else {
                levels.at(level).emplace_back(new_node);
            }

            if (parent != nullptr) {
                new_node->previous.emplace_back(std::move(parent));
            }

            return {std::move(new_node), has_inserted};
        }

        auto erase(SharedPtr<GSSNode> node) -> void
        {
            auto &nodes_at_level = levels.at(node->level);
            std::erase(nodes_at_level, node);
        }

    private:
        auto findNodeSameNodeAtItsLevelWithSameState(GSSNode *node) -> SharedPtr<GSSNode>
        {
            const auto &nodes_at_level = levels.at(node->level);
            auto node_it = std::ranges::find_if(nodes_at_level, [node](auto &node_at_level) {
                return node->state == node_at_level->state &&
                       Comparator(node->value, node_at_level->value);
            });

            if (node_it == nodes_at_level.end()) {
                return nullptr;
            }

            return *node_it;
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_GSS_HPP */

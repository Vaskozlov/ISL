#ifndef ISL_PROJECT_DOT_REPR_HPP
#define ISL_PROJECT_DOT_REPR_HPP

#include <ankerl/unordered_dense.h>
#include <functional>
#include <isl/gss.hpp>
#include <isl/thread/id_generator.hpp>

namespace isl::dot
{
    namespace detail
    {
        using Edge = std::pair<std::size_t, std::size_t>;

        struct EdgeInfo
        {
            std::string label;
            std::string color = "black";
        };

        struct NodeInfo
        {
            std::string label;
            std::string color = "black";
        };

        struct TreeInformationCommon
        {
            std::set<Edge> edges;
            ankerl::unordered_dense::map<Edge, EdgeInfo> edgesInfo;
            ankerl::unordered_dense::map<std::size_t, NodeInfo> nodesInfo;
            thread::IdGenerator idGenerator{1};

            auto generateDotRepr() const -> std::string;
        };

        template<typename T, typename ToStrArg>
        struct TreeInformation : public TreeInformationCommon
        {
            std::map<T, std::size_t> nodeToId;
            std::function<std::string(ToStrArg)> nodeTypeToString;

            auto createIdForNode(T node) -> std::size_t
            {
                auto it = nodeToId.find(node);

                if (it == nodeToId.end()) {
                    const auto node_id = idGenerator.next();
                    nodeToId.emplace(std::move(node), node_id);
                    return node_id;
                }

                return it->second;
            }
        };

        template<typename T>
        auto createDotRepresentation(
            GSSNode<T>
                node,
            TreeInformation<GSSNode<T>, T> &tree_information) -> void
        {
            auto node_id = tree_information.createIdForNode(node);
            tree_information.nodesInfo.try_emplace(
                node_id,
                NodeInfo{
                    .label = tree_information.nodeTypeToString(node->value),
                });

            for (auto &prev : node->previous) {
                auto prev_id = tree_information.createIdForNode(prev);
                auto edge = Edge{node_id, prev_id};

                if (!tree_information.edges.contains(edge)) {
                    tree_information.edges.emplace(edge);
                    tree_information.edgesInfo.emplace(edge, EdgeInfo{});
                    createDotRepresentation(prev, tree_information);
                }
            }
        }
    }// namespace detail

    template<typename T>
    inline auto createDotRepresentation(
        const GSStack<T> &stack,
        std::function<std::string(T)>
            to_string_function) -> std::string
    {
        using namespace detail;

        auto tree_information = TreeInformation<GSSNode<T>, T>{
            .nodeTypeToString = std::move(to_string_function),
        };

        for (const auto &node : stack.getHead()) {
            createDotRepresentation(node, tree_information);
        }

        return tree_information.generateDotRepr();
    }
}// namespace isl::dot

#endif /* ISL_PROJECT_DOT_REPR_HPP */

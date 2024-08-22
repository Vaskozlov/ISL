#ifndef ISL_PROJECT_DOT_REPR_HPP
#define ISL_PROJECT_DOT_REPR_HPP

#include <ankerl/unordered_dense.h>
#include <functional>
#include <isl/id_generator.hpp>

namespace isl::dot
{
    using Edge = std::pair<std::size_t, std::size_t>;

    struct EdgeInfo
    {
        std::string label{};
        std::string color = "black";
    };

    struct NodeInfo
    {
        std::string label{};
        std::string color = "black";
    };

    struct TreeInformationCommon
    {
        std::set<Edge> edges{};
        ankerl::unordered_dense::map<Edge, EdgeInfo> edgesInfo{};
        ankerl::unordered_dense::map<SmallId, NodeInfo> nodesInfo{};
        IdGenerator<SmallId> idGenerator{1};

        auto generateDotRepr() const -> std::string;
    };

    template<typename T, typename ToStrArg>
    struct TreeInformation : public TreeInformationCommon
    {
        std::map<T, SmallId> nodeToId{};
        std::function<std::string(ToStrArg)> nodeTypeToString;

        auto createIdForNode(T node) -> SmallId
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
}// namespace isl::dot

#endif /* ISL_PROJECT_DOT_REPR_HPP */

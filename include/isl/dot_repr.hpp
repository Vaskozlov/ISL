#ifndef ISL_PROJECT_DOT_REPR_HPP
#define ISL_PROJECT_DOT_REPR_HPP

#include <functional>
#include <isl/gss.hpp>

namespace isl::dot
{
    namespace detail
    {
        static constexpr std::string_view DotFileHeader = R"(
digraph ""
{
 fontname="Helvetica,Arial,sans-serif"
 node [fontname="Helvetica,Arial,sans-serif"]
 edge [fontname="Helvetica,Arial,sans-serif"]
)";

        using Edge = std::pair<std::size_t, std::size_t>;

        template<typename T>
        struct VisualizationData
        {
            std::map<GSSNode<T>, std::size_t> nodeToId;
            std::function<std::string(T)> nodeTypeToString;
            std::set<Edge> edges;
            std::unordered_map<std::size_t, std::string> nodesLabels;
            std::size_t idGenerator{1};

            auto createIdForNode(GSSNode<T> node) -> std::size_t
            {
                auto it = nodeToId.find(node);

                if (it == nodeToId.end()) {
                    nodeToId.emplace(node, idGenerator);
                    return idGenerator++;
                }

                return it->second;
            }
        };

        inline auto addQuotesToEscapingSymbols(std::string_view str) -> std::string
        {
            auto result = std::string{};
            result.reserve(str.size());

            for (auto chr : str) {
                if (chr == '\"') {
                    result.append("\\\"");
                } else {
                    result.push_back(chr);
                }
            }

            return result;
        }

        template<typename T>
        auto createDotRepresentation(GSSNode<T> node, VisualizationData<T> &visualization_data)
            -> void
        {
            auto node_id = visualization_data.createIdForNode(node);
            visualization_data.nodesLabels.try_emplace(
                node_id, visualization_data.nodeTypeToString(node->value));

            for (auto &prev : node->previous) {
                auto prev_id = visualization_data.createIdForNode(prev);
                auto edge = Edge{node_id, prev_id};

                if (!visualization_data.edges.contains(edge)) {
                    visualization_data.edges.emplace(edge);
                    createDotRepresentation(prev, visualization_data);
                }
            }
        }
    }// namespace detail

    template<typename T>
    inline auto createDotRepresentation(
        const GSStack<T> &stack, std::function<std::string(T)> to_string_function) -> std::string
    {
        using namespace detail;

        auto result = std::string{DotFileHeader};
        auto visualization_data = VisualizationData{
            .nodeTypeToString = std::move(to_string_function),
        };

        for (const auto &node : stack.getHead()) {
            createDotRepresentation(node, visualization_data);
        }

        for (auto edge : visualization_data.edges) {
            auto [parent, child] = edge;
            result += fmt::format(" n{} -> n{} [color=\"{}\"];\n", parent, child, "black");
        }

        for (auto &[key, repr] : visualization_data.nodesLabels) {
            result += fmt::format(" n{} [label=\"{}\"]\n", key, addQuotesToEscapingSymbols(repr));
        }

        result.append("}\n");
        return result;
    }
}// namespace isl::dot

#endif /* ISL_PROJECT_DOT_REPR_HPP */

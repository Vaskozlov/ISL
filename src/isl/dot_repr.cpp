#include <isl/dot_repr.hpp>

namespace isl::dot
{
    static constexpr std::string_view DotFileHeader = R"(
digraph ""
{
 fontname="Helvetica,Arial,sans-serif"
 node [fontname="Helvetica,Arial,sans-serif"]
 edge [fontname="Helvetica,Arial,sans-serif"]
)";

    static auto addQuotesToEscapingSymbols(std::string_view str) -> std::string
    {
        auto result = std::string{};
        result.reserve(str.size());

        for (auto chr : str) {
            if (chr == '\"') {
                result.append("\\\"");
            } else if (chr == '\\') {
                result.append("\\\\");
            } else {
                result.push_back(chr);
            }
        }

        return result;
    }

    auto TreeInformationCommon::generateDotRepr() const -> std::string
    {
        auto result = std::string{DotFileHeader};

        for (const auto &edge : edges) {
            auto [parent, child] = edge;
            const auto &edge_info = edgesInfo.at(edge);

            result += fmt::format(
                " n{} -> n{} [label=\"{}\", color=\"{}\"];\n",
                parent,
                child,
                edge_info.label,
                edge_info.color);
        }

        for (const auto &[key, node_info] : nodesInfo) {
            result += fmt::format(
                " n{} [color=\"{}\",label=\"{}\"]\n",
                key,
                node_info.color,
                addQuotesToEscapingSymbols(node_info.label));
        }

        result.append("}\n");
        return result;
    }
}// namespace isl::dot

#include <isl/dot_repr.hpp>

namespace isl::dor::detail
{
    auto addQuotesToEscapingSymbols(std::string_view str) -> std::string
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
}// namespace isl::dor::detail

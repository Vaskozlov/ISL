#include <isl/string_view.hpp>

template class isl::BasicStringView<char>;

auto fmt::formatter<isl::string_view>::format(
    const isl::string_view &str, format_context &ctx) const -> decltype(ctx.out())
{
    return formatter<std::string_view>::format(isl::as<std::string_view>(str), ctx);
}

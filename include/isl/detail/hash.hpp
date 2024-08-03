#ifndef ISL_PROJECT_HASH_HPP
#define ISL_PROJECT_HASH_HPP

#include <ankerl/unordered_dense.h>
#include <isl/detail/defines.hpp>
#include <string_view>

namespace isl::hash
{
    ISL_DECL auto combine(std::size_t first) -> std::size_t
    {
        return first;
    }

    template<typename... Rest>
    ISL_DECL auto combine(std::size_t first, std::size_t second, Rest... rest) -> std::size_t
    {
        first = ankerl::unordered_dense::detail::wyhash::mix(first, second);
        return combine(first, rest...);
    }
}// namespace isl::hash

#endif /* ISL_PROJECT_HASH_HPP */

#ifndef ISL_PROJECT_HASH_HPP
#define ISL_PROJECT_HASH_HPP

#include <isl/detail/defines.hpp>

namespace isl::hash
{
    ISL_DECL auto combine(const std::size_t first) -> std::size_t
    {
        return first;
    }

    template<typename... Rest>
    ISL_DECL auto combine(std::size_t first, const std::size_t second, Rest... rest) -> std::size_t
    {
        // NOLINTNEXTLINE
        first ^= second + 0x9e3779b9ULL + (first << 6ULL) + (first >> 2ULL);
        return combine(first, rest...);
    }
}// namespace isl::hash

#endif /* ISL_PROJECT_HASH_HPP */

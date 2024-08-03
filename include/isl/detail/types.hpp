#ifndef ISL_PROJECT_TYPES_HPP
#define ISL_PROJECT_TYPES_HPP

#include <ankerl/unordered_dense.h>
#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <isl/detail/as.hpp>
#include <isl/detail/container_guard.hpp>
#include <isl/detail/pair.hpp>
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace isl
{
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using Id = std::size_t;
    using ssize_t = std::intmax_t;

    using f32 = std::float_t;
    using f64 = std::double_t;

    template<typename T>
    using Optional = std::optional<T>;

    template<typename T1, typename T2>
    using Pair = std::conditional_t<
        std::is_trivial_v<T1> && std::is_trivial_v<T2>, detail::TrivialPair<T1, T2>,
        std::pair<T1, T2>>;

    template<typename T, std::size_t Size>
    using CArray = T[Size];

    template<typename Value>
    using Set = ankerl::unordered_dense::set<Value>;

    template<typename Key, typename Value>
    using Map = ankerl::unordered_dense::map<Key, Value>;
}// namespace isl

#endif /* ISL_PROJECT_TYPES_HPP */

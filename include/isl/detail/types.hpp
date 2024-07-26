#ifndef ISL_PROJECT_TYPES_HPP
#define ISL_PROJECT_TYPES_HPP

#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <isl/detail/as.hpp>
#include <isl/detail/container_guard.hpp>
#include <isl/detail/pair.hpp>
#include <map>
#include <optional>
#include <set>
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

    template<typename T, std::size_t Size>
    CONTAINER_GUARD(Array, std::array, array, T, Size);

    template<
        typename CharT, typename Traits = std::char_traits<CharT>,
        typename Allocator = std::allocator<CharT>>
    CONTAINER_GUARD(BasicString, std::basic_string, basic_string, CharT, Traits, Allocator);

    template<typename T, typename Alloc = std::allocator<T>>
    CONTAINER_GUARD(Vector, std::vector, vector, T, Alloc);

    template<typename T, typename Alloc = std::allocator<T>>
    CONTAINER_GUARD(Stack, std::stack, stack, T, isl::Vector<T, Alloc>);

    template<class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
    CONTAINER_GUARD(Set, std::set, set, Key, Compare, Allocator);

    template<
        class Key, class T, class Compare = std::less<Key>,
        class Allocator = std::allocator<std::pair<const Key, T>>>
    CONTAINER_GUARD(Map, std::map, map, Key, T, Compare, Allocator);

    template<
        class Value, class Hash = std::hash<Value>, class Pred = std::equal_to<Value>,
        class Alloc = std::allocator<Value>>
    CONTAINER_GUARD(UnorderedSet, std::unordered_set, unordered_set, Value, Hash, Pred, Alloc);

    template<
        class Key, class Tp, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>,
        class Alloc = std::allocator<std::pair<const Key, Tp>>>
    CONTAINER_GUARD(UnorderedMap, std::unordered_map, unordered_map, Key, Tp, Hash, Pred, Alloc);

    using String = BasicString<char>;
    using U8String = BasicString<char8_t>;
    using U16String = BasicString<char16_t>;
    using U32String = BasicString<char32_t>;
    using WString = BasicString<wchar_t>;

    template<typename T1, typename T2>
    using Pair = std::conditional_t<
        std::is_trivial_v<T1> && std::is_trivial_v<T2>, detail::TrivialPair<T1, T2>,
        std::pair<T1, T2>>;

    template<typename T, std::size_t Size>
    using CArray = T[Size];
}// namespace isl

#endif /* ISL_PROJECT_TYPES_HPP */

module;

#include <array>
#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

export module isl:types;

export import :pair;
import :container_guard;

export namespace isl
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

    template<typename T, size_t Size>
    using Array = detail::ContainerGuard<std::array<T, Size>>;

    template<typename T, typename Alloc = std::allocator<T>>
    using Vector = detail::ContainerGuard<std::vector<T, Alloc>>;

    using String = detail::ContainerGuard<std::string>;
    using U8String = detail::ContainerGuard<std::u8string>;
    using U16String = detail::ContainerGuard<std::u16string>;
    using U32String = detail::ContainerGuard<std::u32string>;
    using WString = detail::ContainerGuard<std::wstring>;

    template<typename T1, typename T2>
    using Pair = std::conditional_t<
        Trivial<T1> && Trivial<T2>, detail::TrivialPair<T1, T2>, std::pair<T1, T2>>;

    template<typename T, size_t Size>
    using CArray = T[Size];
}// namespace isl

int test()
{
int16_t data[5] = {};
int16_t *A = data;
int16_t *B = A;
int16_t C = 5;
int16_t D = 0x7FFF;

for (; C > 0; --C) {
    register int16_t AC = *B;
    ++B;

    if (AC >= 0) {
        continue;
    }

    if (AC - D >= 0) {
        D = AC;
    }
}
}
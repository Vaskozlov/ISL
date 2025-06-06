#ifndef CCL_PROJECT_UTF8_HPP
#define CCL_PROJECT_UTF8_HPP

#include <bit>
#include <cstddef>
#include <isl/isl.hpp>
#include <iterator>
#include <numeric>

namespace isl::utf8
{
    [[nodiscard]] consteval auto operator"" _B(unsigned long long value) -> std::byte
    {
        if (value > std::numeric_limits<u8>::max()) {
            throw std::logic_error(
                "Unable to convert integer, which is greater than 255, to a byte");
        }

        return as<std::byte>(value);
    }

    namespace detail
    {
        constexpr inline u32 OneByteMax = 127;
        constexpr inline u32 TwoBytesMax = 2047;
        constexpr inline u32 TreeBytesMax = 65'535;
        constexpr inline u32 FourBytesMax = 1'114'111;

        constexpr inline auto OneByteMask = 0b1000'0000_B;
        constexpr inline auto TwoBytesMask = 0b1110'0000_B;
        constexpr inline auto TwoBytesSignature = 0b1100'0000_B;
        constexpr inline auto TreeBytesMask = 0b1111'0000_B;
        constexpr inline auto TreeBytesSignature = 0b1110'0000_B;
        constexpr inline auto FourBytesMask = 0b1111'1000_B;
        constexpr inline auto FourBytesSignature = 0b1111'0000_B;
        constexpr inline auto ContinuationMask = 0b1100'0000_B;
        constexpr inline auto ContinuationSignature = 0b1000'0000_B;
        constexpr inline u8 TrailingSize = 6;

        constexpr inline std::array UtfMasks{
            0_B, OneByteMask, TwoBytesMask, TreeBytesMask, FourBytesMask};
    } // namespace detail

    ISL_DECL auto isTrailingCharacter(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & detail::ContinuationMask) == detail::ContinuationSignature;
    }

    ISL_DECL auto isOneByteSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & detail::OneByteMask) == 0_B;
    }

    ISL_DECL auto isTwoBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & detail::TwoBytesMask) == detail::TwoBytesSignature;
    }

    ISL_DECL auto isThreeBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & detail::TreeBytesMask) == detail::TreeBytesSignature;
    }

    ISL_DECL auto isFourBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & detail::FourBytesMask) == detail::FourBytesSignature;
    }

    ISL_DECL auto getMask(u16 size) -> std::byte
    {
        return detail::UtfMasks.at(size);
    }

    ISL_DECL auto size(char chr) noexcept -> u16
    {
        if (isOneByteSize(chr)) [[likely]] {
            return 1;
        }

        if (isTwoBytesSize(chr)) [[unlikely]] {
            return 2;
        }

        if (isThreeBytesSize(chr)) [[unlikely]] {
            return 3;
        }

        if (isFourBytesSize(chr)) [[unlikely]] {
            return 4;
        }

        return 0;
    }

    template <typename T>
    constexpr auto
        appendUtf32ToUtf8Container(std::back_insert_iterator<T> back_inserter, char32_t chr) -> void
    {
        using namespace detail;

        constexpr auto non_continuation_mask = ~ContinuationMask;

        // NOLINTBEGIN

        if (chr <= OneByteMax) [[likely]] {
            back_inserter = as<char>(chr);
            return;
        }

        if (chr <= TwoBytesMax) {
            back_inserter = as<char>(TwoBytesSignature | as<std::byte>(chr >> 6));
            back_inserter =
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask));
            return;
        }

        if (chr <= TreeBytesMax) {
            back_inserter = as<char>(TreeBytesSignature | as<std::byte>(chr >> 12));
            back_inserter =
                as<char>(ContinuationSignature | (as<std::byte>(chr >> 6) & non_continuation_mask));
            back_inserter =
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask));
            return;
        }

        if (chr <= FourBytesMax) {
            back_inserter = as<char>(FourBytesSignature | as<std::byte>(chr >> 18));
            back_inserter = as<char>(
                ContinuationSignature | (as<std::byte>(chr >> 12) & non_continuation_mask));
            back_inserter =
                as<char>(ContinuationSignature | (as<std::byte>(chr >> 6) & non_continuation_mask));
            back_inserter =
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask));
            return;
        }

        throw std::invalid_argument{"unable to convert symbol to utf8"};
        // NOLINTEND
    }
} // namespace isl::utf8

#endif /* CCL_PROJECT_UTF8_HPP */

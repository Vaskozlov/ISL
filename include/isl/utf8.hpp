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

    template<typename T>
    concept ValueTypeUtf8 = IsSameToAny<typename T::value_type, char, char8_t>;

    namespace constants
    {
        constexpr u32 OneByteMax = 127;
        constexpr u32 TwoBytesMax = 2047;
        constexpr u32 TreeBytesMax = 65'535;
        constexpr u32 FourBytesMax = 1'114'111;

        constexpr auto OneByteMask = 0b1000'0000_B;
        constexpr auto TwoBytesMask = 0b1110'0000_B;
        constexpr auto TwoBytesSignature = 0b1100'0000_B;
        constexpr auto TreeBytesMask = 0b1111'0000_B;
        constexpr auto TreeBytesSignature = 0b1110'0000_B;
        constexpr auto FourBytesMask = 0b1111'1000_B;
        constexpr auto FourBytesSignature = 0b1111'0000_B;
        constexpr auto ContinuationMask = 0b1100'0000_B;
        constexpr auto ContinuationSignature = 0b1000'0000_B;
        constexpr u8 TrailingSize = 6;

        constexpr std::array<std::byte, 5> UtfMasks{
            0_B, OneByteMask, TwoBytesMask, TreeBytesMask, FourBytesMask};
    }// namespace constants

    ISL_DECL auto isTrailingCharacter(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & constants::ContinuationMask) ==
               constants::ContinuationSignature;
    }

    ISL_DECL auto isOneByteSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & constants::OneByteMask) == 0_B;
    }

    ISL_DECL auto isTwoBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & constants::TwoBytesMask) == constants::TwoBytesSignature;
    }

    ISL_DECL auto isThreeBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & constants::TreeBytesMask) == constants::TreeBytesSignature;
    }

    ISL_DECL auto isFourBytesSize(char chr) noexcept -> bool
    {
        return (as<std::byte>(chr) & constants::FourBytesMask) == constants::FourBytesSignature;
    }

    ISL_DECL auto getMask(u16 size) -> std::byte
    {
        return constants::UtfMasks.at(size);
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

    template<ValueTypeUtf8 T>
    constexpr auto appendUtf32ToUtf8Container(T &string, char32_t chr) -> void
    {
        using namespace constants;
        using namespace std::string_view_literals;

        constexpr auto non_continuation_mask = ~ContinuationMask;

        // NOLINTBEGIN

        if (chr <= OneByteMax) [[likely]] {
            string.push_back(as<char>(chr));
        } else if (chr <= TwoBytesMax) {
            string.push_back(as<char>(TwoBytesSignature | as<std::byte>(chr >> 6)));
            string.push_back(
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask)));
        } else if (chr <= TreeBytesMax) {
            string.push_back(as<char>(TreeBytesSignature | as<std::byte>(chr >> 12)));
            string.push_back(as<char>(
                ContinuationSignature | (as<std::byte>(chr >> 6) & non_continuation_mask)));
            string.push_back(
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask)));
        } else if (chr <= FourBytesMax) {
            string.push_back(as<char>(FourBytesSignature | as<std::byte>(chr >> 18)));
            string.push_back(as<char>(
                ContinuationSignature | (as<std::byte>(chr >> 12) & non_continuation_mask)));
            string.push_back(as<char>(
                ContinuationSignature | (as<std::byte>(chr >> 6) & non_continuation_mask)));
            string.push_back(
                as<char>(ContinuationSignature | (as<std::byte>(chr) & non_continuation_mask)));
        } else {
            throw std::invalid_argument{"unable to convert symbol to utf8"};
        }
        // NOLINTEND
    }
}// namespace isl::utf8

#endif /* CCL_PROJECT_UTF8_HPP */

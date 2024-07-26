#ifndef CCL_PROJECT_UTF_SET_HPP
#define CCL_PROJECT_UTF_SET_HPP

#include <bitset>
#include <isl/range.hpp>
#include <unordered_set>

namespace isl
{
    class UtfSet
    {
    public:
        static constexpr auto asciiStorageSize = as<size_t>(128);

    private:
        std::bitset<asciiStorageSize> asciiSymbolsStorage;
        std::unordered_set<char32_t> nonAsciiStorage;

    public:
        UtfSet() = default;

        UtfSet(
            std::bitset<asciiStorageSize>
                ascii_symbols,
            std::unordered_set<char32_t>
                non_ascii_symbols)
          : asciiSymbolsStorage{ascii_symbols}
          , nonAsciiStorage{std::move(non_ascii_symbols)}
        {}

        UtfSet(
            std::bitset<asciiStorageSize>
                ascii_symbols,
            const std::vector<Range<char32_t>> &ranges)
          : asciiSymbolsStorage{ascii_symbols}
        {
            for (const auto range : ranges) {
                set(range, true);
            }
        }


        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return asciiSymbolsStorage.none() && nonAsciiStorage.empty();
        }

        [[nodiscard]] auto at(char32_t chr) const noexcept -> bool
        {
            if (chr < asciiStorageSize) [[likely]] {
                return asciiSymbolsStorage.test(chr);
            }

            return nonAsciiStorage.contains(chr);
        }

        auto set(char32_t chr, bool value = true) -> void
        {
            if (chr < asciiStorageSize) [[likely]] {
                asciiSymbolsStorage.set(chr, value);
            } else [[unlikely]] {
                setBigChar(chr, value);
            }
        }

        auto set(Range<char32_t> range, bool value = true) -> void
        {
            ISL_UNROLL_N(4)
            for (const char32_t chr : range) {
                set(chr, value);
            }
        }

    private:
        auto setBigChar(char32_t chr, bool value) -> void
        {
            if (value) [[likely]] {
                nonAsciiStorage.insert(chr);
            } else [[unlikely]] {
                nonAsciiStorage.erase(chr);
            }
        }
    };
}// namespace isl

#endif /* CCL_PROJECT_UTF_SET_HPP */

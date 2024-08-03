#include <isl/utf_set.hpp>

namespace isl
{
    UtfSet::UtfSet(std::bitset<asciiStorageSize> ascii_symbols, Set<char32_t> non_ascii_symbols)
      : asciiSymbolsStorage{ascii_symbols}
      , nonAsciiStorage{std::move(non_ascii_symbols)}
    {}

    UtfSet::UtfSet(
        std::bitset<asciiStorageSize>
            ascii_symbols,
        const std::vector<Range<char32_t>> &ranges)
      : asciiSymbolsStorage{ascii_symbols}
    {
        for (const auto range : ranges) {
            set(range, true);
        }
    }

    auto UtfSet::at(char32_t chr) const noexcept -> bool
    {
        if (chr < asciiStorageSize) [[likely]] {
            return asciiSymbolsStorage.test(chr);
        }

        return nonAsciiStorage.contains(chr);
    }

    auto UtfSet::set(char32_t chr, bool value) -> void
    {
        if (chr < asciiStorageSize) [[likely]] {
            asciiSymbolsStorage.set(chr, value);
        } else [[unlikely]] {
            setBigChar(chr, value);
        }
    }

    auto UtfSet::set(Range<char32_t> range, bool value) -> void
    {
        ISL_UNROLL_N(4)
        for (const char32_t chr : range) {
            set(chr, value);
        }
    }

    auto UtfSet::setBigChar(char32_t chr, bool value) -> void
    {
        if (value) [[likely]] {
            nonAsciiStorage.insert(chr);
        } else [[unlikely]] {
            nonAsciiStorage.erase(chr);
        }
    }
}// namespace isl

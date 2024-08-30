#include <isl/utf_set.hpp>

namespace isl
{
    UtfSet::UtfSet(
        const std::bitset<asciiStorageSize>
            ascii_symbols,
        ankerl::unordered_dense::set<char32_t>
            non_ascii_symbols)
      : asciiSymbolsStorage{ascii_symbols}
      , nonAsciiStorage{std::move(non_ascii_symbols)}
    {}

    UtfSet::UtfSet(
        const std::bitset<asciiStorageSize>
            ascii_symbols,
        const std::vector<Range<char32_t>> &ranges)
      : asciiSymbolsStorage{ascii_symbols}
    {
        for (const auto range : ranges) {
            set(range, true);
        }
    }

    auto UtfSet::at(const char32_t chr) const noexcept -> bool
    {
        if (chr < asciiStorageSize) {
            return asciiSymbolsStorage.test(chr);
        }

        return nonAsciiStorage.contains(chr);
    }

    auto UtfSet::set(char32_t chr, bool value) -> void
    {
        if (chr < asciiStorageSize) {
            asciiSymbolsStorage.set(chr, value);
            return;
        }

        setBigChar(chr, value);
    }

    auto UtfSet::set(const Range<char32_t> range, const bool value) -> void
    {
        ISL_UNROLL_N(4)
        for (const char32_t chr : range) {
            set(chr, value);
        }
    }

    auto UtfSet::setBigChar(const char32_t chr, const bool value) -> void
    {
        if (value) {
            nonAsciiStorage.insert(chr);
            return;
        }

        nonAsciiStorage.erase(chr);
    }
}// namespace isl

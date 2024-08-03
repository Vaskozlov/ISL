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
        Set<char32_t> nonAsciiStorage;

    public:
        UtfSet() = default;

        UtfSet(std::bitset<asciiStorageSize> ascii_symbols, Set<char32_t> non_ascii_symbols);

        UtfSet(
            std::bitset<asciiStorageSize>
                ascii_symbols,
            const std::vector<Range<char32_t>> &ranges);

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return asciiSymbolsStorage.none() && nonAsciiStorage.empty();
        }

        [[nodiscard]] auto at(char32_t chr) const noexcept -> bool;

        auto set(char32_t chr, bool value = true) -> void;
        auto set(Range<char32_t> range, bool value = true) -> void;

    private:
        auto setBigChar(char32_t chr, bool value) -> void;
    };
}// namespace isl

#endif /* CCL_PROJECT_UTF_SET_HPP */

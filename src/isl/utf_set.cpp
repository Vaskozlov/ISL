#include "isl/utf_set.hpp"

namespace isl
{
    auto UtfSet::empty() const noexcept -> bool
    {
        return smallStorage.none() && storage.empty();
    }

    auto UtfSet::at(char32_t chr) const noexcept -> bool
    {
        if (chr < smallStorageSize) [[likely]] {
            return smallStorage.test(chr);
        }

        return storage.contains(chr);
    }

    auto UtfSet::set(char32_t chr, bool value) -> void
    {
        if (chr < smallStorageSize) [[likely]] {
            smallStorage.set(chr, value);
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
            storage.insert(chr);
        } else [[unlikely]] {
            storage.erase(chr);
        }
    }
}// namespace isl

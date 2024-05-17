#ifndef CCL_PROJECT_UTF_SET_HPP
#define CCL_PROJECT_UTF_SET_HPP

#include <bitset>
#include <isl/range.hpp>
#include <unordered_set>

namespace isl
{
    class UtfSet
    {
        static constexpr auto smallStorageSize = 128ZU;

        std::bitset<smallStorageSize> smallStorage{};
        std::unordered_set<char32_t> storage;

    public:
        UtfSet() = default;

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return smallStorage.none() && storage.empty();
        }

        [[nodiscard]] auto at(char32_t chr) const noexcept -> bool
        {
            if (chr < smallStorageSize) [[likely]] {
                return smallStorage.test(chr);
            }

            return storage.contains(chr);
        }

        auto set(char32_t chr, bool value = true) -> void
        {
            if (chr < smallStorageSize) [[likely]] {
                smallStorage.set(chr, value);
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
                storage.insert(chr);
            } else [[unlikely]] {
                storage.erase(chr);
            }
        }
    };
}// namespace isl

#endif /* CCL_PROJECT_UTF_SET_HPP */

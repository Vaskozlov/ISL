module;
#include <isl/core/defines.hpp>

export module isl.collection:utf_set;

export import isl.core;
export import :range;

export namespace isl
{
    class UtfSet
    {
        static constexpr auto smallStorageSize = as<std::size_t>(128);

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

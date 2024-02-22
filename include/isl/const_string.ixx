module;

#include <array>
#include <string_view>
#include <isl/detail/defines.hpp>

export module isl:const_string;

import :string_view;
import :iterator;
import :types;

export namespace isl
{
    template<size_t Size>
    class ConstString : public AutoImplementedIteratorMethods<ConstString<Size>>
    {
    public:
        using value_type = char;
        using storage_t = std::array<char, Size>;
        using const_iterator = typename storage_t::const_iterator;
        using const_reverse_iterator = typename storage_t::const_reverse_iterator;

        storage_t string{};

        // NOLINTNEXTLINE
        [[nodiscard]] consteval ConstString(const CArray<value_type, Size> &str)
          : string{std::to_array(str)}
        {}

        // NOLINTNEXTLINE
        [[nodiscard]] consteval ConstString(const std::array<value_type, Size> &str)
          : string{str}
        {}

        ISL_DECL auto begin() const noexcept -> const_iterator
        {
            return string.begin();
        }

        ISL_DECL auto end() const noexcept -> const_iterator
        {
            return string.begin() + size();
        }

        ISL_DECL auto size() const noexcept -> size_t
        {
            return string.size() - 1;
        }

        ISL_DECL auto empty() const noexcept -> bool
        {
            return size() == 0;
        }

        // NOLINTNEXTLINE implicit conversion
        ISL_DECL operator isl::string_view() const noexcept
        {
            return {string.data(), size()};
        }

        // NOLINTNEXTLINE implicit conversion
        ISL_DECL operator std::string_view() const noexcept
        {
            return {string.begin(), string.begin() + size()};
        }

        ISL_DECL auto at(size_t index) const -> char
        {
            return string.at(index);
        }

        ISL_DECL auto operator[](size_t index) const noexcept -> char
        {
            return string[index];
        }

        ISL_DECL auto
            operator<=>(const ConstString &other) const noexcept -> std::weak_ordering = default;
    };
}// namespace isl

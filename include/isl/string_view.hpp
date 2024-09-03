#ifndef ISL_PROJECT_STRING_VIEW_HPP
#define ISL_PROJECT_STRING_VIEW_HPP

#include <algorithm>
#include <ankerl/unordered_dense.h>
#include <isl/isl.hpp>
#include <isl/iterator.hpp>
#include <isl/utf8.hpp>

namespace isl
{
    namespace detail
    {
        template<typename T>
        ISL_DECL auto strlen(T &&str) noexcept -> std::size_t
        {
            return std::basic_string_view{std::forward<T>(str)}.size();
        }
    }// namespace detail

    template<CharacterLiteral CharT>
    class BasicStringView;

    using string_view = BasicStringView<char>;
    using u8string_view = BasicStringView<char8_t>;
    using u16string_view = BasicStringView<char16_t>;
    using u32string_view = BasicStringView<char16_t>;
    using wstring_view = BasicStringView<wchar_t>;

    template<typename T, typename CharT>
    concept StringLike = IsSameToAny<
        std::remove_cvref_t<T>,
        BasicStringView<CharT>,
        std::basic_string_view<CharT>,
        std::basic_string<CharT>>;

    template<CharacterLiteral CharT>
    class BasicStringView : public AutoImplementedIteratorMethods<BasicStringView<CharT>>
    {
    public:
        using pointer = const CharT *;
        using iterator = pointer;
        using value_type = CharT;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;

        static constexpr auto npos = std::string_view::npos;

    private:
        pointer string{};
        std::size_t length{};

    public:
        BasicStringView() noexcept = default;

        template<std::size_t N>
        constexpr explicit BasicStringView(const std::array<CharT, N> &char_array) noexcept
          : string{char_array.data()}
          , length{char_array.size()}
        {}

        constexpr BasicStringView(
            const pointer char_pointer,
            const std::size_t string_length) noexcept
          : string{char_pointer}
          , length{string_length}
        {}

        constexpr BasicStringView(iterator first, iterator last) noexcept
          : string{first}
          , length{distance(first, last)}
        {}

        // NOLINTNEXTLINE
        constexpr BasicStringView(const AnyTrait<std::is_pointer, std::is_array> auto &str) noexcept
          : string{str}
          , length{detail::strlen(str)}
        {}

        // NOLINTNEXTLINE
        constexpr BasicStringView(std::string_view str) noexcept
          : string{str.data()}
          , length{str.size()}
        {}

        // NOLINTNEXTLINE
        constexpr BasicStringView(const std::string &str) noexcept
          : string{str.c_str()}
          , length{str.size()}
        {}

        ISL_DECL auto size() const noexcept -> std::size_t
        {
            return length;
        }

        ISL_DECL auto empty() const noexcept -> bool
        {
            return size() == 0;
        }

        ISL_DECL auto data() const noexcept -> pointer
        {
            return begin();
        }

        ISL_DECL auto begin() const noexcept -> iterator
        {
            return string;
        }

        ISL_DECL auto end() const noexcept -> iterator
        {
            return string + length;
        }

        ISL_DECL auto substr(std::size_t first) const noexcept -> BasicStringView
        {
            first = std::min(size(), first);

            return {begin() + first, end()};
        }

        ISL_DECL auto
            substr(std::size_t first, const std::size_t len) const noexcept -> BasicStringView
        {
            auto last = first + len;

            last = std::min(size(), last);
            first = std::min(size(), first);

            return {begin() + first, begin() + last};
        }

        ISL_DECL auto withoutLastSymbol() const noexcept -> BasicStringView
            requires(std::same_as<CharT, char> || std::same_as<CharT, char8_t>)
        {
            auto truncated = *this;

            while (!truncated.empty() && utf8::isTrailingCharacter(truncated.back())) {
                truncated.length -= 1;
            }

            if (!truncated.empty()) {
                truncated.length -= 1;
            }

            return truncated;
        }

        ISL_DECL auto withoutLastSymbol() const noexcept -> BasicStringView
            requires(!(std::same_as<CharT, char> || std::same_as<CharT, char8_t>))
        {
            return substr(0, size() - 1);
        }

        ISL_DECL auto find(CharT chr, std::size_t offset = 0) const noexcept -> std::size_t
        {
            if (offset >= length) {
                return npos;
            }

            const auto it_to_elem = std::ranges::find(begin() + offset, end(), chr);
            return it_to_elem == end() ? npos : distance(begin(), it_to_elem);
        }

        ISL_DECL auto find(CharT chr, iterator from) const noexcept -> std::size_t
        {
            if (from >= end()) {
                return npos;
            }

            const auto it_to_elem = std::ranges::find(from, end(), chr);
            return it_to_elem == end() ? npos : distance(begin(), it_to_elem);
        }

        ISL_DECL auto contains(CharT chr) const noexcept -> bool
        {
            return find(chr) != npos;
        }

        ISL_DECL auto findRangeEnd(CharT range_start, CharT range_end) const noexcept -> std::size_t
        {
            auto pairs_count = std::size_t{};

            const auto matched_pair_iterator =
                std::ranges::find_if(*this, [&pairs_count, range_start, range_end](CharT chr) {
                    pairs_count += static_cast<std::size_t>(chr == range_start);
                    pairs_count -= static_cast<std::size_t>(chr == range_end);
                    return pairs_count == 0;
                });

            if (matched_pair_iterator == end()) {
                return npos;
            }

            return distance(begin(), matched_pair_iterator);
        }

        ISL_DECL auto rfind(CharT chr, const std::size_t offset = 0) const noexcept -> std::size_t
        {
            if (offset >= length) {
                return npos;
            }

            const auto reverse_end = this->rend();

            const auto it_to_elem = std::ranges::find(
                this->rbegin() + static_cast<ptrdiff_t>(offset), reverse_end, chr);

            return it_to_elem == reverse_end ? npos : distance(it_to_elem, reverse_end) - 1;
        }

        ISL_DECL auto lstrip(BasicStringView characters_to_strip) const -> BasicStringView
        {
            auto stripped_string = *this;
            auto has_characters_to_strip = [&stripped_string, &characters_to_strip]() {
                const auto first_character = stripped_string.front();
                return characters_to_strip.contains(first_character);
            };

            while (!stripped_string.empty() && has_characters_to_strip()) {
                --stripped_string.length;
                ++stripped_string.string;
            }

            return stripped_string;
        }

        ISL_DECL auto rstrip(const BasicStringView characters_to_strip) const -> BasicStringView
        {
            auto stripped_string = *this;

            while (!stripped_string.empty() &&
                   characters_to_strip.contains(stripped_string.back())) {
                --stripped_string.length;
            }

            return stripped_string;
        }

        ISL_DECL auto strip(BasicStringView characters_to_strip) const -> BasicStringView
        {
            const auto left_stripped = lstrip(characters_to_strip);
            return left_stripped.rstrip(characters_to_strip);
        }

        ISL_DECL auto setLength(std::size_t new_length) const noexcept -> BasicStringView
        {
            auto new_string = *this;
            new_string.length = new_length;
            return new_string;
        }

        ISL_DECL auto startsWith(const StringLike<CharT> auto &str) const noexcept -> bool
        {
            return substr(0, str.size()).operator==(str);
        }

        ISL_DECL auto front() const -> CharT
        {
            if (empty()) {
                throw std::out_of_range{
                    "BasicStringView::front() failed, because string is empty."};
            }

            return *begin();
        }

        ISL_DECL auto back() const -> CharT
        {
            if (empty()) {
                throw std::out_of_range{"BasicStringView::back() failed, because string is empty."};
            }

            return *(end() - 1);
        }

        ISL_DECL auto operator[](const std::size_t index) const -> CharT
        {
            return at(index);
        }

        ISL_DECL auto at(std::size_t index) const -> CharT
        {
            if (index >= length) {
                throw std::out_of_range{
                    fmt::format("BasicStringView::at index {} is out of range", index)};
            }

            return string[index];
        }

        template<std::constructible_from<const CharT *, std::size_t> T>
        ISL_DECL explicit operator T() const
        {
            return {string, length};
        }

        ISL_DECL auto operator==(const BasicStringView &other) const noexcept -> bool
        {
            return this->operator==(static_cast<std::string_view>(other));
        }

        ISL_DECL auto operator==(const StringLike<CharT> auto &other) const noexcept -> bool
        {
            return std::equal(begin(), end(), std::begin(other), std::end(other));
        }

        ISL_DECL auto
            operator<=>(const BasicStringView &other) const noexcept -> std::strong_ordering
        {
            return this->operator<=>(as<std::string_view>(other));
        }

        ISL_DECL auto
            operator<=>(const StringLike<CharT> auto &other) const noexcept -> std::strong_ordering
        {
            auto min_size = std::min(size(), std::size(other));

            for (auto i = std::size_t{}; i != min_size; ++i) {
                const auto left_chr = at(i);
                const auto right_chr = other.at(i);

                if (left_chr != right_chr) {
                    return left_chr <=> right_chr;
                }
            }

            return size() <=> std::size(other);
        }

    private:
        template<typename T>
        ISL_DECL static auto distance(T first, T last) noexcept -> std::size_t
        {
            return as<std::size_t>(std::distance(first, last));
        }
    };

    namespace string_view_literals
    {
        [[nodiscard]] consteval auto
            operator""_sv(const char *string, std::size_t length) -> BasicStringView<char>
        {
            return {string, length};
        }

        [[nodiscard]] consteval auto
            operator""_sv(const char8_t *string, std::size_t length) -> BasicStringView<char8_t>
        {
            return {string, length};
        }

        [[nodiscard]] consteval auto
            operator""_sv(const char16_t *string, std::size_t length) -> BasicStringView<char16_t>
        {
            return {string, length};
        }

        [[nodiscard]] consteval auto
            operator""_sv(const char32_t *string, std::size_t length) -> BasicStringView<char32_t>
        {
            return {string, length};
        }

        [[nodiscard]] consteval auto
            operator""_sv(const wchar_t *string, std::size_t length) -> BasicStringView<wchar_t>
        {
            return {string, length};
        }
    }// namespace string_view_literals
}// namespace isl

template<isl::CharacterLiteral CharT>
struct ankerl::unordered_dense::hash<isl::BasicStringView<CharT>>
{
    using is_avalanching = void;

    [[nodiscard]] auto operator()(const isl::BasicStringView<CharT> &str) const noexcept -> auto
    {
        static_assert(std::has_unique_object_representations_v<CharT>);
        return detail::wyhash::hash(str.data(), sizeof(CharT) * str.size());
    }
};

template<>
struct fmt::formatter<isl::string_view> : formatter<std::string_view>
{
    auto format(const isl::string_view &str, format_context &ctx) const -> format_context::iterator
    {
        return formatter<std::string_view>::format(static_cast<std::string_view>(str), ctx);
    }
};

#endif /* ISL_PROJECT_STRING_VIEW_HPP*/

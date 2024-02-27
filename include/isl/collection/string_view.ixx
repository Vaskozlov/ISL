module;

#include <isl/core/defines.hpp>

export module isl.collection:string_view;

export import isl.core;

export namespace isl
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
        pointer string{nullptr};
        std::size_t length{0};

    public:
        BasicStringView() noexcept = default;

        template<std::size_t N>
        constexpr explicit BasicStringView(const std::array<CharT, N> &char_array) noexcept
          : string{char_array.data()}
          , length{char_array.size()}
        {}

        constexpr BasicStringView(pointer char_pointer, std::size_t string_length) noexcept
          : string{char_pointer}
          , length{string_length}
        {}

        constexpr BasicStringView(iterator first, iterator last) noexcept
          : string{first}
          , length{distance(first, last)}
        {}

        // NOLINTNEXTLINE
        constexpr BasicStringView(const CharacterArray auto &str) noexcept
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

        ISL_DECL auto substr(std::size_t first, std::size_t len) const noexcept -> BasicStringView
        {
            auto last = first + len;

            last = std::min(size(), last);
            first = std::min(size(), first);

            return {begin() + first, begin() + last};
        }

        ISL_DECL auto removeLastCharacter() const noexcept -> BasicStringView
        {
            auto truncated = *this;

            if (!empty()) {
                --truncated.length;
            }

            return truncated;
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto find(CharT chr, std::size_t offset = 0) const noexcept -> std::size_t
        {
            if (offset >= length) {
                return npos;
            }

            const auto it_to_elem = std::find(begin() + offset, end(), chr);
            return it_to_elem == end() ? npos : distance(begin(), it_to_elem);
        }

        ISL_SAFE_VERSION
        ISL_DECL auto find(CharT chr, std::size_t offset = 0) const noexcept -> std::optional<std::size_t>
        {
            auto result = find<FunctionAPI::UNSAFE>(chr, offset);

            if (result == npos) {
                return std::nullopt;
            }

            return result;
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto find(CharT chr, iterator from) const noexcept -> std::size_t
        {
            if (from >= end()) {
                return npos;
            }

            const auto it_to_elem = std::find(from, end(), chr);
            return it_to_elem == end() ? npos : distance(begin(), it_to_elem);
        }

        ISL_SAFE_VERSION
        ISL_DECL auto find(CharT chr, iterator from) const noexcept -> std::optional<std::size_t>
        {
            auto result = find<FunctionAPI::UNSAFE>(chr, from);

            if (result == npos) {
                return std::nullopt;
            }

            return result;
        }

        ISL_DECL auto contains(CharT chr) const noexcept -> bool
        {
            return find<FunctionAPI::SAFE>(chr).has_value();
        }

        /**
         * @brief returns index of the element, which closes range opened with starter
         * @param starter character, that starts range
         * @param ender character, that ends range
         * @return index on success or npos on failure
         */
        ISL_UNSAFE_VERSION
        ISL_DECL auto findMatchingPair(CharT starter, CharT ender) const noexcept -> std::size_t
        {
            auto pairs_count = as<std::size_t>(0);

            const auto matched_pair_iterator =
                std::ranges::find_if(*this, [&pairs_count, starter, ender](CharT chr) {
                    pairs_count += as<std::size_t>(chr == starter);
                    pairs_count -= as<std::size_t>(chr == ender);
                    return pairs_count == 0;
                });

            if (matched_pair_iterator == end()) {
                return npos;
            }

            return distance(begin(), matched_pair_iterator);
        }

        /**
         * @brief returns index of the element, which closes range opened with starter
         * @param starter character, that starts range
         * @param ender character, that ends range
         * @return index on success or std::nullopt failure
         */
        ISL_SAFE_VERSION
        ISL_DECL auto
            findMatchingPair(CharT starter, CharT ender) const noexcept -> std::optional<std::size_t>
        {
            auto result = findMatchingPair<FunctionAPI::UNSAFE>(starter, ender);

            if (result == npos) {
                return std::nullopt;
            }

            return result;
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto rfind(CharT chr, std::size_t offset = 0) const noexcept -> std::size_t
        {
            if (offset >= length) {
                return npos;
            }

            const auto it_to_elem = std::find(this->rbegin() + as<long>(offset), this->rend(), chr);
            return it_to_elem == this->rend() ? npos : distance(it_to_elem, this->rend()) - 1;
        }

        ISL_SAFE_VERSION
        ISL_DECL auto rfind(CharT chr, std::size_t offset = 0) const noexcept -> std::optional<std::size_t>
        {
            auto result = rfind<FunctionAPI::UNSAFE>(chr, offset);

            if (result == npos) {
                return std::nullopt;
            }

            return result;
        }

        ISL_DECL auto
            leftStrip(BasicStringView characters_to_strip) const noexcept -> BasicStringView
        {
            auto stripped_string = *this;
            auto has_characters_to_strip = [&stripped_string, &characters_to_strip]() {
                const auto first_character = stripped_string.template front<FunctionAPI::UNSAFE>();
                return characters_to_strip.contains(first_character);
            };

            while (!stripped_string.empty() && has_characters_to_strip()) {
                --stripped_string.length;
                ++stripped_string.string;
            }

            return stripped_string;
        }

        ISL_DECL auto
            rightStrip(BasicStringView characters_to_strip) const noexcept -> BasicStringView
        {
            auto stripped_string = *this;

            while (!stripped_string.empty() &&
                   characters_to_strip.contains(*stripped_string.back())) {
                --stripped_string.length;
            }

            return stripped_string;
        }

        ISL_DECL auto strip(BasicStringView characters_to_strip) const noexcept -> BasicStringView
        {
            const auto left_stripped = leftStrip(characters_to_strip);
            return left_stripped.rightStrip(characters_to_strip);
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto changeLength(std::size_t new_length) const noexcept -> BasicStringView
        {
            auto new_string = *this;
            new_string.length = new_length;
            return new_string;
        }

        ISL_SAFE_VERSION
        ISL_DECL auto changeLength(std::size_t new_length) const -> BasicStringView
        {
            if (new_length > length) {
                throw std::invalid_argument{"New length is greater than the old one"};
            }

            return changeLength<FunctionAPI::UNSAFE>(new_length);
        }

        ISL_DECL auto startsWith(const StringLike<CharT> auto &str) const noexcept -> bool
        {
            return substr(0, str.size()).operator==(str);
        }

        ISL_SAFE_VERSION
        ISL_DECL auto front() const noexcept -> std::optional<CharT>
        {
            if (empty()) {
                return std::nullopt;
            }

            return *begin();
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto front() const noexcept -> CharT
        {
            return *begin();
        }

        ISL_SAFE_VERSION
        ISL_DECL auto back() const noexcept -> std::optional<CharT>
        {
            if (empty()) {
                return std::nullopt;
            }

            return *(end() - 1);
        }

        ISL_UNSAFE_VERSION
        ISL_DECL auto back() const noexcept -> CharT
        {
            return *(end() - 1);
        }

        ISL_DECL auto operator[](std::size_t index) const -> CharT
        {
            return at(index);
        }

        ISL_DECL auto at(std::size_t index) const -> CharT
        {
            if (index >= length) {
                throw std::out_of_range("index out of range");
            }

            return string[index];
        }

        template<std::constructible_from<const CharT *, std::size_t> T>
        ISL_DECL explicit operator T() const
        {
            return {string, length};
        }

        ISL_DECL auto operator==(BasicStringView other) const noexcept -> bool
        {
            return this->operator==(as<std::string_view>(other));
        }

        ISL_DECL auto operator==(const StringLike<CharT> auto &other) const noexcept -> bool
        {
            return std::equal(begin(), end(), std::begin(other), std::end(other));
        }

        ISL_DECL auto operator<=>(BasicStringView other) const noexcept -> std::weak_ordering
        {
            return this->operator<=>(as<std::string_view>(other));
        }

        ISL_DECL auto
            operator<=>(const StringLike<CharT> auto &other) const noexcept -> std::weak_ordering
        {
            auto min_size = std::min(size(), std::size(other));

            for (auto i = as<std::size_t>(0); i != min_size; ++i) {
                if (this->operator[](i) != other[i]) {
                    return this->operator[](i) <=> other[i];
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

export template<>
struct std::formatter<isl::string_view> : public std::formatter<std::string_view>
{
    auto format(const isl::string_view &str, format_context &ctx) const -> decltype(ctx.out())
    {
        return formatter<std::string_view>::format(isl::as<std::string_view>(str), ctx);
    }
};

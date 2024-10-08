#ifndef ISL_PROJECT_FLATMAP_HPP
#define ISL_PROJECT_FLATMAP_HPP

#include <isl/isl.hpp>
#include <isl/iterator.hpp>

namespace isl
{
    template<typename Key, typename Value, std::size_t Size, typename Pred = std::equal_to<>>
    class StaticFlatmap
      : public AutoImplementedIteratorMethods<StaticFlatmap<Key, Value, Size, Pred>>
    {
    public:
        using key_type = Key;
        using mapped_type = Value;
        using value_type = Pair<Key, Value>;
        using storage_t = std::array<value_type, Size>;
        using iterator = typename storage_t::iterator;
        using const_iterator = typename storage_t::const_iterator;

    private:
        storage_t storage{};
        std::size_t occupied{};

    public:
        ISL_DECL auto size() const noexcept -> std::size_t
        {
            return occupied;
        }

        ISL_DECL static auto capacity() noexcept -> std::size_t
        {
            return Size;
        }

        ISL_DECL auto begin() noexcept -> iterator
        {
            return storage.begin();
        }

        ISL_DECL auto end() noexcept -> iterator
        {
            return storage.begin() + occupied;
        }

        ISL_DECL auto begin() const noexcept -> const_iterator
        {
            return storage.begin();
        }

        ISL_DECL auto end() const noexcept -> const_iterator
        {
            return storage.begin() + occupied;
        }

        template<typename K, typename... Ts>
        constexpr auto tryEmplace(K &&key, Ts &&...args) -> std::pair<iterator, bool>
            requires std::constructible_from<value_type, Key, Ts...>
        {
            if (occupied == capacity()) {
                throw std::out_of_range{"StaticFlatmap::tryEmplace() failed, map is full."};
            }

            auto result = find(key);

            if (result != end()) {
                return {result, false};
            }

            storage[occupied++] = value_type{std::forward<K>(key), std::forward<Ts>(args)...};
            return {result, true};
        }

        ISL_DECL auto at(const Key &key) ISL_LIFETIMEBOUND -> Value &
        {
            return at(*this, key);
        }

        ISL_DECL auto at(const Key &key) const ISL_LIFETIMEBOUND -> const Value &
        {
            return at(*this, key);
        }

        ISL_DECL auto operator[](const Key &key) ISL_LIFETIMEBOUND->Value &
        {
            return tryEmplace(key).first->second;
        }

        ISL_DECL auto operator[](const Key &key) const ISL_LIFETIMEBOUND->const Value &
        {
            return at(key);
        }

        ISL_DECL auto contains(const Key &key) const noexcept -> bool
        {
            return find(key) != end();
        }

        ISL_DECL auto find(const Key &key) noexcept -> iterator
        {
            return find(*this, key);
        }

        ISL_DECL auto find(const Key &key) const noexcept -> const_iterator
        {
            return find(*this, key);
        }

        StaticFlatmap() = default;

        constexpr StaticFlatmap(const std::initializer_list<value_type> &initial_data)
        {
            for (const value_type &value : initial_data) {
                if (occupied == capacity()) {
                    throw std::runtime_error{
                        "StaticFlatmap capacity limit reached during initialization"};
                }

                storage[occupied++] = value;
            }
        }

    private:
        template<typename Self>
        ISL_DECL static auto find(Self &self, const Key &key) noexcept -> auto
        {
            return std::ranges::find_if(self, [&key](const value_type &value) {
                return Pred{}(key, value.first);
            });
        }

        template<typename Self>
        ISL_DECL static auto at(Self &self, const Key &key) -> auto &
        {
            auto elem = self.find(key);

            if (elem == self.end()) {
                throw std::out_of_range{"Key not found in StaticFlatmap at() method."};
            }

            return elem->second;
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_FLATMAP_HPP */

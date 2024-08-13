#ifndef ISL_PROJECT_FLAT_MAP_HPP
#define ISL_PROJECT_FLAT_MAP_HPP

#include <initializer_list>
#include <isl/isl.hpp>

namespace isl
{
    template<typename Key, typename Value, class Container = std::vector<std::pair<Key, Value>>>
    class FlatMap
    {
    private:
        Container container;

    public:
        using value_type = Pair<Key, Value>;
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

        FlatMap() = default;

        FlatMap(const std::initializer_list<value_type> &initial_data)
        {
            for (const auto &elem : initial_data) {
                insert(elem);
            }
        }

        template<typename... Ts>
        auto tryEmplace(const Key &key, Ts &&...args) -> std::pair<iterator, bool>
        {
            auto it = std::ranges::find_if(container, [&key](const auto &elem) {
                return elem.first == key;
            });

            if (it == container.end()) {
                container.emplace_back(
                    std::piecewise_construct, std::tuple<Key>(key),
                    std::tuple<Ts...>(std::forward<Ts>(args)...));

                return {std::prev(container.end()), true};
            }

            return {it, false};
        }

        auto insert(const value_type &new_object) -> std::pair<iterator, bool>
        {
            return tryEmplace(new_object.first, new_object.second);
        }

        auto insert(value_type &&new_object) -> std::pair<iterator, bool>
        {
            return tryEmplace(std::move(new_object.first), std::move(new_object.second));
        }

        [[nodiscard]] auto contains(const Key &value) const -> bool
        {
            return std::ranges::find_if(container, value) != container.end();
        }

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return container.empty();
        }

        [[nodiscard]] auto size() const noexcept -> std::size_t
        {
            return container.size();
        }

        auto at(const Key &key) -> Value &
        {
            auto it = std::ranges::find_if(container, [&key](const auto &elem) {
                return elem.first == key;
            });

            if (it == container.end()) {
                throw std::runtime_error{"Element not found"};
            }

            return it->second;
        }

        auto at(const Key &key) const -> const Value &
        {
            auto it = std::ranges::find_if(container, [&key](const auto &elem) {
                return elem.first == key;
            });

            if (it == container.end()) {
                throw std::runtime_error{"Element not found"};
            }

            return it->second;
        }

        auto erase(const Key &value) -> void
        {
            std::erase_if(container, [&value](const auto &elem) {
                return elem.first == value;
            });
        }

        [[nodiscard]] auto begin() -> iterator
        {
            return container.begin();
        }

        [[nodiscard]] auto begin() const -> const_iterator
        {
            return container.begin();
        }

        [[nodiscard]] auto cbegin() const -> const_iterator
        {
            return begin();
        }

        [[nodiscard]] auto end() -> iterator
        {
            return container.end();
        }

        [[nodiscard]] auto end() const -> const_iterator
        {
            return container.end();
        }

        [[nodiscard]] auto cend() const -> const_iterator
        {
            return end();
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_FLAT_MAP_HPP */

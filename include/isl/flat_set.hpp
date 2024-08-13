#ifndef ISL_PROJECT_FLAT_SET_HPP
#define ISL_PROJECT_FLAT_SET_HPP

#include <initializer_list>
#include <isl/isl.hpp>

namespace isl
{
    template<typename T, class Container = std::vector<T>>
    class FlatSet
    {
    private:
        Container container;

    public:
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;

        FlatSet() = default;

        FlatSet(const std::initializer_list<T> &initial_data)
        {
            for (const auto &elem : initial_data) {
                tryEmplace(elem);
            }
        }

        template<typename... Ts>
        auto tryEmplace(Ts &&...args) -> std::pair<iterator, bool>
        {
            auto &new_object = container.emplace_back(std::forward<Ts>(args)...);
            auto it = std::ranges::find(container, new_object);

            if (it + 1 != container.end()) {
                container.pop_back();
                return {it, false};
            }

            return {it, true};
        }

        auto insert(const T &new_object) -> std::pair<iterator, bool>
        {
            return tryEmplace(new_object);
        }

        auto insert(T &&new_object) -> std::pair<iterator, bool>
        {
            return tryEmplace(std::move(new_object));
        }

        [[nodiscard]] auto contains(const T &value) const -> bool
        {
            return std::ranges::find(container, value) != container.end();
        }

        auto erase(const T &value) -> void
        {
            std::erase(container, value);
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

#endif /* ISL_PROJECT_FLAT_SET_HPP */

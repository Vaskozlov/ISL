#ifndef CCL_PROJECT_LAZY_HPP
#define CCL_PROJECT_LAZY_HPP

#include <functional>
#include <isl/isl.hpp>
#include <variant>

namespace isl
{
    template<typename T>
    class Lazy
    {
    private:
        mutable std::variant<std::function<T()>, T> storage;

    public:
        template<typename F>
            requires std::is_invocable_r_v<T, F>
        explicit Lazy(F &&func)
          : storage{std::in_place_type<std::function<T()>>, std::forward<F>(func)}
        {}

        template<typename... Ts>
        explicit Lazy(Ts &&...args)
          : storage{std::in_place_type<T>, std::forward<Ts>(args)...}
        {}

        ISL_DECL auto get() const ISL_LIFETIMEBOUND -> const T &
        {
            compute();
            return std::get<1>(storage);
        }

        ISL_DECL auto get() ISL_LIFETIMEBOUND -> T &
        {
            compute();
            return std::get<1>(storage);
        }

    private:
        constexpr auto compute() const -> void
        {
            if (storage.index() == 0) {
                storage.template emplace<T>(std::get<0>(storage)());
            }
        }
    };

    template<typename T>
    ISL_DECL auto toLazy(T &&value) -> Lazy<std::remove_cvref_t<T>>
    {
        return Lazy<T>{std::forward<T>(value)};
    }

    template<std::invocable Func>
    ISL_DECL auto toLazy(Func &&function) -> Lazy<decltype(function())>
    {
        return Lazy<decltype(function())>(std::forward<Func>(function));
    }
}// namespace isl

#endif /* CCL_PROJECT_LAZY_HPP */

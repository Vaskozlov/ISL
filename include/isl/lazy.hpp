#ifndef CCL_PROJECT_LAZY_HPP
#define CCL_PROJECT_LAZY_HPP

#include <functional>
#include <isl/isl.hpp>
#include <variant>

namespace isl
{
    template<typename T>
    concept LazyStorable = std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T> &&
                           std::is_trivially_copy_assignable_v<T>;

    template<LazyStorable T>
    class Lazy : public std::variant<std::function<T()>, T>
    {
    private:
        static constexpr bool NoexceptComputable = std::is_nothrow_invocable_v<std::function<T()>>;

    public:
        using std::variant<std::function<T()>, T>::index;
        using std::variant<std::function<T()>, T>::variant;

        ISL_DECL auto get() const noexcept(NoexceptComputable) -> T
        {
            if (this->index() == 0) {
                return std::get<0>(*this)();
            }

            return std::get<1>(*this);
        }

        ISL_DECL auto get() noexcept(NoexceptComputable) ISL_LIFETIMEBOUND -> T &
        {
            compute();
            return std::get<1>(*this);
        }

    private:
        constexpr auto compute() noexcept(NoexceptComputable) -> void
        {
            if (index() == 0) {
                *this = std::get<0>(*this)();
            }
        }
    };

    template<typename T>
    ISL_DECL auto toLazy(T &&value) -> Lazy<std::remove_cvref_t<T>>
        requires(LazyStorable<std::remove_cvref_t<T>>)
    {
        return Lazy<T>{std::forward<T>(value)};
    }

    template<std::invocable Func>
    ISL_DECL auto toLazy(Func &&function) -> Lazy<decltype(function())>
        requires(LazyStorable<std::invoke_result_t<Func>>)
    {
        return Lazy<decltype(function())>(std::forward<Func>(function));
    }
}// namespace isl

#endif /* CCL_PROJECT_LAZY_HPP */

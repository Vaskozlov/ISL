#ifndef ISL_PROJECT_ITERATOR_HPP
#define ISL_PROJECT_ITERATOR_HPP

#include <isl/detail/defines.hpp>
#include <iterator>

namespace isl
{
    template <typename T>
    class AutoImplementedIteratorMethods
    {
        ISL_DECL auto toParent() const noexcept ISL_LIFETIMEBOUND -> const T &
        {
            return static_cast<const T &>(*this);
        }

        ISL_DECL auto begin() const noexcept(noexcept(std::declval<const T>().begin()))
            -> decltype(auto)
        {
            return std::begin(toParent());
        }

        ISL_DECL auto end() const noexcept(noexcept(std::declval<const T>().end()))
            -> decltype(auto)
        {
            return std::end(toParent());
        }

    public:
        ISL_DECL auto cbegin() const noexcept(noexcept(begin())) -> decltype(auto)
        {
            return begin();
        }

        ISL_DECL auto cend() const noexcept(noexcept(end())) -> decltype(auto)
        {
            return end();
        }

        ISL_DECL auto rbegin() noexcept(noexcept(end())) -> decltype(auto)
        {
            return std::reverse_iterator{end()};
        }

        ISL_DECL auto rend() noexcept(noexcept(begin())) -> decltype(auto)
        {
            return std::reverse_iterator{begin()};
        }

        ISL_DECL auto rbegin() const noexcept(noexcept(end())) -> decltype(auto)
        {
            return std::reverse_iterator{end()};
        }

        ISL_DECL auto rend() const noexcept(noexcept(begin())) -> decltype(auto)
        {
            return std::reverse_iterator{begin()};
        }

        ISL_DECL auto crbegin() const noexcept(noexcept(rbegin())) -> decltype(auto)
        {
            return rbegin();
        }

        ISL_DECL auto crend() const noexcept(noexcept(rend())) -> decltype(auto)
        {
            return rend();
        }
    };
} // namespace isl

#endif /* ISL_PROJECT_ITERATOR_HPP */

#ifndef ISL_PROJECT_RANGE_HPP
#define ISL_PROJECT_RANGE_HPP

#include <isl/isl.hpp>
#include <isl/iterator.hpp>

namespace isl
{
    template<std::unsigned_integral T = size_t>
    class Range
      : public AutoIteratorMethods<Range<T>>
      , public detail::TrivialPair<T, T>
    {
    public:
        class iterator;

        using detail::TrivialPair<T, T>::TrivialPair;

        ISL_DECL auto getFrom() const noexcept -> T
        {
            return this->first;
        }

        ISL_DECL auto getTo() const noexcept -> T
        {
            return this->second;
        }

        ISL_DECL auto inRange(T value) const noexcept
        {
            return static_cast<bool>(value >= getFrom() & value <= getTo());
        }

        ISL_DECL auto begin() const noexcept -> iterator
        {
            return iterator{getFrom()};
        }

        ISL_DECL auto end() const noexcept -> iterator
        {
            return iterator{getTo()};
        }
    };

    template<std::unsigned_integral T>
    class Range<T>::iterator
    {
    private:
        T value;

    public:
        ISL_DECL explicit iterator(T t_value) noexcept
          : value{t_value}
        {}

        ISL_DECL auto operator*() const noexcept -> T
        {
            return value;
        }

        constexpr auto operator++() noexcept ISL_LIFETIMEBOUND->iterator &
        {
            ++value;
            return *this;
        }

        ISL_DECL auto operator++(int) noexcept ISL_LIFETIMEBOUND->iterator
        {
            auto old = *this;
            ++value;
            return old;
        }

        ISL_DECL auto operator<=>(const iterator &) const noexcept -> std::weak_ordering = default;
    };
}// namespace isl

#endif /* ISL_PROJECT_RANGE_HPP */

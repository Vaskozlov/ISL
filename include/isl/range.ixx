module;
#include <concepts>
#include <isl/detail/defines.hpp>
export module isl:range;

import :types;
import :pair;
import :iterator;
import :concepts;

export namespace isl {
    template<std::unsigned_integral T = size_t>
    class Range
            : public AutoImplementedIteratorMethods<Range<T>>
              , public detail::TrivialPair<T, T> {
    public:
        class iterator {
        private:
            T value;

        public:
            ISL_DECL explicit iterator(T t_value) noexcept
                : value{t_value} {
            }

            ISL_DECL auto operator*() const noexcept -> T {
                return value;
            }

            constexpr auto operator++() noexcept ISL_LIFETIMEBOUND -> iterator& {
                ++value;
                return *this;
            }

            ISL_DECL auto operator++(int) noexcept ISL_LIFETIMEBOUND -> iterator {
                auto old = *this;
                ++value;
                return old;
            }

            ISL_DECL auto operator<=>(const iterator&other) const noexcept = default;
        };

        using detail::TrivialPair<T, T>::TrivialPair;

        ISL_DECL auto getFrom() const noexcept -> T {
            return this->first;
        }

        ISL_DECL auto getTo() const noexcept -> T {
            return this->second;
        }

        ISL_DECL auto inRange(T value) const noexcept -> bool {
            return static_cast<bool>(value >= getFrom() & value <= getTo());
        }

        ISL_DECL auto begin() const noexcept -> iterator {
            return iterator{getFrom()};
        }

        ISL_DECL auto end() const noexcept -> iterator {
            return iterator{getTo()};
        }
    };
} // namespace isl

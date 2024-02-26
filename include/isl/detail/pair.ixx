module;
#include <isl/std.hpp>
#include <isl/detail/defines.hpp>

export module isl:pair;
import :concepts;

namespace isl::detail
{
    template<typename T1, typename T2>
    class ISL_TRIVIAL_ABI TrivialPair
    {
        static_assert(Trivial<T1> && Trivial<T2>);

    public:
        T1 first{};
        T2 second{};

        TrivialPair() = default;

        ISL_DECL explicit TrivialPair(T1 first_elem) noexcept
          : first{first_elem}
        {}

        ISL_DECL TrivialPair(T1 first_elem, T2 second_elem) noexcept
          : first{first_elem}
          , second{second_elem}
        {}

        ISL_DECL auto
            operator<=>(const TrivialPair &other) const noexcept -> std::weak_ordering = default;
    };
}// namespace isl::detail

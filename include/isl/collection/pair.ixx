module;
#include <isl/core/defines.hpp>


export module isl.collection:pair;
import isl.core;

export namespace isl::detail
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

        ISL_DECL auto operator<=>(const TrivialPair &other) const noexcept = default;
    };
}// namespace isl::detail

export namespace isl
{
    template<typename T1, typename T2>
    using Pair = std::conditional_t<
        Trivial<T1> && Trivial<T2>, detail::TrivialPair<T1, T2>, std::pair<T1, T2>>;
}// namespace isl

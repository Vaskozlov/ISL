#ifndef ISL_PROJECT_CRTP_HPP
#define ISL_PROJECT_CRTP_HPP

#include <isl/isl.hpp>

namespace isl
{
    template<typename CRTP>
    class CrtpFoundation
    {
    public:
        ISL_DECL auto toParent() noexcept ISL_LIFETIMEBOUND -> CRTP &
        {
            return static_cast<CRTP &>(*this);
        }

        ISL_DECL auto toParent() const noexcept ISL_LIFETIMEBOUND -> const CRTP &
        {
            return static_cast<const CRTP &>(*this);
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_CRTP_HPP */

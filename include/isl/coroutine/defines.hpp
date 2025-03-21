#ifndef ISL_PROJECT_COROUTINE_DEFINES_HPP
#define ISL_PROJECT_COROUTINE_DEFINES_HPP

#include <isl/isl.hpp>

#if __has_include(<coroutine>)
#    include <coroutine>
#    define ISL_EXPERIMENTAL_COROUTINE false
#else
#    include <experimental/coroutine>
#    define ISL_EXPERIMENTAL_COROUTINE true
#endif

namespace isl::coro
{
#if ISL_EXPERIMENTAL_COROUTINE
    using namespace std::experimental;
#else
    using namespace std;
#endif /* ISL_EXPERIMENTAL_COROUTINE */

    template <typename T>
    using ValueStorageType = std::conditional_t<std::is_trivial_v<T>, std::optional<T>, T *>;
} // namespace isl::coro

#endif /* ISL_PROJECT_COROUTINE_DEFINES_HPP */

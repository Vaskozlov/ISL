#ifndef ISL_DEFINES_HPP
#define ISL_DEFINES_HPP


#define ISL_DECL [[nodiscard]] constexpr

// NOLINTBEGIN

#define ISL_PERFECT_FORWARDING(Repr, Type)                                                         \
    template<typename Repr>                                                                        \
        requires std::is_convertible_v<Repr, Type>

#define ISL_PERFECT_FORWARDING_2(Repr1, Type1, Repr2, Type2)                                       \
    template<typename Repr1, typename Repr2>                                                       \
        requires std::is_convertible_v<Repr1, Type1> && std::is_convertible_v<Repr2, Type2>

#define ISL_PERFECT_FORWARDING_3(Repr1, Type1, Repr2, Type2, Repr3, Type3)                         \
    template<typename Repr1, typename Repr2, typename Repr3>                                       \
        requires std::is_convertible_v<Repr1, Type1> && std::is_convertible_v<Repr2, Type2> &&     \
                 std::is_convertible_v<Repr3, Type3>

// NOLINTEND

#define ISL_STRX(x) #x
#define ISL_STR(x) ISL_STRX(x)

#define ISL_ASSERT(x) assert(x)
#define ISL_ASSERT_MSG(x, message) assert((x) && message)

#if INTPTR_MAX == INT32_MAX
#    define ISL_64BIT false
#else
#    define ISL_64BIT true
#endif /* ISL_64BIT */

#if __cpp_if_consteval
#    define ISL_COMPILE_TIME_BRANCH consteval
#else
#    define ISL_COMPILE_TIME_BRANCH (std::is_constant_evaluated())
#endif /* ISL_COMPILE_TIME_BRANCH */

#if __cpp_if_consteval
#    define ISL_RUNTIME_BRANCH !consteval
#else
#    define ISL_RUNTIME_BRANCH (!std::is_constant_evaluated())
#endif /* ISL_RUNTIME_BRANCH */

#if defined(_MSC_VER)
#    define ISL_INLINE __forceinline
#else
#    define ISL_INLINE __attribute__((always_inline)) inline
#endif

#if defined(__clang__)
#    define ISL_TRIVIAL_ABI [[clang::trivial_abi]]
#else
#    define ISL_TRIVIAL_ABI
#endif /* ISL_TRIVIAL_ABI */

#if defined(__clang__)
#    define ISL_UNROLL_N(N) _Pragma(ISL_STR(unroll N))
#elif defined(__GNUC__)
#    define ISL_UNROLL_N(N) _Pragma(ISL_STR(GCC unroll N))
#else
#    define ISL_UNROLL_N(N)
#endif /* ISL_UNROLL_N */

#ifdef __has_builtin
#    if __has_builtin(__builtin_prefetch)
#        define ISL_PREFETCH(ADDR) __builtin_prefetch(ADDR)
#    else
#        define ISL_PREFETCH(ADDR)
#    endif
#else
#    define ISL_PREFETCH(ADDR)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#    define ISL_LIFETIMEBOUND [[msvc::lifetimebound]]
#elif defined(__clang__)
#    define ISL_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#    define ISL_LIFETIMEBOUND
#endif

#define ISL_NOEXCEPT_IF(EXPR) noexcept(noexcept(EXPR))

#if defined(__clang__) || defined(__GNUC__)
#    define ISL_PURE __attribute__((pure))
#else
#    define ISL_PURE
#endif

#define ISL_SAFE_VERSION                                                                           \
    template<FunctionAPI Mode = FunctionAPI::SAFE>                                                 \
        requires(Mode == FunctionAPI::SAFE)

#define ISL_UNSAFE_VERSION                                                                         \
    template<FunctionAPI Mode>                                                                     \
        requires(Mode == FunctionAPI::UNSAFE)

#define ISL_EXCEPTION(name, base_exception, classname)                                             \
    struct name : public base_exception                                                            \
    {                                                                                              \
        using base_exception::classname;                                                           \
    }

#endif /* ISL_DEFINES_HPP */

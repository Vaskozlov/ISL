#ifndef ISL_PROJECT_BIT_HPP
#define ISL_PROJECT_BIT_HPP

#include <array>
#include <isl/isl.hpp>

namespace isl::bit
{
    namespace msvc
    {
        ISL_DECL auto compileTimeBitScanForward(std::size_t value) -> std::size_t
        {
            auto bit_index = std::size_t{};
            constexpr auto mask = std::size_t{1};

            for (; value > 0; ++bit_index) {
                if ((value & mask) == mask) {
                    return bit_index;
                }
                value >>= 1;
            }

            return sizeof(std::size_t) * 8;
        }

#ifdef _MSC_VER
        ISL_DECL auto runtimeBitScanForward(std::size_t value) -> std::size_t
        {
            unsigned long bit_index;

#    if defined(_WIN32) && !defined(_WIN64)
            _BitScanForward(&bit_index, value);
#    else
            _BitScanForward64(&bit_index, value);
#    endif
            return bit_index;
        }
#endif /* _MSC_VER */
    }// namespace msvc

    ISL_DECL auto bitScanForward(std::size_t value) -> std::size_t
    {
#if defined(__has_builtin) && __has_builtin(__builtin_ctzl)
        return static_cast<std::size_t>(__builtin_ctzl(value));

#elif defined(_MSC_VER)
        if (std::is_constant_evaluated()) {
            return msvc::compileTimeBitScanForward(value);
        } else {
            return msvc::runtimeBitScanForward(value);
        }
#else
        return msvc::compileTimeBitScanForward(value);
#endif
    }

    template<std::size_t N>
        requires(N % (sizeof(std::size_t) * 8) == 0)
    class BitSet
    {
    private:
        constexpr static auto registerSize = sizeof(std::size_t) * 8;

        std::array<std::size_t, N / registerSize> bits{};

    public:
        constexpr auto set(std::size_t index) -> void
        {
            bits[index / registerSize] |= 1ULL << (index % registerSize);
        }

        constexpr auto reset(std::size_t index) -> void
        {
            bits[index / registerSize] &= ~(1ULL << (index % registerSize));
        }

        constexpr auto clear() -> void
        {
            bits.fill(0);
        }

        ISL_INLINE auto findAndSet(std::size_t from = 0) -> std::size_t
        {
            ISL_UNROLL_N(4)
            for (std::size_t i = 0; i != bits.size(); ++i) {
                auto &number = bits[i];

                if (number != std::numeric_limits<std::size_t>::max()) {
                    auto index = bitScanForward(~number);
                    number |= 1ULL << index;
                    return index + i * registerSize;
                }
            }

            return std::numeric_limits<std::size_t>::max();
        }

        ISL_DECL auto size() const noexcept -> std::size_t
        {
            return N;
        }

        ISL_DECL auto test(std::size_t index) const -> bool
        {
            return (bits[index / registerSize] & (1ULL << (index % registerSize))) != 0;
        }

        ISL_DECL auto operator[](std::size_t index) -> bool
        {
            return test(index);
        }

        ISL_DECL auto operator[](std::size_t index) const -> bool
        {
            return test(index);
        }
    };
}// namespace isl::bit
#endif /* ISL_PROJECT_BIT_HPP */

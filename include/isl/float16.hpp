#ifndef ISL_FLOAT16_HPP
#define ISL_FLOAT16_HPP

#include <bit>
#include <cinttypes>
#include <cstddef>

namespace isl
{
    class float16
    {
    private:
        friend auto fp32ToFp16(float) -> float16;
        friend auto fp16ToFp32(float16) -> float;

        std::uint16_t data;

        explicit float16(const std::uint16_t d)
          : data{d}
        {}

    public:
        explicit float16(float value);

        explicit operator float() const;
    };
} // namespace isl

#if defined(__AVX2__) && defined(__F16C__)
#    include <immintrin.h>

namespace isl
{
    inline float16 fp32ToFp16(float value)
    {
        auto vec32 = _mm256_set1_ps(value);
        auto vec16 = _mm256_cvtps_ph(vec32, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
        return float16{static_cast<std::uint16_t>(_mm_extract_epi16(vec16, 0))};
    }

    inline float fp16ToFp32(float16 value)
    {
        auto vec16 = _mm_set1_epi16(value.data);
        return _mm_cvtss_f32(_mm_cvtph_ps(vec16));
    }
} // namespace isl

// namespace isl
#elif defined(__ARM_NEON) && defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
#    include <arm_neon.h>

namespace isl
{
    inline auto fp32ToFp16(const float value) -> float16
    {
        const auto vec32 = vdupq_n_f32(value);
        const auto vec16 = vcvt_f16_f32(vec32);
        const auto result = vget_lane_f16(vec16, 0);

        return float16{std::bit_cast<std::uint16_t>(result)};
    }

    inline auto fp16ToFp32(const float16 value) -> float
    {
        const auto vec16 = vdup_n_f16(std::bit_cast<float16_t>(value.data));
        const auto vec32 = vcvt_f32_f16(vec16);
        return vgetq_lane_f32(vec32, 0);
    }
} // namespace isl
#else
namespace isl
{
    namespace detail
    {
        inline float overflow()
        {
            volatile float f = 1e10;

            for (std::size_t i = 0; i < 10; ++i) {
                f = f * f; // this will overflow before the for loop terminates
            }

            return f;
        }
    } // namespace detail

    inline float fp16ToFp32(const float16 value)
    {
        std::int32_t s = (value.data >> 15) & 0x00000001;
        std::int32_t e = (value.data >> 10) & 0x0000001f;
        std::int32_t m = value.data & 0x000003ff;

        if (e == 0) {
            if (m == 0) {
                //
                // Plus or minus zero
                //

                return std::bit_cast<float>(static_cast<std::uint32_t>(s << 31));
            } else {
                //
                // Denormalized number -- renormalize it
                //

                while (!(m & 0x00000400)) {
                    m <<= 1;
                    e -= 1;
                }

                e += 1;
                m &= ~0x00000400;
            }
        } else if (e == 31) {
            if (m == 0) {
                //
                // Positive or negative infinity
                //

                return std::bit_cast<float>(static_cast<uint32_t>((s << 31) | 0x7f800000));
            } else {
                //
                // Nan -- preserve sign and significand bits
                //

                return std::bit_cast<float>(
                    static_cast<std::uint32_t>((s << 31) | 0x7f800000 | (m << 13)));
            }
        }

        //
        // Normalized number
        //

        e = e + (127 - 15);
        m = m << 13;

        //
        // Assemble s, e and m.
        //

        return std::bit_cast<float>(static_cast<std::uint32_t>((s << 31) | (e << 23) | m));
    }

    inline float16 fp32ToFp16(const float value)
    {
        const std::int32_t i = std::bit_cast<std::int32_t>(value);

        //
        // Our floating point number, f, is represented by the bit
        // pattern in integer i.  Disassemble that bit pattern into
        // the sign, s, the exponent, e, and the significand, m.
        // Shift s into the position where it will go in the
        // resulting half number.
        // Adjust e, accounting for the different exponent bias
        // of float and half (127 versus 15).
        //

        std::int32_t s = (i >> 16) & 0x00008000;
        std::int32_t e = ((i >> 23) & 0x000000ff) - (127 - 15);
        std::int32_t m = i & 0x007fffff;

        //
        // Now reassemble s, e and m into a half:
        //

        if (e <= 0) {
            if (e < -10) {
                //
                // E is less than -10.  The absolute value of f is
                // less than half_MIN (f may be a small normalized
                // float, a denormalized float or a zero).
                //
                // We convert f to a half zero.
                //

                return float16(static_cast<std::uint16_t>(s));
            }

            //
            // E is between -10 and 0.  F is a normalized float,
            // whose magnitude is less than __half_NRM_MIN.
            //
            // We convert f to a denormalized half.
            //

            m = (m | 0x00800000) >> (1 - e);

            //
            // Round to nearest, round "0.5" up.
            //
            // Rounding may cause the significand to overflow and make
            // our number normalized.  Because of the way a half's bits
            // are laid out, we don't have to treat this case separately;
            // the code below will handle it correctly.
            //

            if (m & 0x00001000) {
                m += 0x00002000;
            }

            //
            // Assemble the half from s, e (zero) and m.
            //

            return float16(static_cast<std::uint16_t>(s | (m >> 13)));
        } else if (e == 0xff - (127 - 15)) {
            if (m == 0) {
                //
                // F is an infinity; convert f to a half
                // infinity with the same sign as f.
                //

                return float16(static_cast<std::uint16_t>(s | 0x7c00));
            } else {
                //
                // F is a NAN; we produce a half NAN that preserves
                // the sign bit and the 10 leftmost bits of the
                // significand of f, with one exception: If the 10
                // leftmost bits are all zero, the NAN would turn
                // into an infinity, so we have to set at least one
                // bit in the significand.
                //

                m >>= 13;

                return float16(static_cast<std::uint16_t>(s | 0x7c00 | m | (m == 0)));
            }
        } else {
            //
            // E is greater than zero.  F is a normalized float.
            // We try to convert f to a normalized half.
            //

            //
            // Round to nearest, round "0.5" up
            //

            if (m & 0x00001000) {
                m += 0x00002000;

                if (m & 0x00800000) {
                    m = 0;  // overflow in significand,
                    e += 1; // adjust exponent
                }
            }

            //
            // Handle exponent overflow
            //

            if (e > 30) {
                detail::overflow(); // Cause a hardware floating point overflow;

                return float16(static_cast<std::uint16_t>(s | 0x7c00));
                // if this returns, the half becomes an
            } // infinity with the same sign as f.

            //
            // Assemble the half from s, e and m.
            //

            return float16(static_cast<std::uint16_t>(s | (e << 10) | (m >> 13)));
        }
    }
} // namespace isl
#endif

namespace isl
{
    inline float16::float16(const float value)
      : data{fp32ToFp16(value).data}
    {}

    inline float16::operator float() const
    {
        return fp16ToFp32(*this);
    }
} // namespace isl

#endif /* ISL_FLOAT16_HPP */

#pragma once
#include "types.h"

namespace st {

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // type conversion free functions
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // float1 -> int1: numeric truncation (3.7f -> 3), same as C (int)f
    template<typename B> inline int1_t<B>   to_int1(const float1_t<B>& v) { return B::cvtps_epi32(v.x); }

    // int1 -> float1: numeric conversion (3 -> 3.0f), same as C (float)i
    template<typename B> inline float1_t<B> to_float1(const int1_t<B>& v) { return B::cvtepi32_ps(v.x); }

    // to_float1 from wider types --- takes x, drops the rest
    template<typename B> inline float1_t<B> to_float1(const float2_t<B>& v) { return v.x; }
    template<typename B> inline float1_t<B> to_float1(const float3_t<B>& v) { return v.x; }
    template<typename B> inline float1_t<B> to_float1(const float4_t<B>& v) { return v.x; }

    // pointer overloads --- load B::width floats/ints from memory
    template<typename B> inline float1_t<B> to_float1(const float* p) {
        if constexpr (B::width == 4) return _mm_loadu_ps(p);
        else                         return _mm256_loadu_ps(p);
    }
    template<typename B> inline int1_t<B> to_int1(const int* p) {
        if constexpr (B::width == 4) return _mm_loadu_si128((const __m128i*)p);
        else                         return _mm256_loadu_si256((const __m256i*)p);
    }

    // active backend convenience --- no template arg needed
    inline float1 to_float1(const float* p) { return to_float1<backend::active>(p); }
    inline int1   to_int1(const int* p) { return to_int1<backend::active>(p); }

    // bitcast variants: reinterpret raw bits, no numeric conversion
    template<typename B> inline int1_t<B>   to_int1_bits(const float1_t<B>& v) { return B::castps_si(v.x); }
    template<typename B> inline float1_t<B> to_float1_bits(const int1_t<B>& v) { return B::castsi_ps(v.x); }


    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float4x4 multiply
    //
    //   st::mul(mat, vec4)        --- transform one SIMD float4  (8 vec4s at once on AVX)
    //   st::mul(mat, scalar_vec4) --- transform a single scalar float4, returns scalar float4
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // float4x4 * float4_t<B>
    // Each component of the result is:  dot(row[r], vec)
    // We broadcast each vec component and accumulate columns.
    // This processes B::width independent vec4s simultaneously.
    template<typename B>
    inline float4_t<B> mul(const float4x4& m, const float4_t<B>& v) {
        // result = col0*v.x + col1*v.y + col2*v.z + col3*v.w
        float1_t<B> rx(m.col[0][0]), ry(m.col[0][1]), rz(m.col[0][2]), rw(m.col[0][3]);
        rx = rx * v.x;  ry = ry * v.x;  rz = rz * v.x;  rw = rw * v.x;

        rx = rx + float1_t<B>(m.col[1][0]) * v.y;
        ry = ry + float1_t<B>(m.col[1][1]) * v.y;
        rz = rz + float1_t<B>(m.col[1][2]) * v.y;
        rw = rw + float1_t<B>(m.col[1][3]) * v.y;

        rx = rx + float1_t<B>(m.col[2][0]) * v.z;
        ry = ry + float1_t<B>(m.col[2][1]) * v.z;
        rz = rz + float1_t<B>(m.col[2][2]) * v.z;
        rw = rw + float1_t<B>(m.col[2][3]) * v.z;

        rx = rx + float1_t<B>(m.col[3][0]) * v.w;
        ry = ry + float1_t<B>(m.col[3][1]) * v.w;
        rz = rz + float1_t<B>(m.col[3][2]) * v.w;
        rw = rw + float1_t<B>(m.col[3][3]) * v.w;

        return { rx, ry, rz, rw };
    }

    // const float* overloads --- pass glm::value_ptr(mat) directly, no float4x4 construction
    template<typename B>
    inline float4_t<B> mul(const float* m, const float4_t<B>& v) {
        return mul(float4x4(m), v);
    }

    // float4x4 * float1_t  --- scale every element of the matrix by a SIMD scalar
    // returns float4x4 (one result matrix per lane would require float4x4_t<B>, so we
    // reduce to a plain float4x4 using lane 0, or user can call per-lane explicitly)
    // Most common use: st::mul(m, 2.0f) style broadcast scale
    template<typename B>
    inline float4x4 mul(const float4x4& m, const float1_t<B>& f) {
        // extract lane 0 as a scalar and scale
        float s = f[0];
        float4x4 out;
        for (int i = 0; i < 16; ++i)
            (&out.col[0][0])[i] = (&m.col[0][0])[i] * s;
        return out;
    }
    template<typename B>
    inline float4x4 mul(const float1_t<B>& f, const float4x4& m) { return mul(m, f); }

    // float4x4 * float3_t  --- two variants:
    //   mul_point(m, v)     treats v as a point,     implicitly w=1  (applies translation)
    //   mul_dir(m, v)       treats v as a direction, implicitly w=0  (ignores translation)
    //   mul(m, v)           defaults to point transform

    template<typename B>
    inline float4_t<B> mul_point(const float4x4& m, const float3_t<B>& v) {
        // w=1: result = col0*x + col1*y + col2*z + col3
        float1_t<B> rx(m.col[0][0]), ry(m.col[0][1]), rz(m.col[0][2]), rw(m.col[0][3]);
        rx = rx * v.x;  ry = ry * v.x;  rz = rz * v.x;  rw = rw * v.x;

        rx = rx + float1_t<B>(m.col[1][0]) * v.y;
        ry = ry + float1_t<B>(m.col[1][1]) * v.y;
        rz = rz + float1_t<B>(m.col[1][2]) * v.y;
        rw = rw + float1_t<B>(m.col[1][3]) * v.y;

        rx = rx + float1_t<B>(m.col[2][0]) * v.z;
        ry = ry + float1_t<B>(m.col[2][1]) * v.z;
        rz = rz + float1_t<B>(m.col[2][2]) * v.z;
        rw = rw + float1_t<B>(m.col[2][3]) * v.z;

        rx = rx + float1_t<B>(m.col[3][0]);  // + col3 * 1
        ry = ry + float1_t<B>(m.col[3][1]);
        rz = rz + float1_t<B>(m.col[3][2]);
        rw = rw + float1_t<B>(m.col[3][3]);

        return { rx, ry, rz, rw };
    }

    template<typename B>
    inline float3_t<B> mul_dir(const float4x4& m, const float3_t<B>& v) {
        // w=0: result = col0*x + col1*y + col2*z  (translation column ignored)
        float1_t<B> rx(m.col[0][0]), ry(m.col[0][1]), rz(m.col[0][2]);
        rx = rx * v.x;  ry = ry * v.x;  rz = rz * v.x;

        rx = rx + float1_t<B>(m.col[1][0]) * v.y;
        ry = ry + float1_t<B>(m.col[1][1]) * v.y;
        rz = rz + float1_t<B>(m.col[1][2]) * v.y;

        rx = rx + float1_t<B>(m.col[2][0]) * v.z;
        ry = ry + float1_t<B>(m.col[2][1]) * v.z;
        rz = rz + float1_t<B>(m.col[2][2]) * v.z;

        return { rx, ry, rz };
    }

    // mul(m, float3) -> float4  (point transform, w computed from full matrix)
    template<typename B>
    inline float4_t<B> mul(const float4x4& m, const float3_t<B>& v) {
        return mul_point(m, v);
    }
    template<typename B>
    inline float4_t<B> mul(const float* m, const float3_t<B>& v) {
        return mul_point(float4x4(m), v);
    }
    inline float4x4::scalar_float4 mul(const float4x4& m, const float4x4::scalar_float4& v) {
        return {
            m.col[0][0] * v.x + m.col[1][0] * v.y + m.col[2][0] * v.z + m.col[3][0] * v.w,
            m.col[0][1] * v.x + m.col[1][1] * v.y + m.col[2][1] * v.z + m.col[3][1] * v.w,
            m.col[0][2] * v.x + m.col[1][2] * v.y + m.col[2][2] * v.z + m.col[3][2] * v.w,
            m.col[0][3] * v.x + m.col[1][3] * v.y + m.col[2][3] * v.z + m.col[3][3] * v.w,
        };
    }
    inline float4x4::scalar_float4 mul(const float* m, const float4x4::scalar_float4& v) {
        return mul(float4x4(m), v);
    }

    // float4x4 * float4x4
    inline float4x4 mul(const float4x4& a, const float4x4& b) {
        float4x4 out;
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r) {
                float s = 0.f;
                for (int k = 0; k < 4; ++k)
                    s += a.col[k][r] * b.col[c][k];
                out.col[c][r] = s;
            }
        return out;
    }
    inline float4x4 mul(const float* a, const float* b) { return mul(float4x4(a), float4x4(b)); }
    inline float4x4 mul(const float4x4& a, const float* b) { return mul(a, float4x4(b)); }
    inline float4x4 mul(const float* a, const float4x4& b) { return mul(float4x4(a), b); }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // eval() --- condition --- raw mask register
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // tag_mask: wraps a raw float1/int1 register as a condition; eval just unwraps it.
    // This is produced by (condition & float1) / (float1 & condition) operators,
    // allowing ~float1 mask values to participate in lazy condition chains correctly.
    template<typename B>
    inline auto eval(const condition<tag_mask, float1_t<B>, void>& c) { return c.lhs.x; }
    template<typename B>
    inline auto eval(const condition<tag_mask, int1_t<B>, void>& c) { return c.lhs.x; }

    // float1_t / int1_t passthrough --- needed so condition<tag_not, float1_t<B>, void>
    // can recurse: eval(tag_not{lhs}) calls eval(lhs) which must match float1_t.
    template<typename B> inline auto eval(const float1_t<B>& v) { return v.x; }
    template<typename B> inline auto eval(const int1_t<B>& v) { return v.x; }

    template<typename B>
    inline auto eval(const condition<tag_greater, float1_t<B>, float1_t<B>>& c) { return B::cmpgt_ps(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_less, float1_t<B>, float1_t<B>>& c) { return B::cmplt_ps(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_equal, float1_t<B>, float1_t<B>>& c) { return B::cmpeq_ps(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_not_equal, float1_t<B>, float1_t<B>>& c) { return B::cmpne_ps(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_greater_equal, float1_t<B>, float1_t<B>>& c) { return B::cmpge_ps(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_less_equal, float1_t<B>, float1_t<B>>& c) { return B::cmple_ps(c.lhs.x, c.rhs.x); }

    template<typename B>
    inline auto eval(const condition<tag_greater, int1_t<B>, int1_t<B>>& c) { return B::cmpgt_epi32(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_less, int1_t<B>, int1_t<B>>& c) { return B::cmpgt_epi32(c.rhs.x, c.lhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_equal, int1_t<B>, int1_t<B>>& c) { return B::cmpeq_epi32(c.lhs.x, c.rhs.x); }
    template<typename B>
    inline auto eval(const condition<tag_not_equal, int1_t<B>, int1_t<B>>& c) {
        return B::xor_si(B::cmpeq_epi32(c.lhs.x, c.rhs.x), B::set1_epi32(-1));
    }
    template<typename B>
    inline auto eval(const condition<tag_greater_equal, int1_t<B>, int1_t<B>>& c) {
        return B::or_si(B::cmpgt_epi32(c.lhs.x, c.rhs.x), B::cmpeq_epi32(c.lhs.x, c.rhs.x));
    }
    template<typename B>
    inline auto eval(const condition<tag_less_equal, int1_t<B>, int1_t<B>>& c) {
        return B::or_si(B::cmpgt_epi32(c.rhs.x, c.lhs.x), B::cmpeq_epi32(c.lhs.x, c.rhs.x));
    }

    // compound conditions
    // Generic compound eval --- handles any nesting depth and any lhs/rhs condition types.
    // The explicit-tag overloads below are replaced by these two; the and_ps/or_ps dispatch
    // is on the result type of eval(lhs) which is always a raw register (__m128/__m256/i).
    namespace detail {
        inline auto mask_and(auto lm, auto rm) {
            if constexpr (std::is_same_v<decltype(lm), __m128i>)  return _mm_and_si128(lm, rm);
            else if constexpr (std::is_same_v<decltype(lm), __m256i>) return _mm256_and_si256(lm, rm);
            else if constexpr (std::is_same_v<decltype(lm), __m256>)  return _mm256_and_ps(lm, rm);
            else                                                        return _mm_and_ps(lm, rm);
        }
        inline auto mask_or(auto lm, auto rm) {
            if constexpr (std::is_same_v<decltype(lm), __m128i>)  return _mm_or_si128(lm, rm);
            else if constexpr (std::is_same_v<decltype(lm), __m256i>) return _mm256_or_si256(lm, rm);
            else if constexpr (std::is_same_v<decltype(lm), __m256>)  return _mm256_or_ps(lm, rm);
            else                                                        return _mm_or_ps(lm, rm);
        }
        inline auto mask_xor_ones(auto m) {
            if constexpr (std::is_same_v<decltype(m), __m128i>)  return _mm_xor_si128(m, _mm_set1_epi32(-1));
            else if constexpr (std::is_same_v<decltype(m), __m256i>) return _mm256_xor_si256(m, _mm256_set1_epi32(-1));
            else if constexpr (std::is_same_v<decltype(m), __m256>)  return _mm256_xor_ps(m, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
            else                                                        return _mm_xor_ps(m, _mm_castsi128_ps(_mm_set1_epi32(-1)));
        }
    }

    template<typename LA, typename RA>
    inline auto eval(const condition<tag_and, LA, RA>& c) {
        return detail::mask_and(eval(c.lhs), eval(c.rhs));
    }
    template<typename LA, typename RA>
    inline auto eval(const condition<tag_or, LA, RA>& c) {
        return detail::mask_or(eval(c.lhs), eval(c.rhs));
    }
    template<typename LA>
    inline auto eval(const condition<tag_not, LA, void>& c) {
        return detail::mask_xor_ones(eval(c.lhs));
    }

    // ------ condition conversions ------------------------------------------------------------------------------------------------------------------------------------------------
    namespace detail {
        template<typename C, typename B>
        inline int1_t<B> cond_to_int1(const C& c) {
            auto m = eval(c);
            if constexpr (std::is_same_v<decltype(m), typename B::int_reg>)
                return int1_t<B>(m);
            else
                return int1_t<B>(B::castps_si(m));
        }
        template<typename C, typename B>
        inline float1_t<B> cond_to_float1(const C& c) {
            auto m = eval(c);
            if constexpr (std::is_same_v<decltype(m), typename B::float_reg>)
                return float1_t<B>(m);
            else
                return float1_t<B>(B::castsi_ps(m));
        }
        // raw-register variants used by float1_t/int1_t constructors (B known from struct)
        template<typename B, typename C>
        inline typename B::float_reg cond_to_float1_reg(const C& c) {
            auto m = eval(c);
            if constexpr (std::is_same_v<decltype(m), typename B::float_reg>) return m;
            else                                                                return B::castsi_ps(m);
        }
        template<typename B, typename C>
        inline typename B::int_reg cond_to_int1_reg(const C& c) {
            auto m = eval(c);
            if constexpr (std::is_same_v<decltype(m), typename B::int_reg>) return m;
            else                                                              return B::castps_si(m);
        }

        inline int any_movemask(__m128  m) { return _mm_movemask_ps(m); }
        inline int any_movemask(__m128i m) { return _mm_movemask_ps(_mm_castsi128_ps(m)); }
        inline int any_movemask(__m256  m) { return _mm256_movemask_ps(m); }
        inline int any_movemask(__m256i m) { return _mm256_movemask_ps(_mm256_castsi256_ps(m)); }

        template<typename C>
        inline bool cond_to_bool(const C& c) {
            return any_movemask(eval(c)) != 0;
        }
    }

    // condition<> --- bool  (deferred implementation)
    template<typename tag, typename lhs_t, typename rhs_t>
    condition<tag, lhs_t, rhs_t>::operator bool() const {
        return detail::cond_to_bool(*this);
    }
    template<typename tag, typename lhs_t>
    condition<tag, lhs_t, void>::operator bool() const {
        return detail::cond_to_bool(*this);
    }

    // mask() --- eval a condition to a float1 mask.
    // Equivalent to: st::float1 m = (a > b);  (which uses the float1_t condition constructor)
    // but explicit, and works when the active backend isn't known from context.
    template<typename tag, typename L, typename R>
    inline float1_t<backend::active> mask(const condition<tag, L, R>& c) {
        auto m = eval(c);
        if constexpr (std::is_same_v<decltype(m), backend::active::float_reg>) return m;
        else return backend::active::castsi_ps(m);
    }
    template<typename tag, typename L>
    inline float1_t<backend::active> mask(const condition<tag, L, void>& c) {
        auto m = eval(c);
        if constexpr (std::is_same_v<decltype(m), backend::active::float_reg>) return m;
        else return backend::active::castsi_ps(m);
    }

    // ------ movemask helper ------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline int movemask(const float1_t<B>& v) { return B::movemask_ps(v.x); }
    template<typename B> inline int movemask(const int1_t<B>& v) { return B::movemask_ps(B::castsi_ps(v.x)); }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // all / any / none
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Low-level helpers: operate on raw register values directly.
    // All overloads are explicit --- no if constexpr / decltype needed.
    namespace detail {

        // to_movemask: convert any mask register to an int bitmask
        inline int to_movemask(__m128  m) { return _mm_movemask_ps(m); }
        inline int to_movemask(__m128i m) { return _mm_movemask_ps(_mm_castsi128_ps(m)); }
        inline int to_movemask(__m256  m) { return _mm256_movemask_ps(m); }
        inline int to_movemask(__m256i m) { return _mm256_movemask_ps(_mm256_castsi256_ps(m)); }

        // full_mask: all lanes set
        inline int full_mask_of(__m128) { return backend::sse::full_mask; }
        inline int full_mask_of(__m128i) { return backend::sse::full_mask; }
        inline int full_mask_of(__m256) { return backend::avx256::full_mask; }
        inline int full_mask_of(__m256i) { return backend::avx256::full_mask; }

        // width: lane count
        inline int width_of(__m128) { return backend::sse::width; }
        inline int width_of(__m128i) { return backend::sse::width; }
        inline int width_of(__m256) { return backend::avx256::width; }
        inline int width_of(__m256i) { return backend::avx256::width; }

    } // namespace detail

    // all/none/any accept either a condition<> or a raw int1_t/float1_t mask.
    // We add explicit overloads for int1_t and float1_t so st::any(b8, f) compiles.

    // --- condition<> overloads ---
    template<typename tag, typename L, typename R>
    inline bool all(const condition<tag, L, R>& c) {
        auto m = eval(c);
        return detail::to_movemask(m) == detail::full_mask_of(m);
    }
    template<typename tag, typename L>
    inline bool all(const condition<tag, L, void>& c) {
        auto m = eval(c);
        return detail::to_movemask(m) == detail::full_mask_of(m);
    }

    template<typename tag, typename L, typename R>
    inline bool none(const condition<tag, L, R>& c) {
        return detail::to_movemask(eval(c)) == 0;
    }
    template<typename tag, typename L>
    inline bool none(const condition<tag, L, void>& c) {
        return detail::to_movemask(eval(c)) == 0;
    }

    template<typename tag, typename L, typename R, typename Func>
    inline void any(const condition<tag, L, R>& c, Func&& f) {
        auto m = eval(c);
        int  mask = detail::to_movemask(m);
        int  w = detail::width_of(m);
        for (int lane = 0; lane < w; ++lane)
            if (mask & (1 << lane)) f(lane);
    }
    template<typename tag, typename L, typename Func>
    inline void any(const condition<tag, L, void>& c, Func&& f) {
        auto m = eval(c);
        int  mask = detail::to_movemask(m);
        int  w = detail::width_of(m);
        for (int lane = 0; lane < w; ++lane)
            if (mask & (1 << lane)) f(lane);
    }

    // --- raw int1_t / float1_t overloads ---
    // movemask only reads the sign bit, so positive values like 1,2,3 look "false".
    // We use vptest (testz) where possible (1 instruction), and cmpne for per-lane work.
    //
    //   none()  --- testz(v,v): 1 instruction, returns true if ALL bits are zero
    //   all()   --- cmpne + movemask: 2 instructions, checks each lane individually
    //   any()   --- cmpne + movemask + loop: need per-lane info so testz is not enough

    namespace detail {
        // none_impl: single vptest instruction
        template<typename B> inline bool none_impl(typename B::int_reg v) {
            if constexpr (B::width == 4) return _mm_testz_si128(v, v) != 0;
            else                         return _mm256_testz_si256(v, v) != 0;
        }
        // nonzero_mask_int: per-lane all-ones if lane != 0, all-zeros if lane == 0
        template<typename B> inline typename B::int_reg nonzero_mask_int(typename B::int_reg v) {
            return B::or_si(B::cmpgt_epi32(v, B::setzero_si()),   // positive lanes
                B::cmpgt_epi32(B::setzero_si(), v));  // negative lanes
        }
    }

    template<typename B>
    inline bool all(const int1_t<B>& v) {
        auto nz = detail::nonzero_mask_int<B>(v.x);
        return B::movemask_ps(B::castsi_ps(nz)) == B::full_mask;
    }
    template<typename B>
    inline bool all(const float1_t<B>& v) {
        return B::movemask_ps(B::cmpne_ps(v.x, B::setzero_ps())) == B::full_mask;
    }

    template<typename B>
    inline bool none(const int1_t<B>& v) {
        return detail::none_impl<B>(v.x);
    }
    template<typename B>
    inline bool none(const float1_t<B>& v) {
        return detail::none_impl<B>(B::castps_si(v.x));
    }

    template<typename B, typename Func>
    inline void any(const int1_t<B>& v, Func&& f) {
        int mask = B::movemask_ps(B::castsi_ps(detail::nonzero_mask_int<B>(v.x)));
        for (int lane = 0; lane < B::width; ++lane)
            if (mask & (1 << lane)) f(lane);
    }
    template<typename B, typename Func>
    inline void any(const float1_t<B>& v, Func&& f) {
        int mask = B::movemask_ps(B::cmpne_ps(v.x, B::setzero_ps()));
        for (int lane = 0; lane < B::width; ++lane)
            if (mask & (1 << lane)) f(lane);
    }

    // ------ operator! for conditions ---------------------------------------------------------------------------------------------------------------------------------------
    template<typename tag, typename lhs_t, typename rhs_t>
    inline condition<tag_not, condition<tag, lhs_t, rhs_t>, void>
        operator!(const condition<tag, lhs_t, rhs_t>& c) { return { c }; }
    template<typename tag, typename lhs_t>
    inline condition<tag_not, condition<tag, lhs_t, void>, void>
        operator!(const condition<tag, lhs_t, void>& c) { return { c }; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // load / store
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline void store(int1_t<B>& v, const int* p) {
        if constexpr (B::width == 4) v.x = _mm_loadu_si128((const __m128i*)p);
        else                         v.x = _mm256_loadu_si256((const __m256i*)p);
    }
    template<typename B> inline void store(float1_t<B>& v, const float* p) {
        if constexpr (B::width == 4) v.x = _mm_loadu_ps(p);
        else                         v.x = _mm256_loadu_ps(p);
    }
    template<typename B> inline void load(const int1_t<B>& v, int* p) {
        if constexpr (B::width == 4) _mm_storeu_si128((__m128i*)p, v.x);
        else                         _mm256_storeu_si256((__m256i*)p, v.x);
    }
    template<typename B> inline void load(const float1_t<B>& v, float* p) {
        if constexpr (B::width == 4) _mm_storeu_ps(p, v.x);
        else                         _mm256_storeu_ps(p, v.x);
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // min / max
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float1_t<B> min(float1_t<B> a, float1_t<B> b) { return B::min_ps(a.x, b.x); }
    template<typename B> inline float1_t<B> max(float1_t<B> a, float1_t<B> b) { return B::max_ps(a.x, b.x); }
    template<typename B> inline int1_t<B>   min(int1_t<B> a, int1_t<B> b) { return B::min_epi32(a.x, b.x); }
    template<typename B> inline int1_t<B>   max(int1_t<B> a, int1_t<B> b) { return B::max_epi32(a.x, b.x); }

    template<typename B> inline float2_t<B> min(float2_t<B> a, float2_t<B> b) { return { min(a.x,b.x), min(a.y,b.y) }; }
    template<typename B> inline float2_t<B> max(float2_t<B> a, float2_t<B> b) { return { max(a.x,b.x), max(a.y,b.y) }; }
    template<typename B> inline float3_t<B> min(float3_t<B> a, float3_t<B> b) { return { min(a.x,b.x), min(a.y,b.y), min(a.z,b.z) }; }
    template<typename B> inline float3_t<B> max(float3_t<B> a, float3_t<B> b) { return { max(a.x,b.x), max(a.y,b.y), max(a.z,b.z) }; }
    template<typename B> inline float4_t<B> min(float4_t<B> a, float4_t<B> b) { return { min(a.x,b.x), min(a.y,b.y), min(a.z,b.z), min(a.w,b.w) }; }
    template<typename B> inline float4_t<B> max(float4_t<B> a, float4_t<B> b) { return { max(a.x,b.x), max(a.y,b.y), max(a.z,b.z), max(a.w,b.w) }; }

    template<typename B> inline float1_t<B> min(float1_t<B> a, float v) { return min(a, float1_t<B>(v)); }
    template<typename B> inline float1_t<B> max(float1_t<B> a, float v) { return max(a, float1_t<B>(v)); }
    template<typename B> inline int1_t<B>   min(int1_t<B>   a, int   v) { return min(a, int1_t<B>(v)); }
    template<typename B> inline int1_t<B>   max(int1_t<B>   a, int   v) { return max(a, int1_t<B>(v)); }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // abs
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B>
    inline float1_t<B> abs(float1_t<B> v) {
        return B::and_ps(v.x, B::castsi_ps(B::set1_epi32(0x7FFFFFFF)));
    }
    template<typename B>
    inline int1_t<B> abs(int1_t<B> v) {
        auto mask = B::cmpgt_epi32(B::setzero_si(), v.x);
        auto neg = B::sub_epi32(B::setzero_si(), v.x);
        return B::or_si(B::and_si(mask, neg), B::andnot_si(mask, v.x));
    }
    template<typename B> inline float2_t<B> abs(float2_t<B> v) { return { abs(v.x), abs(v.y) }; }
    template<typename B> inline float3_t<B> abs(float3_t<B> v) { return { abs(v.x), abs(v.y), abs(v.z) }; }
    template<typename B> inline float4_t<B> abs(float4_t<B> v) { return { abs(v.x), abs(v.y), abs(v.z), abs(v.w) }; }
    template<typename B> inline int2_t<B>   abs(int2_t<B>   v) { return { abs(v.x), abs(v.y) }; }
    template<typename B> inline int3_t<B>   abs(int3_t<B>   v) { return { abs(v.x), abs(v.y), abs(v.z) }; }
    template<typename B> inline int4_t<B>   abs(int4_t<B>   v) { return { abs(v.x), abs(v.y), abs(v.z), abs(v.w) }; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // floor / ceil / round / frac
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float1_t<B> floor(float1_t<B> v) { return B::floor_ps(v.x); }
    template<typename B> inline float1_t<B> round(float1_t<B> v) { return B::round_ps(v.x); }
    template<typename B> inline float1_t<B> ceil(float1_t<B> v) {
        return B::cvtepi32_ps(B::cvtps_epi32(B::add_ps(v.x, B::set1_ps(0.999999f))));
    }
    template<typename B> inline float1_t<B> frac(float1_t<B> v) { return v - floor(v); }

    template<typename B> inline float2_t<B> floor(float2_t<B> v) { return { floor(v.x), floor(v.y) }; }
    template<typename B> inline float3_t<B> floor(float3_t<B> v) { return { floor(v.x), floor(v.y), floor(v.z) }; }
    template<typename B> inline float4_t<B> floor(float4_t<B> v) { return { floor(v.x), floor(v.y), floor(v.z), floor(v.w) }; }
    template<typename B> inline float2_t<B> ceil(float2_t<B> v) { return { ceil(v.x),  ceil(v.y) }; }
    template<typename B> inline float3_t<B> ceil(float3_t<B> v) { return { ceil(v.x),  ceil(v.y),  ceil(v.z) }; }
    template<typename B> inline float4_t<B> ceil(float4_t<B> v) { return { ceil(v.x),  ceil(v.y),  ceil(v.z),  ceil(v.w) }; }
    template<typename B> inline float2_t<B> round(float2_t<B> v) { return { round(v.x), round(v.y) }; }
    template<typename B> inline float3_t<B> round(float3_t<B> v) { return { round(v.x), round(v.y), round(v.z) }; }
    template<typename B> inline float4_t<B> round(float4_t<B> v) { return { round(v.x), round(v.y), round(v.z), round(v.w) }; }
    template<typename B> inline float2_t<B> frac(float2_t<B> v) { return { frac(v.x),  frac(v.y) }; }
    template<typename B> inline float3_t<B> frac(float3_t<B> v) { return { frac(v.x),  frac(v.y),  frac(v.z) }; }
    template<typename B> inline float4_t<B> frac(float4_t<B> v) { return { frac(v.x),  frac(v.y),  frac(v.z),  frac(v.w) }; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // sqrt / rsqrt
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float1_t<B> sqrt(float1_t<B> v) { return B::sqrt_ps(v.x); }
    template<typename B> inline float2_t<B> sqrt(float2_t<B> v) { return { sqrt(v.x), sqrt(v.y) }; }
    template<typename B> inline float3_t<B> sqrt(float3_t<B> v) { return { sqrt(v.x), sqrt(v.y), sqrt(v.z) }; }
    template<typename B> inline float4_t<B> sqrt(float4_t<B> v) { return { sqrt(v.x), sqrt(v.y), sqrt(v.z), sqrt(v.w) }; }

    template<typename B> inline float1_t<B> rsqrt(float1_t<B> v) {
        if constexpr (B::width == 4) return _mm_rsqrt_ps(v.x);
        else                         return _mm256_rsqrt_ps(v.x);
    }
    template<typename B> inline float2_t<B> rsqrt(float2_t<B> v) { return { rsqrt(v.x), rsqrt(v.y) }; }
    template<typename B> inline float3_t<B> rsqrt(float3_t<B> v) { return { rsqrt(v.x), rsqrt(v.y), rsqrt(v.z) }; }
    template<typename B> inline float4_t<B> rsqrt(float4_t<B> v) { return { rsqrt(v.x), rsqrt(v.y), rsqrt(v.z), rsqrt(v.w) }; }

    // Newton-Raphson accurate rsqrt
    template<typename B> inline float1_t<B> rsqrt_accurate(float1_t<B> v) {
        float1_t<B> y = rsqrt(v);
        float1_t<B> half(0.5f), three_halves(1.5f);
        return y * (three_halves - half * v * y * y);
    }

    template<typename B> inline float1_t<B> sqrt_safe(float1_t<B> v) {
        return sqrt(max(v, float1_t<B>(0.f)));
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // clamp
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float1_t<B> clamp01(float1_t<B> v) {
        return B::min_ps(B::max_ps(v.x, B::setzero_ps()), B::set1_ps(1.f));
    }
    template<typename B> inline float2_t<B> clamp01(float2_t<B> v) { return { clamp01(v.x), clamp01(v.y) }; }
    template<typename B> inline float3_t<B> clamp01(float3_t<B> v) { return { clamp01(v.x), clamp01(v.y), clamp01(v.z) }; }
    template<typename B> inline float4_t<B> clamp01(float4_t<B> v) { return { clamp01(v.x), clamp01(v.y), clamp01(v.z), clamp01(v.w) }; }

    template<typename B> inline float1_t<B> clamp(float1_t<B> v, float lo, float hi) {
        return B::min_ps(B::max_ps(v.x, B::set1_ps(lo)), B::set1_ps(hi));
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // dot / length / normalize / cross / distance
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B>
    inline float1_t<B> dot(const float2_t<B>& a, const float2_t<B>& b) {
        return B::add_ps(B::mul_ps(a.x.x, b.x.x), B::mul_ps(a.y.x, b.y.x));
    }
    template<typename B>
    inline float1_t<B> dot(const float3_t<B>& a, const float3_t<B>& b) {
        auto xy = B::add_ps(B::mul_ps(a.x.x, b.x.x), B::mul_ps(a.y.x, b.y.x));
        return B::add_ps(xy, B::mul_ps(a.z.x, b.z.x));
    }
    template<typename B>
    inline float1_t<B> dot(const float4_t<B>& a, const float4_t<B>& b) {
        auto xy = B::add_ps(B::mul_ps(a.x.x, b.x.x), B::mul_ps(a.y.x, b.y.x));
        auto zw = B::add_ps(B::mul_ps(a.z.x, b.z.x), B::mul_ps(a.w.x, b.w.x));
        return B::add_ps(xy, zw);
    }

    template<typename B> inline float1_t<B> length(const float2_t<B>& v) { return sqrt(dot(v, v)); }
    template<typename B> inline float1_t<B> length(const float3_t<B>& v) { return sqrt(dot(v, v)); }
    template<typename B> inline float1_t<B> length(const float4_t<B>& v) { return sqrt(dot(v, v)); }

    template<typename B> inline float1_t<B> distance(const float2_t<B>& a, const float2_t<B>& b) { return length(a - b); }
    template<typename B> inline float1_t<B> distance(const float3_t<B>& a, const float3_t<B>& b) { return length(a - b); }
    template<typename B> inline float1_t<B> distance(const float4_t<B>& a, const float4_t<B>& b) { return length(a - b); }

    template<typename B>
    inline float3_t<B> normalize(const float3_t<B>& v) {
        float1_t<B> len = length(v);
        auto mask = B::cmpne_ps(len.x, B::setzero_ps());
        auto inv_len = B::and_ps(B::div_ps(B::set1_ps(1.f), len.x), mask);
        return { B::mul_ps(v.x.x, inv_len), B::mul_ps(v.y.x, inv_len), B::mul_ps(v.z.x, inv_len) };
    }
    template<typename B>
    inline float2_t<B> normalize(const float2_t<B>& v) {
        float1_t<B> len = length(v);
        auto mask = B::cmpne_ps(len.x, B::setzero_ps());
        auto inv_len = B::and_ps(B::div_ps(B::set1_ps(1.f), len.x), mask);
        return { B::mul_ps(v.x.x, inv_len), B::mul_ps(v.y.x, inv_len) };
    }
    template<typename B>
    inline float4_t<B> normalize(const float4_t<B>& v) {
        float1_t<B> len = length(v);
        auto mask = B::cmpne_ps(len.x, B::setzero_ps());
        auto inv_len = B::and_ps(B::div_ps(B::set1_ps(1.f), len.x), mask);
        return { B::mul_ps(v.x.x, inv_len), B::mul_ps(v.y.x, inv_len),
                 B::mul_ps(v.z.x, inv_len), B::mul_ps(v.w.x, inv_len) };
    }

    template<typename B>
    inline float3_t<B> fast_normalize(const float3_t<B>& v) {
        float1_t<B> len_sq = dot(v, v);
        auto mask = B::cmpne_ps(len_sq.x, B::setzero_ps());
        auto inv_len = B::and_ps(rsqrt(len_sq).x, mask);
        return { B::mul_ps(v.x.x, inv_len), B::mul_ps(v.y.x, inv_len), B::mul_ps(v.z.x, inv_len) };
    }

    template<typename B>
    inline float3_t<B> cross(const float3_t<B>& a, const float3_t<B>& b) {
        return {
            B::sub_ps(B::mul_ps(a.y.x, b.z.x), B::mul_ps(a.z.x, b.y.x)),
            B::sub_ps(B::mul_ps(a.z.x, b.x.x), B::mul_ps(a.x.x, b.z.x)),
            B::sub_ps(B::mul_ps(a.x.x, b.y.x), B::mul_ps(a.y.x, b.x.x))
        };
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // reflect / refract / faceforward
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B>
    inline float3_t<B> reflect(const float3_t<B>& v, const float3_t<B>& n) {
        return v - n * (dot(v, n) * float1_t<B>(2.f));
    }

    template<typename B>
    inline float3_t<B> refract(const float3_t<B>& v, const float3_t<B>& n, float eta) {
        float1_t<B> dot_vn = dot(v, n);
        float1_t<B> k = float1_t<B>(1.f) - float1_t<B>(eta * eta) * (float1_t<B>(1.f) - dot_vn * dot_vn);
        auto mask = B::cmpge_ps(k.x, B::setzero_ps());
        float3_t<B> r = v * float1_t<B>(eta) - n * (float1_t<B>(eta) * dot_vn + sqrt(k));
        return { B::and_ps(r.x.x, mask), B::and_ps(r.y.x, mask), B::and_ps(r.z.x, mask) };
    }

    template<typename B>
    inline float3_t<B> faceforward(const float3_t<B>& n, const float3_t<B>& i, const float3_t<B>& nref) {
        auto mask = B::cmplt_ps(dot(i, nref).x, B::setzero_ps());
        auto neg = [&](auto c) { return B::sub_ps(B::setzero_ps(), c); };
        return {
            B::or_ps(B::and_ps(mask, n.x.x), B::andnot_ps(mask, neg(n.x.x))),
            B::or_ps(B::and_ps(mask, n.y.x), B::andnot_ps(mask, neg(n.y.x))),
            B::or_ps(B::and_ps(mask, n.z.x), B::andnot_ps(mask, neg(n.z.x)))
        };
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // select / ternary
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B>
    inline int1_t<B> select(const int1_t<B>& cond, const int1_t<B>& if_true, const int1_t<B>& if_false) {
        return B::or_si(B::and_si(cond.x, if_true.x), B::andnot_si(cond.x, if_false.x));
    }
    template<typename B>
    inline float1_t<B> select(const float1_t<B>& cond, const float1_t<B>& if_true, const float1_t<B>& if_false) {
        return B::or_ps(B::and_ps(cond.x, if_true.x), B::andnot_ps(cond.x, if_false.x));
    }
    template<typename B>
    inline float1_t<B> select(const int1_t<B>& cond, const float1_t<B>& if_true, const float1_t<B>& if_false) {
        auto cf = B::castsi_ps(cond.x);
        return B::or_ps(B::and_ps(cf, if_true.x), B::andnot_ps(cf, if_false.x));
    }
    template<typename B>
    inline int1_t<B> select(const float1_t<B>& cond, const int1_t<B>& if_true, const int1_t<B>& if_false) {
        auto ci = B::castps_si(cond.x);
        return B::or_si(B::and_si(ci, if_true.x), B::andnot_si(ci, if_false.x));
    }
    // scalar literal overloads --- select(cond, 2.0f, 0.0f)
    template<typename B>
    inline float1_t<B> select(const float1_t<B>& cond, float if_true, float if_false) {
        return select(cond, float1_t<B>(if_true), float1_t<B>(if_false));
    }
    template<typename B>
    inline float1_t<B> select(const int1_t<B>& cond, float if_true, float if_false) {
        return select(cond, float1_t<B>(if_true), float1_t<B>(if_false));
    }
    template<typename B>
    inline int1_t<B> select(const int1_t<B>& cond, int if_true, int if_false) {
        return select(cond, int1_t<B>(if_true), int1_t<B>(if_false));
    }
    // condition<> overloads --- select((a >= b) & (a >= c), 2.0f, 0.0f)
    template<typename B, typename tag, typename L, typename R>
    inline float1_t<B> select(const condition<tag, L, R>& cond, float1_t<B> if_true, float1_t<B> if_false) {
        return select(float1_t<B>(cond), if_true, if_false);
    }
    template<typename B, typename tag, typename L, typename R>
    inline float1_t<B> select(const condition<tag, L, R>& cond, float if_true, float if_false) {
        return select(float1_t<B>(cond), float1_t<B>(if_true), float1_t<B>(if_false));
    }
    template<typename B, typename tag, typename L, typename R>
    inline int1_t<B> select(const condition<tag, L, R>& cond, int1_t<B> if_true, int1_t<B> if_false) {
        return select(float1_t<B>(cond), if_true, if_false);
    }
    template<typename B, typename tag, typename L, typename R>
    inline int1_t<B> select(const condition<tag, L, R>& cond, int if_true, int if_false) {
        return select(float1_t<B>(cond), int1_t<B>(if_true), int1_t<B>(if_false));
    }

    template<typename Cond, typename T, typename U>
    inline auto ternary(const Cond& cond, const T& if_true, const U& if_false) {
        return select(cond, if_true, if_false);
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // get --- extract a single lane
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float get(const float1_t<B>& v, int i) { return v[i]; }
    template<typename B> inline int   get(const int1_t<B>& v, int i) { return v[i]; }

    template<int id, typename B> inline float get(const float1_t<B>& v) {
        static_assert(id >= 0 && id < B::width);
        return v[id];
    }
    template<int id, typename B> inline int get(const int1_t<B>& v) {
        static_assert(id >= 0 && id < B::width);
        return v[id];
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // widen (SSE --- AVX) / split (AVX --- SSE)
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    inline avx::float1 widen(sse::float1 lo, sse::float1 hi) {
        return _mm256_set_m128(hi.x, lo.x);
    }
    inline avx::int1 widen(sse::int1 lo, sse::int1 hi) {
        return _mm256_set_m128i(hi.x, lo.x);
    }
    inline avx::float3 widen(const sse::float3& lo, const sse::float3& hi) {
        return { widen(lo.x,hi.x), widen(lo.y,hi.y), widen(lo.z,hi.z) };
    }
    inline avx::float4 widen(const sse::float4& lo, const sse::float4& hi) {
        return { widen(lo.x,hi.x), widen(lo.y,hi.y), widen(lo.z,hi.z), widen(lo.w,hi.w) };
    }

    inline sse::float1 narrow_lo(avx::float1 v) { return _mm256_castps256_ps128(v.x); }
    inline sse::float1 narrow_hi(avx::float1 v) { return _mm256_extractf128_ps(v.x, 1); }
    inline sse::int1   narrow_lo(avx::int1 v) { return _mm256_castsi256_si128(v.x); }
    inline sse::int1   narrow_hi(avx::int1 v) { return _mm256_extracti128_si256(v.x, 1); }

    inline std::pair<sse::float3, sse::float3> split(const avx::float3& v) {
        return {
            sse::float3{ narrow_lo(v.x), narrow_lo(v.y), narrow_lo(v.z) },
            sse::float3{ narrow_hi(v.x), narrow_hi(v.y), narrow_hi(v.z) }
        };
    }
    inline std::pair<sse::float4, sse::float4> split(const avx::float4& v) {
        return {
            sse::float4{ narrow_lo(v.x), narrow_lo(v.y), narrow_lo(v.z), narrow_lo(v.w) },
            sse::float4{ narrow_hi(v.x), narrow_hi(v.y), narrow_hi(v.z), narrow_hi(v.w) }
        };
    }

    // zeroupper --- call when switching from AVX back to SSE code
    inline void zeroupper() { _mm256_zeroupper(); }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // pack / unpack  --- float -> int1 color, int1 -> float color
    //
    // naming: pack_<format>  /  unpack_<format>
    //   format = argb | rgba | rgb (rgb = no alpha channel, A forced to 255)
    //   all values clamped 0..1 before packing
    //
    //   pack_argb(float4)  ->  int1   0xAARRGGBB
    //   pack_rgba(float4)  ->  int1   0xRRGGBBAA
    //   pack_argb(float3)  ->  int1   0xFFRRGGBB  (A=255)
    //   pack_rgba(float3)  ->  int1   0xRRGGBBFF  (A=255)
    //
    //   unpack_argb(int1)  ->  float4  xyzw = ARGB
    //   unpack_rgba(int1)  ->  float4  xyzw = RGBA
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    namespace detail {
        template<typename B>
        inline auto pack_clamp_ch(typename B::float_reg ch) {
            auto scale = B::set1_ps(255.f);
            auto zero = B::setzero_si();
            auto max255 = B::set1_epi32(255);
            return B::max_epi32(B::min_epi32(B::cvtps_epi32(B::mul_ps(ch, scale)), max255), zero);
        }
    }

    // pack_argb(float4)  ->  0xAARRGGBB
    template<typename B>
    inline int1_t<B> pack_argb(const float4_t<B>& v) {
        auto r = detail::pack_clamp_ch<B>(v.x.x);
        auto g = detail::pack_clamp_ch<B>(v.y.x);
        auto b = detail::pack_clamp_ch<B>(v.z.x);
        auto a = detail::pack_clamp_ch<B>(v.w.x);
        return B::or_si(B::or_si(B::slli_epi32(a, 24), B::slli_epi32(r, 16)),
            B::or_si(B::slli_epi32(g, 8), b));
    }

    // pack_rgba(float4)  ->  0xRRGGBBAA
    template<typename B>
    inline int1_t<B> pack_rgba(const float4_t<B>& v) {
        auto r = detail::pack_clamp_ch<B>(v.x.x);
        auto g = detail::pack_clamp_ch<B>(v.y.x);
        auto b = detail::pack_clamp_ch<B>(v.z.x);
        auto a = detail::pack_clamp_ch<B>(v.w.x);
        return B::or_si(B::or_si(B::slli_epi32(r, 24), B::slli_epi32(g, 16)),
            B::or_si(B::slli_epi32(b, 8), a));
    }

    // pack_argb(float3)  ->  0xFFRRGGBB  (A=255)
    template<typename B>
    inline int1_t<B> pack_argb(const float3_t<B>& v) {
        auto r = detail::pack_clamp_ch<B>(v.x.x);
        auto g = detail::pack_clamp_ch<B>(v.y.x);
        auto b = detail::pack_clamp_ch<B>(v.z.x);
        auto a = B::set1_epi32(255);
        return B::or_si(B::or_si(B::slli_epi32(a, 24), B::slli_epi32(r, 16)),
            B::or_si(B::slli_epi32(g, 8), b));
    }

    // pack_rgba(float3)  ->  0xRRGGBBFF  (A=255)
    template<typename B>
    inline int1_t<B> pack_rgba(const float3_t<B>& v) {
        auto r = detail::pack_clamp_ch<B>(v.x.x);
        auto g = detail::pack_clamp_ch<B>(v.y.x);
        auto b = detail::pack_clamp_ch<B>(v.z.x);
        auto a = B::set1_epi32(255);
        return B::or_si(B::or_si(B::slli_epi32(r, 24), B::slli_epi32(g, 16)),
            B::or_si(B::slli_epi32(b, 8), a));
    }

    // unpack_argb(int1)  ->  float4 (a,r,g,b) in 0..1
    template<typename B>
    inline float4_t<B> unpack_argb(const int1_t<B>& c) {
        auto inv255 = B::set1_ps(1.f / 255.f);
        auto mask = B::set1_epi32(0xFF);
        auto a = B::and_si(B::srli_epi32(c.x, 24), mask);
        auto r = B::and_si(B::srli_epi32(c.x, 16), mask);
        auto g = B::and_si(B::srli_epi32(c.x, 8), mask);
        auto b = B::and_si(c.x, mask);
        return { B::mul_ps(B::cvtepi32_ps(a), inv255), B::mul_ps(B::cvtepi32_ps(r), inv255),
                 B::mul_ps(B::cvtepi32_ps(g), inv255), B::mul_ps(B::cvtepi32_ps(b), inv255) };
    }

    // unpack_rgba(int1)  ->  float4 (r,g,b,a) in 0..1
    template<typename B>
    inline float4_t<B> unpack_rgba(const int1_t<B>& c) {
        auto inv255 = B::set1_ps(1.f / 255.f);
        auto mask = B::set1_epi32(0xFF);
        auto r = B::and_si(B::srli_epi32(c.x, 24), mask);
        auto g = B::and_si(B::srli_epi32(c.x, 16), mask);
        auto b = B::and_si(B::srli_epi32(c.x, 8), mask);
        auto a = B::and_si(c.x, mask);
        return { B::mul_ps(B::cvtepi32_ps(r), inv255), B::mul_ps(B::cvtepi32_ps(g), inv255),
                 B::mul_ps(B::cvtepi32_ps(b), inv255), B::mul_ps(B::cvtepi32_ps(a), inv255) };
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // bool helpers
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B>
    inline bool all_zero(const int1_t<B>& v) {
        if constexpr (B::width == 4) return _mm_testz_si128(v.x, v.x);
        else                         return _mm256_testz_si256(v.x, v.x);
    }
    template<typename B> inline bool any_nonzero(const int1_t<B>& v) { return !all_zero(v); }
    template<typename B> inline bool any_zero(const int1_t<B>& v) {
        return B::movemask_ps(B::castsi_ps(B::cmpeq_epi32(v.x, B::setzero_si()))) != 0;
    }
    template<typename B> inline bool all_nonzero(const int1_t<B>& v) {
        return B::movemask_ps(B::castsi_ps(B::cmpeq_epi32(v.x, B::setzero_si()))) == 0;
    }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // range conversion
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float1_t<B> to_01(const float1_t<B>& v) { return v * 0.5f + 0.5f; }
    template<typename B> inline float2_t<B> to_01(const float2_t<B>& v) { return v * 0.5f + 0.5f; }
    template<typename B> inline float3_t<B> to_01(const float3_t<B>& v) { return v * 0.5f + 0.5f; }
    template<typename B> inline float4_t<B> to_01(const float4_t<B>& v) { return v * 0.5f + 0.5f; }

    template<typename B> inline float1_t<B> to_11(const float1_t<B>& v) { return v * 2.0f - 1.0f; }
    template<typename B> inline float2_t<B> to_11(const float2_t<B>& v) { return v * 2.0f - 1.0f; }
    template<typename B> inline float3_t<B> to_11(const float3_t<B>& v) { return v * 2.0f - 1.0f; }
    template<typename B> inline float4_t<B> to_11(const float4_t<B>& v) { return v * 2.0f - 1.0f; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // to_float1/2/3/4  and  to_int1/2/3/4  --- type cast / swizzle
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // to_float2
    template<typename B> inline float2_t<B> to_float2(const int2_t<B>& v) { return { to_float1(v.x), to_float1(v.y) }; }
    template<typename B> inline float2_t<B> to_float2(const float3_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline float2_t<B> to_float2(const float4_t<B>& v) { return { v.x, v.y }; }

    // to_float3
    template<typename B> inline float3_t<B> to_float3(const int3_t<B>& v) { return { to_float1(v.x), to_float1(v.y), to_float1(v.z) }; }
    template<typename B> inline float3_t<B> to_float3(const float4_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline float3_t<B> to_float3(const float2_t<B>& v, float1_t<B> z = float1_t<B>(0.f)) { return { v.x, v.y, z }; }

    // to_float4
    template<typename B> inline float4_t<B> to_float4(const float3_t<B>& v, float1_t<B> w = float1_t<B>(1.f)) { return { v.x, v.y, v.z, w }; }
    template<typename B> inline float4_t<B> to_float4(const int4_t<B>& v) { return { to_float1(v.x), to_float1(v.y), to_float1(v.z), to_float1(v.w) }; }

    // to_int2
    template<typename B> inline int2_t<B> to_int2(const float2_t<B>& v) { return { to_int1(v.x), to_int1(v.y) }; }
    template<typename B> inline int2_t<B> to_int2(const int3_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline int2_t<B> to_int2(const int4_t<B>& v) { return { v.x, v.y }; }

    // to_int3
    template<typename B> inline int3_t<B> to_int3(const float3_t<B>& v) { return { to_int1(v.x), to_int1(v.y), to_int1(v.z) }; }
    template<typename B> inline int3_t<B> to_int3(const int4_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline int3_t<B> to_int3(const int2_t<B>& v, int1_t<B> z = int1_t<B>(0)) { return { v.x, v.y, z }; }

    // to_int4
    template<typename B> inline int4_t<B> to_int4(const float4_t<B>& v) { return { to_int1(v.x), to_int1(v.y), to_int1(v.z), to_int1(v.w) }; }
    template<typename B> inline int4_t<B> to_int4(const int3_t<B>& v, int1_t<B> w = int1_t<B>(0)) { return { v.x, v.y, v.z, w }; }

} // namespace st
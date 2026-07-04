#pragma once
#include <immintrin.h>
#include <type_traits>
#include <utility>

#undef min
#undef max

namespace st {
    namespace backend {

        // --- SSE (4-wide) ---
        struct sse {
            using float_reg = __m128;
            using int_reg = __m128i;
            static constexpr int width = 4;
            static constexpr int full_mask = 0xF;

            static float_reg add_ps(float_reg a, float_reg b) { return _mm_add_ps(a, b); }
            static float_reg sub_ps(float_reg a, float_reg b) { return _mm_sub_ps(a, b); }
            static float_reg mul_ps(float_reg a, float_reg b) { return _mm_mul_ps(a, b); }
            static float_reg div_ps(float_reg a, float_reg b) { return _mm_div_ps(a, b); }
            static float_reg sqrt_ps(float_reg a) { return _mm_sqrt_ps(a); }
            static float_reg min_ps(float_reg a, float_reg b) { return _mm_min_ps(a, b); }
            static float_reg max_ps(float_reg a, float_reg b) { return _mm_max_ps(a, b); }
            static float_reg and_ps(float_reg a, float_reg b) { return _mm_and_ps(a, b); }
            static float_reg or_ps(float_reg a, float_reg b) { return _mm_or_ps(a, b); }
            static float_reg xor_ps(float_reg a, float_reg b) { return _mm_xor_ps(a, b); }
            static float_reg andnot_ps(float_reg a, float_reg b) { return _mm_andnot_ps(a, b); }
            static float_reg set1_ps(float v) { return _mm_set1_ps(v); }
            static float_reg setzero_ps() { return _mm_setzero_ps(); }
            static float_reg floor_ps(float_reg a) { return _mm_floor_ps(a); }
            static float_reg round_ps(float_reg a) { return _mm_round_ps(a, _MM_FROUND_TO_NEAREST_INT); }

            static float_reg cmpgt_ps(float_reg a, float_reg b) { return _mm_cmpgt_ps(a, b); }
            static float_reg cmplt_ps(float_reg a, float_reg b) { return _mm_cmplt_ps(a, b); }
            static float_reg cmpeq_ps(float_reg a, float_reg b) { return _mm_cmpeq_ps(a, b); }
            static float_reg cmpge_ps(float_reg a, float_reg b) { return _mm_cmpge_ps(a, b); }
            static float_reg cmple_ps(float_reg a, float_reg b) { return _mm_cmple_ps(a, b); }
            static float_reg cmpne_ps(float_reg a, float_reg b) { return _mm_cmpneq_ps(a, b); }

            static int_reg   add_epi32(int_reg a, int_reg b) { return _mm_add_epi32(a, b); }
            static int_reg   sub_epi32(int_reg a, int_reg b) { return _mm_sub_epi32(a, b); }
            static int_reg   mul_epi32(int_reg a, int_reg b) { return _mm_mullo_epi32(a, b); }
            static int_reg   min_epi32(int_reg a, int_reg b) { return _mm_min_epi32(a, b); }
            static int_reg   max_epi32(int_reg a, int_reg b) { return _mm_max_epi32(a, b); }
            static int_reg   and_si(int_reg a, int_reg b) { return _mm_and_si128(a, b); }
            static int_reg   or_si(int_reg a, int_reg b) { return _mm_or_si128(a, b); }
            static int_reg   xor_si(int_reg a, int_reg b) { return _mm_xor_si128(a, b); }
            static int_reg   andnot_si(int_reg a, int_reg b) { return _mm_andnot_si128(a, b); }
            static int_reg   set1_epi32(int v) { return _mm_set1_epi32(v); }
            static int_reg   setzero_si() { return _mm_setzero_si128(); }
            static int_reg   cmpeq_epi32(int_reg a, int_reg b) { return _mm_cmpeq_epi32(a, b); }
            static int_reg   cmpgt_epi32(int_reg a, int_reg b) { return _mm_cmpgt_epi32(a, b); }
            static int_reg   slli_epi32(int_reg a, int bits) { return _mm_slli_epi32(a, bits); }
            static int_reg   srli_epi32(int_reg a, int bits) { return _mm_srli_epi32(a, bits); }
            static int_reg   srai_epi32(int_reg a, int bits) { return _mm_srai_epi32(a, bits); }

            static float_reg cvtepi32_ps(int_reg a) { return _mm_cvtepi32_ps(a); }
            static int_reg   cvtps_epi32(float_reg a) { return _mm_cvtps_epi32(a); }
            static int_reg   castps_si(float_reg a) { return _mm_castps_si128(a); }
            static float_reg castsi_ps(int_reg a) { return _mm_castsi128_ps(a); }
            static int       movemask_ps(float_reg a) { return _mm_movemask_ps(a); }
        };

        // --- AVX-256 ---
        struct avx256 {
            using float_reg = __m256;
            using int_reg = __m256i;
            static constexpr int width = 8;
            static constexpr int full_mask = 0xFF;

            static float_reg add_ps(float_reg a, float_reg b) { return _mm256_add_ps(a, b); }
            static float_reg sub_ps(float_reg a, float_reg b) { return _mm256_sub_ps(a, b); }
            static float_reg mul_ps(float_reg a, float_reg b) { return _mm256_mul_ps(a, b); }
            static float_reg div_ps(float_reg a, float_reg b) { return _mm256_div_ps(a, b); }
            static float_reg sqrt_ps(float_reg a) { return _mm256_sqrt_ps(a); }
            static float_reg min_ps(float_reg a, float_reg b) { return _mm256_min_ps(a, b); }
            static float_reg max_ps(float_reg a, float_reg b) { return _mm256_max_ps(a, b); }
            static float_reg and_ps(float_reg a, float_reg b) { return _mm256_and_ps(a, b); }
            static float_reg or_ps(float_reg a, float_reg b) { return _mm256_or_ps(a, b); }
            static float_reg xor_ps(float_reg a, float_reg b) { return _mm256_xor_ps(a, b); }
            static float_reg andnot_ps(float_reg a, float_reg b) { return _mm256_andnot_ps(a, b); }
            static float_reg set1_ps(float v) { return _mm256_set1_ps(v); }
            static float_reg setzero_ps() { return _mm256_setzero_ps(); }
            static float_reg floor_ps(float_reg a) { return _mm256_floor_ps(a); }
            static float_reg round_ps(float_reg a) { return _mm256_round_ps(a, _MM_FROUND_TO_NEAREST_INT); }

            // AVX uses _mm256_cmp_ps with predicate
            static float_reg cmpgt_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_GT_OQ); }
            static float_reg cmplt_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_LT_OQ); }
            static float_reg cmpeq_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_EQ_OQ); }
            static float_reg cmpge_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_GE_OQ); }
            static float_reg cmple_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_LE_OQ); }
            static float_reg cmpne_ps(float_reg a, float_reg b) { return _mm256_cmp_ps(a, b, _CMP_NEQ_OQ); }

            static int_reg   add_epi32(int_reg a, int_reg b) { return _mm256_add_epi32(a, b); }
            static int_reg   sub_epi32(int_reg a, int_reg b) { return _mm256_sub_epi32(a, b); }
            static int_reg   mul_epi32(int_reg a, int_reg b) { return _mm256_mullo_epi32(a, b); }  // AVX2
            static int_reg   min_epi32(int_reg a, int_reg b) { return _mm256_min_epi32(a, b); }
            static int_reg   max_epi32(int_reg a, int_reg b) { return _mm256_max_epi32(a, b); }
            static int_reg   and_si(int_reg a, int_reg b) { return _mm256_and_si256(a, b); }
            static int_reg   or_si(int_reg a, int_reg b) { return _mm256_or_si256(a, b); }
            static int_reg   xor_si(int_reg a, int_reg b) { return _mm256_xor_si256(a, b); }
            static int_reg   andnot_si(int_reg a, int_reg b) { return _mm256_andnot_si256(a, b); }
            static int_reg   set1_epi32(int v) { return _mm256_set1_epi32(v); }
            static int_reg   setzero_si() { return _mm256_setzero_si256(); }
            static int_reg   cmpeq_epi32(int_reg a, int_reg b) { return _mm256_cmpeq_epi32(a, b); }  // AVX2
            static int_reg   cmpgt_epi32(int_reg a, int_reg b) { return _mm256_cmpgt_epi32(a, b); }  // AVX2
            static int_reg   slli_epi32(int_reg a, int bits) { return _mm256_slli_epi32(a, bits); }
            static int_reg   srli_epi32(int_reg a, int bits) { return _mm256_srli_epi32(a, bits); }
            static int_reg   srai_epi32(int_reg a, int bits) { return _mm256_srai_epi32(a, bits); }

            static float_reg cvtepi32_ps(int_reg a) { return _mm256_cvtepi32_ps(a); }
            static int_reg   cvtps_epi32(float_reg a) { return _mm256_cvtps_epi32(a); }
            static int_reg   castps_si(float_reg a) { return _mm256_castps_si256(a); }
            static float_reg castsi_ps(int_reg a) { return _mm256_castsi256_ps(a); }
            static int       movemask_ps(float_reg a) { return _mm256_movemask_ps(a); }
        };

        // --- AVX-512 stub (16-wide) ---
        // Fill in when targeting AVX-512 hardware.
        // All function names are identical --- only the _mm512_ prefix changes.
        struct avx512 {
            using float_reg = __m512;
            using int_reg = __m512i;
            static constexpr int width = 16;
            static constexpr int full_mask = 0xFFFF;

            // Arithmetic
            static float_reg add_ps(float_reg a, float_reg b) { return _mm512_add_ps(a, b); }
            static float_reg sub_ps(float_reg a, float_reg b) { return _mm512_sub_ps(a, b); }
            static float_reg mul_ps(float_reg a, float_reg b) { return _mm512_mul_ps(a, b); }
            static float_reg div_ps(float_reg a, float_reg b) { return _mm512_div_ps(a, b); }
            static float_reg sqrt_ps(float_reg a) { return _mm512_sqrt_ps(a); }
            static float_reg min_ps(float_reg a, float_reg b) { return _mm512_min_ps(a, b); }
            static float_reg max_ps(float_reg a, float_reg b) { return _mm512_max_ps(a, b); }
            static float_reg set1_ps(float v) { return _mm512_set1_ps(v); }
            static float_reg setzero_ps() { return _mm512_setzero_ps(); }
            static float_reg floor_ps(float_reg a) { return _mm512_floor_ps(a); }
            static float_reg round_ps(float_reg a) { return _mm512_roundscale_ps(a, _MM_FROUND_TO_NEAREST_INT); }

            // AVX-512 comparisons return __mmask16, not a register
            // We convert to a full register via _mm512_movm_epi32 for uniform interface
            static float_reg cmpgt_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_GT_OQ)));
            }
            static float_reg cmplt_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_LT_OQ)));
            }
            static float_reg cmpeq_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_EQ_OQ)));
            }
            static float_reg cmpge_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_GE_OQ)));
            }
            static float_reg cmple_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_LE_OQ)));
            }
            static float_reg cmpne_ps(float_reg a, float_reg b) {
                return _mm512_castsi512_ps(_mm512_movm_epi32(_mm512_cmp_ps_mask(a, b, _CMP_NEQ_OQ)));
            }

            // Bitwise float
            static float_reg and_ps(float_reg a, float_reg b) { return _mm512_castsi512_ps(_mm512_and_si512(_mm512_castps_si512(a), _mm512_castps_si512(b))); }
            static float_reg or_ps(float_reg a, float_reg b) { return _mm512_castsi512_ps(_mm512_or_si512(_mm512_castps_si512(a), _mm512_castps_si512(b))); }
            static float_reg xor_ps(float_reg a, float_reg b) { return _mm512_castsi512_ps(_mm512_xor_si512(_mm512_castps_si512(a), _mm512_castps_si512(b))); }
            static float_reg andnot_ps(float_reg a, float_reg b) { return _mm512_castsi512_ps(_mm512_andnot_si512(_mm512_castps_si512(a), _mm512_castps_si512(b))); }

            // Integer
            static int_reg   add_epi32(int_reg a, int_reg b) { return _mm512_add_epi32(a, b); }
            static int_reg   sub_epi32(int_reg a, int_reg b) { return _mm512_sub_epi32(a, b); }
            static int_reg   mul_epi32(int_reg a, int_reg b) { return _mm512_mullo_epi32(a, b); }
            static int_reg   min_epi32(int_reg a, int_reg b) { return _mm512_min_epi32(a, b); }
            static int_reg   max_epi32(int_reg a, int_reg b) { return _mm512_max_epi32(a, b); }
            static int_reg   and_si(int_reg a, int_reg b) { return _mm512_and_si512(a, b); }
            static int_reg   or_si(int_reg a, int_reg b) { return _mm512_or_si512(a, b); }
            static int_reg   xor_si(int_reg a, int_reg b) { return _mm512_xor_si512(a, b); }
            static int_reg   andnot_si(int_reg a, int_reg b) { return _mm512_andnot_si512(a, b); }
            static int_reg   set1_epi32(int v) { return _mm512_set1_epi32(v); }
            static int_reg   setzero_si() { return _mm512_setzero_si512(); }
            static int_reg   cmpeq_epi32(int_reg a, int_reg b) { return _mm512_movm_epi32(_mm512_cmpeq_epi32_mask(a, b)); }
            static int_reg   cmpgt_epi32(int_reg a, int_reg b) { return _mm512_movm_epi32(_mm512_cmpgt_epi32_mask(a, b)); }
            static int_reg   slli_epi32(int_reg a, int bits) { return _mm512_slli_epi32(a, bits); }
            static int_reg   srli_epi32(int_reg a, int bits) { return _mm512_srli_epi32(a, bits); }
            static int_reg   srai_epi32(int_reg a, int bits) { return _mm512_srai_epi32(a, bits); }

            static float_reg cvtepi32_ps(int_reg a) { return _mm512_cvtepi32_ps(a); }
            static int_reg   cvtps_epi32(float_reg a) { return _mm512_cvtps_epi32(a); }
            static int_reg   castps_si(float_reg a) { return _mm512_castps_si512(a); }
            static float_reg castsi_ps(int_reg a) { return _mm512_castsi512_ps(a); }
            static int       movemask_ps(float_reg a) { return (int)_mm512_cmp_ps_mask(a, _mm512_setzero_ps(), _CMP_NEQ_UQ); }
        };

        // --- Active backend selection ---
#if   defined(ST_AVX_512)
        using active = avx512;
#elif defined(ST_AVX_256)
        using active = avx256;
#else
        using active = sse;          // default fallback
#endif

    } // namespace backend

    // short alias + width constant used throughout the library
    using B = backend::active;
    inline constexpr int simd_width = B::width;

} // namespace st

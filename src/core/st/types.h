#pragma once
#include "backend.h"

namespace st {

    // ------ forward declarations ---------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend> struct int1_t;
    template<typename Backend> struct int2_t;
    template<typename Backend> struct int3_t;
    template<typename Backend> struct int4_t;
    template<typename Backend> struct float1_t;
    template<typename Backend> struct float2_t;
    template<typename Backend> struct float3_t;
    template<typename Backend> struct float4_t;

    // ------ condition tags ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
    struct tag_equal {};
    struct tag_not_equal {};
    struct tag_less {};
    struct tag_less_equal {};
    struct tag_greater {};
    struct tag_greater_equal {};
    struct tag_and {};
    struct tag_or {};
    struct tag_not {};
    struct tag_mask {};  // wraps a raw float1/int1 mask register as a condition

    // forward declaration for float1_t condition constructor (defined in st_math.h after eval())
    namespace detail {
        template<typename B, typename C> typename B::float_reg cond_to_float1_reg(const C& c);
        template<typename B, typename C> typename B::int_reg   cond_to_int1_reg(const C& c);
    }

    // ------ condition<> ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename tag, typename lhs_t, typename rhs_t = void>
    struct condition {
        lhs_t lhs;
        rhs_t rhs;

        // convert to float1/int1: use float1_t(condition) / int1_t(condition) constructors instead.
        // operator bool() is for st::all/st::none scalar tests.
        explicit operator bool() const;

        template<typename tag2, typename l2, typename r2>
        condition<tag_and, condition, condition<tag2, l2, r2>>
            operator&(const condition<tag2, l2, r2>& o) const { return { *this, o }; }

        template<typename tag2, typename l2, typename r2>
        condition<tag_or, condition, condition<tag2, l2, r2>>
            operator|(const condition<tag2, l2, r2>& o) const { return { *this, o }; }

        condition<tag_not, condition> operator~() const { return { *this }; }
    };

    template<typename tag, typename lhs_t>
    struct condition<tag, lhs_t, void> {
        lhs_t lhs;

        explicit operator bool() const;

        template<typename tag2, typename l2, typename r2>
        condition<tag_and, condition, condition<tag2, l2, r2>>
            operator&(const condition<tag2, l2, r2>& o) const { return { *this, o }; }

        template<typename tag2, typename l2, typename r2>
        condition<tag_or, condition, condition<tag2, l2, r2>>
            operator|(const condition<tag2, l2, r2>& o) const { return { *this, o }; }

        condition<tag_not, condition> operator~() const { return { *this }; }
    };

    // as_cond(float1/int1) --- wrap a raw mask register as a lazy condition<tag_mask>
    // eval(condition<tag_mask, float1_t<B>, void>) just returns the raw register directly.
    // This keeps & chains involving ~float1 fully lazy (correct full-mask semantics,
    // no sign-bit-only movemask issues).
    template<typename B>
    inline condition<tag_mask, float1_t<B>, void> as_cond(const float1_t<B>& f) { return { f }; }
    template<typename B>
    inline condition<tag_mask, int1_t<B>, void> as_cond(const int1_t<B>& f) { return { f }; }

    // condition & float1  /  condition | float1  (and symmetric)
    // Wraps float1 as condition<tag_mask> so the result is condition<tag_and,...>
    // which eval/any/all/none handle with full-mask AND, not sign-bit movemask.
    template<typename tag, typename L, typename R, typename B>
    inline auto operator&(const condition<tag, L, R>& c, const float1_t<B>& f)
    {
        return condition<tag_and, condition<tag, L, R>, condition<tag_mask, float1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename R, typename B>
    inline auto operator|(const condition<tag, L, R>& c, const float1_t<B>& f)
    {
        return condition<tag_or, condition<tag, L, R>, condition<tag_mask, float1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename B>
    inline auto operator&(const condition<tag, L, void>& c, const float1_t<B>& f)
    {
        return condition<tag_and, condition<tag, L, void>, condition<tag_mask, float1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename B>
    inline auto operator|(const condition<tag, L, void>& c, const float1_t<B>& f)
    {
        return condition<tag_or, condition<tag, L, void>, condition<tag_mask, float1_t<B>, void>>{ c, { f } };
    }

    template<typename B, typename tag, typename L, typename R>
    inline auto operator&(const float1_t<B>& f, const condition<tag, L, R>& c)
    {
        return condition<tag_and, condition<tag_mask, float1_t<B>, void>, condition<tag, L, R>>{ {f}, c };
    }
    template<typename B, typename tag, typename L, typename R>
    inline auto operator|(const float1_t<B>& f, const condition<tag, L, R>& c)
    {
        return condition<tag_or, condition<tag_mask, float1_t<B>, void>, condition<tag, L, R>>{ {f}, c };
    }
    template<typename B, typename tag, typename L>
    inline auto operator&(const float1_t<B>& f, const condition<tag, L, void>& c)
    {
        return condition<tag_and, condition<tag_mask, float1_t<B>, void>, condition<tag, L, void>>{ {f}, c };
    }
    template<typename B, typename tag, typename L>
    inline auto operator|(const float1_t<B>& f, const condition<tag, L, void>& c)
    {
        return condition<tag_or, condition<tag_mask, float1_t<B>, void>, condition<tag, L, void>>{ {f}, c };
    }

    // condition & int1  /  condition | int1  (and symmetric) --- same pattern as float1
    template<typename tag, typename L, typename R, typename B>
    inline auto operator&(const condition<tag, L, R>& c, const int1_t<B>& f)
    {
        return condition<tag_and, condition<tag, L, R>, condition<tag_mask, int1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename R, typename B>
    inline auto operator|(const condition<tag, L, R>& c, const int1_t<B>& f)
    {
        return condition<tag_or, condition<tag, L, R>, condition<tag_mask, int1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename B>
    inline auto operator&(const condition<tag, L, void>& c, const int1_t<B>& f)
    {
        return condition<tag_and, condition<tag, L, void>, condition<tag_mask, int1_t<B>, void>>{ c, { f } };
    }
    template<typename tag, typename L, typename B>
    inline auto operator|(const condition<tag, L, void>& c, const int1_t<B>& f)
    {
        return condition<tag_or, condition<tag, L, void>, condition<tag_mask, int1_t<B>, void>>{ c, { f } };
    }

    template<typename B, typename tag, typename L, typename R>
    inline auto operator&(const int1_t<B>& f, const condition<tag, L, R>& c)
    {
        return condition<tag_and, condition<tag_mask, int1_t<B>, void>, condition<tag, L, R>>{ {f}, c };
    }
    template<typename B, typename tag, typename L, typename R>
    inline auto operator|(const int1_t<B>& f, const condition<tag, L, R>& c)
    {
        return condition<tag_or, condition<tag_mask, int1_t<B>, void>, condition<tag, L, R>>{ {f}, c };
    }
    template<typename B, typename tag, typename L>
    inline auto operator&(const int1_t<B>& f, const condition<tag, L, void>& c)
    {
        return condition<tag_and, condition<tag_mask, int1_t<B>, void>, condition<tag, L, void>>{ {f}, c };
    }
    template<typename B, typename tag, typename L>
    inline auto operator|(const int1_t<B>& f, const condition<tag, L, void>& c)
    {
        return condition<tag_or, condition<tag_mask, int1_t<B>, void>, condition<tag, L, void>>{ {f}, c };
    }

    template<typename Backend>
    struct alignas(sizeof(typename Backend::int_reg)) int1_t {
        using B = Backend;
        B::int_reg x;

        int1_t() : x(B::setzero_si()) {}
        int1_t(B::int_reg v) : x(v) {}
        int1_t(int v) : x(B::set1_epi32(v)) {}
        int1_t(const float1_t<B>& v) : x(B::cvtps_epi32(v.x)) {}  // float->int (truncate)

        // construct from any condition<> --- B is known from this struct, no ambiguity
        template<typename tag, typename L, typename R>
        int1_t(const condition<tag, L, R>& c) : x(detail::cond_to_int1_reg<B>(c)) {}
        template<typename tag, typename L>
        int1_t(const condition<tag, L, void>& c) : x(detail::cond_to_int1_reg<B>(c)) {}

        // cast to float1_t
        // convert: (float)3    3.0f
        float1_t<B> to_float()      const { return B::cvtepi32_ps(x); }
        // bitcast: reinterpret int bits as float, no conversion
        float1_t<B> to_float_bits() const { return B::castsi_ps(x); }

        explicit operator float1_t<B>() const { return to_float(); }
        explicit int1_t(int a, int b, int c, int d) requires (B::width == 4)
            : x(_mm_setr_epi32(a, b, c, d)) {
        }
        explicit int1_t(int a, int b, int c, int d,
            int e, int f, int g, int h) requires (B::width == 8)
            : x(_mm256_setr_epi32(a, b, c, d, e, f, g, h)) {
        }

        operator B::int_reg()        const { return x; }
        operator B::int_reg& () { return x; }

        int1_t& operator=(int1_t b) { x = b.x; return *this; }
        int1_t& operator=(int v) { x = B::set1_epi32(v); return *this; }

        // arithmetic
        int1_t operator+(int1_t b) const { return B::add_epi32(x, b.x); }
        int1_t operator-(int1_t b) const { return B::sub_epi32(x, b.x); }
        int1_t operator*(int1_t b) const { return B::mul_epi32(x, b.x); }
        int1_t operator/(int1_t b) const {
            // integer divide via float conversion
            auto fa = B::cvtepi32_ps(x);
            auto fb = B::cvtepi32_ps(b.x);
            auto zero_mask = B::cmpeq_epi32(b.x, B::setzero_si());
            auto result = B::cvtps_epi32(B::div_ps(fa, fb));
            return B::andnot_si(zero_mask, result);
        }

        int1_t operator-() const { return B::sub_epi32(B::setzero_si(), x); }
        int1_t operator+() const { return *this; }

        int1_t& operator++() { x = B::add_epi32(x, B::set1_epi32(1)); return *this; }
        int1_t& operator--() { x = B::sub_epi32(x, B::set1_epi32(1)); return *this; }
        int1_t  operator++(int) { int1_t t = *this; ++*this; return t; }
        int1_t  operator--(int) { int1_t t = *this; --*this; return t; }

        int1_t& operator+=(int1_t b) { x = B::add_epi32(x, b.x); return *this; }
        int1_t& operator-=(int1_t b) { x = B::sub_epi32(x, b.x); return *this; }
        int1_t& operator*=(int1_t b) { x = B::mul_epi32(x, b.x); return *this; }
        int1_t& operator/=(int1_t b) { *this = *this / b; return *this; }

        int1_t operator+(int v) const { return *this + int1_t(v); }
        int1_t operator-(int v) const { return *this - int1_t(v); }
        int1_t operator*(int v) const { return *this * int1_t(v); }
        int1_t operator/(int v) const { return *this / int1_t(v); }
        int1_t& operator+=(int v) { return *this += int1_t(v); }
        int1_t& operator-=(int v) { return *this -= int1_t(v); }
        int1_t& operator*=(int v) { return *this *= int1_t(v); }
        int1_t& operator/=(int v) { return *this /= int1_t(v); }

        // bitwise
        int1_t operator&(int1_t b) const { return B::and_si(x, b.x); }
        int1_t operator|(int1_t b) const { return B::or_si(x, b.x); }
        int1_t operator^(int1_t b) const { return B::xor_si(x, b.x); }
        // ~int1 returns condition<tag_not, int1_t> so it stays lazy and chains correctly
        // with other conditions via & |. Use flip_bits() for the raw bitwise NOT int1.
        condition<tag_not, int1_t, void> operator~() const { return { *this }; }
        int1_t flip_bits()         const { return B::xor_si(x, B::set1_epi32(-1)); }
        int1_t operator&(int v)    const { return *this & int1_t(v); }
        int1_t operator|(int v)    const { return *this | int1_t(v); }
        int1_t operator^(int v)    const { return *this ^ int1_t(v); }

        int1_t& operator&=(int1_t b) { x = B::and_si(x, b.x); return *this; }
        int1_t& operator|=(int1_t b) { x = B::or_si(x, b.x);  return *this; }
        int1_t& operator^=(int1_t b) { x = B::xor_si(x, b.x); return *this; }
        int1_t& operator&=(int v) { return *this &= int1_t(v); }
        int1_t& operator|=(int v) { return *this |= int1_t(v); }
        int1_t& operator^=(int v) { return *this ^= int1_t(v); }

        // shifts
        int1_t operator<<(int bits) const { return B::slli_epi32(x, bits); }
        int1_t operator>>(int bits) const { return B::srai_epi32(x, bits); }
        int1_t& operator<<=(int bits) { x = B::slli_epi32(x, bits); return *this; }
        int1_t& operator>>=(int bits) { x = B::srai_epi32(x, bits); return *this; }

        // comparisons --- condition<>
        condition<tag_greater, int1_t, int1_t> operator> (int1_t b) const { return { *this, b }; }
        condition<tag_less, int1_t, int1_t> operator< (int1_t b) const { return { *this, b }; }
        condition<tag_equal, int1_t, int1_t> operator==(int1_t b) const { return { *this, b }; }
        condition<tag_not_equal, int1_t, int1_t> operator!=(int1_t b) const { return { *this, b }; }
        condition<tag_greater_equal, int1_t, int1_t> operator>=(int1_t b) const { return { *this, b }; }
        condition<tag_less_equal, int1_t, int1_t> operator<=(int1_t b) const { return { *this, b }; }

        condition<tag_greater, int1_t, int1_t> operator> (int v) const { return *this > int1_t(v); }
        condition<tag_less, int1_t, int1_t> operator< (int v) const { return *this < int1_t(v); }
        condition<tag_equal, int1_t, int1_t> operator==(int v) const { return *this == int1_t(v); }
        condition<tag_not_equal, int1_t, int1_t> operator!=(int v) const { return *this != int1_t(v); }
        condition<tag_greater_equal, int1_t, int1_t> operator>=(int v) const { return *this >= int1_t(v); }
        condition<tag_less_equal, int1_t, int1_t> operator<=(int v) const { return *this <= int1_t(v); }

        condition<tag_not, condition<tag_greater, int1_t, int1_t>>
            operator!() const { return { *this > int1_t(0) }; }

        // lane access
        int operator[](int i) const {
            alignas(sizeof(typename B::int_reg)) int tmp[B::width];
            if constexpr (B::width == 4) _mm_store_si128((__m128i*)tmp, x);
            else                         _mm256_store_si256((__m256i*)tmp, x);
            return tmp[i];
        }
    };

    template<typename B>
    inline int1_t<B> operator+(int a, int1_t<B> b) { return int1_t<B>(a) + b; }
    template<typename B>
    inline int1_t<B> operator-(int a, int1_t<B> b) { return int1_t<B>(a) - b; }
    template<typename B>
    inline int1_t<B> operator*(int a, int1_t<B> b) { return int1_t<B>(a) * b; }
    template<typename B>
    inline int1_t<B> operator/(int a, int1_t<B> b) { return int1_t<B>(a) / b; }
    template<typename B>
    inline int1_t<B> operator&(int a, int1_t<B> b) { return int1_t<B>(a) & b; }
    template<typename B>
    inline int1_t<B> operator|(int a, int1_t<B> b) { return int1_t<B>(a) | b; }
    template<typename B>
    inline int1_t<B> operator^(int a, int1_t<B> b) { return int1_t<B>(a) ^ b; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float1_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::float_reg)) float1_t {
        using B = Backend;
        B::float_reg x;

        float1_t() : x(B::setzero_ps()) {}
        float1_t(B::float_reg v) : x(v) {}
        float1_t(float v) : x(B::set1_ps(v)) {}
        float1_t(const int1_t<B>& v) : x(B::cvtepi32_ps(v.x)) {}
        float1_t(int v) : x(B::set1_ps((float)v)) {}

        // construct from any condition<> --- B is known from this struct, so no ambiguity
        template<typename tag, typename L, typename R>
        float1_t(const condition<tag, L, R>& c) : float1_t(detail::cond_to_float1_reg<B>(c)) {}
        template<typename tag, typename L>
        float1_t(const condition<tag, L, void>& c) : float1_t(detail::cond_to_float1_reg<B>(c)) {}

        // cast to int1_t
        // truncate: (int)3.7f    3   (same as C cast, uses cvtps)
        int1_t<B> to_int()     const { return B::cvtps_epi32(x); }
        // bitcast: reinterpret float bits as int, no conversion (e.g. for masking)
        int1_t<B> to_int_bits() const { return B::castps_si(x); }

        // explicit cast operators so (int1)f and (int1_t<B>)f work
        explicit operator int1_t<B>() const { return to_int(); }

        explicit float1_t(float a, float b, float c, float d) requires (B::width == 4)
            : x(_mm_setr_ps(a, b, c, d)) {
        }
        explicit float1_t(float a, float b, float c, float d,
            float e, float f, float g, float h) requires (B::width == 8)
            : x(_mm256_setr_ps(a, b, c, d, e, f, g, h)) {
        }

        operator B::float_reg()        const { return x; }
        operator B::float_reg& () { return x; }

        float1_t& operator=(float1_t b) { x = b.x; return *this; }
        float1_t& operator=(float v) { x = B::set1_ps(v); return *this; }
        float1_t& operator=(int v) { x = B::set1_ps((float)v); return *this; }

        // arithmetic
        float1_t operator+(float1_t b) const { return B::add_ps(x, b.x); }
        float1_t operator-(float1_t b) const { return B::sub_ps(x, b.x); }
        float1_t operator*(float1_t b) const { return B::mul_ps(x, b.x); }
        float1_t operator/(float1_t b) const { return B::div_ps(x, b.x); }

        float1_t operator-() const { return B::sub_ps(B::setzero_ps(), x); }
        float1_t operator+() const { return *this; }

        float1_t& operator++() { x = B::add_ps(x, B::set1_ps(1.f)); return *this; }
        float1_t& operator--() { x = B::sub_ps(x, B::set1_ps(1.f)); return *this; }
        float1_t  operator++(int) { float1_t t = *this; ++*this; return t; }
        float1_t  operator--(int) { float1_t t = *this; --*this; return t; }

        float1_t& operator+=(float1_t b) { x = B::add_ps(x, b.x); return *this; }
        float1_t& operator-=(float1_t b) { x = B::sub_ps(x, b.x); return *this; }
        float1_t& operator*=(float1_t b) { x = B::mul_ps(x, b.x); return *this; }
        float1_t& operator/=(float1_t b) { x = B::div_ps(x, b.x); return *this; }

        float1_t operator+(float v)  const { return *this + float1_t(v); }
        float1_t operator-(float v)  const { return *this - float1_t(v); }
        float1_t operator*(float v)  const { return *this * float1_t(v); }
        float1_t operator/(float v)  const { return *this / float1_t(v); }
        float1_t operator+(int v)    const { return *this + float1_t(v); }
        float1_t operator-(int v)    const { return *this - float1_t(v); }
        float1_t operator*(int v)    const { return *this * float1_t(v); }
        float1_t operator/(int v)    const { return *this / float1_t(v); }

        float1_t& operator+=(float v) { return *this += float1_t(v); }
        float1_t& operator-=(float v) { return *this -= float1_t(v); }
        float1_t& operator*=(float v) { return *this *= float1_t(v); }
        float1_t& operator/=(float v) { return *this /= float1_t(v); }
        float1_t& operator+=(int v) { return *this += float1_t(v); }
        float1_t& operator-=(int v) { return *this -= float1_t(v); }
        float1_t& operator*=(int v) { return *this *= float1_t(v); }
        float1_t& operator/=(int v) { return *this /= float1_t(v); }

        // bitwise
        float1_t operator&(float1_t b)      const { return B::and_ps(x, b.x); }
        float1_t operator|(float1_t b)      const { return B::or_ps(x, b.x); }
        float1_t operator^(float1_t b)      const { return B::xor_ps(x, b.x); }
        // ~float1 returns condition<tag_not, float1_t> so it stays lazy and chains with
        // other conditions via & | without losing the full-mask semantics.
        // Use float1_t::flip_bits() if you want the raw bitwise NOT float1 result.
        condition<tag_not, float1_t, void> operator~() const { return { *this }; }
        float1_t flip_bits()                const { return B::xor_ps(x, B::castsi_ps(B::set1_epi32(-1))); }
        float1_t operator&(int1_t<B> b)     const { return B::and_ps(x, B::castsi_ps(b.x)); }
        float1_t operator|(int1_t<B> b)     const { return B::or_ps(x, B::castsi_ps(b.x)); }
        float1_t operator^(int1_t<B> b)     const { return B::xor_ps(x, B::castsi_ps(b.x)); }

        // condition<> & float1_t  --- eval condition then apply bitwise op
        template<typename tag, typename L, typename R>
        float1_t operator&(const condition<tag, L, R>& c) const { return *this & (float1_t)c; }
        template<typename tag, typename L, typename R>
        float1_t operator|(const condition<tag, L, R>& c) const { return *this | (float1_t)c; }
        template<typename tag, typename L>
        float1_t operator&(const condition<tag, L, void>& c) const { return *this & (float1_t)c; }
        template<typename tag, typename L>
        float1_t operator|(const condition<tag, L, void>& c) const { return *this | (float1_t)c; }

        float1_t& operator&=(float1_t b) { x = B::and_ps(x, b.x); return *this; }
        float1_t& operator|=(float1_t b) { x = B::or_ps(x, b.x);  return *this; }
        float1_t& operator^=(float1_t b) { x = B::xor_ps(x, b.x); return *this; }
        float1_t& operator&=(int1_t<B> b) { x = B::and_ps(x, B::castsi_ps(b.x)); return *this; }
        float1_t& operator|=(int1_t<B> b) { x = B::or_ps(x, B::castsi_ps(b.x)); return *this; }
        float1_t& operator^=(int1_t<B> b) { x = B::xor_ps(x, B::castsi_ps(b.x)); return *this; }

        // comparisons --- condition<>
        condition<tag_greater, float1_t, float1_t> operator> (float1_t b) const { return { *this, b }; }
        condition<tag_less, float1_t, float1_t> operator< (float1_t b) const { return { *this, b }; }
        condition<tag_equal, float1_t, float1_t> operator==(float1_t b) const { return { *this, b }; }
        condition<tag_not_equal, float1_t, float1_t> operator!=(float1_t b) const { return { *this, b }; }
        condition<tag_greater_equal, float1_t, float1_t> operator>=(float1_t b) const { return { *this, b }; }
        condition<tag_less_equal, float1_t, float1_t> operator<=(float1_t b) const { return { *this, b }; }

        condition<tag_greater, float1_t, float1_t> operator> (float v) const { return *this > float1_t(v); }
        condition<tag_less, float1_t, float1_t> operator< (float v) const { return *this < float1_t(v); }
        condition<tag_equal, float1_t, float1_t> operator==(float v) const { return *this == float1_t(v); }
        condition<tag_not_equal, float1_t, float1_t> operator!=(float v) const { return *this != float1_t(v); }
        condition<tag_greater_equal, float1_t, float1_t> operator>=(float v) const { return *this >= float1_t(v); }
        condition<tag_less_equal, float1_t, float1_t> operator<=(float v) const { return *this <= float1_t(v); }

        condition<tag_greater, float1_t, float1_t> operator> (int v) const { return *this > float1_t(v); }
        condition<tag_less, float1_t, float1_t> operator< (int v) const { return *this < float1_t(v); }
        condition<tag_equal, float1_t, float1_t> operator==(int v) const { return *this == float1_t(v); }
        condition<tag_not_equal, float1_t, float1_t> operator!=(int v) const { return *this != float1_t(v); }
        condition<tag_greater_equal, float1_t, float1_t> operator>=(int v) const { return *this >= float1_t(v); }
        condition<tag_less_equal, float1_t, float1_t> operator<=(int v) const { return *this <= float1_t(v); }

        condition<tag_not, condition<tag_greater, float1_t, float1_t>>
            operator!() const { return { *this > float1_t(0.f) }; }

        // lane access
        float operator[](int i) const {
            alignas(sizeof(typename B::float_reg)) float tmp[B::width];
            if constexpr (B::width == 4) _mm_store_ps(tmp, x);
            else                         _mm256_store_ps(tmp, x);
            return tmp[i];
        }
    };

    template<typename B>
    inline float1_t<B> operator+(float a, float1_t<B> b) { return float1_t<B>(a) + b; }
    template<typename B>
    inline float1_t<B> operator-(float a, float1_t<B> b) { return float1_t<B>(a) - b; }
    template<typename B>
    inline float1_t<B> operator*(float a, float1_t<B> b) { return float1_t<B>(a) * b; }
    template<typename B>
    inline float1_t<B> operator/(float a, float1_t<B> b) { return float1_t<B>(a) / b; }
    template<typename B>
    inline float1_t<B> operator+(int a, float1_t<B> b) { return float1_t<B>(a) + b; }
    template<typename B>
    inline float1_t<B> operator-(int a, float1_t<B> b) { return float1_t<B>(a) - b; }
    template<typename B>
    inline float1_t<B> operator*(int a, float1_t<B> b) { return float1_t<B>(a) * b; }
    template<typename B>
    inline float1_t<B> operator/(int a, float1_t<B> b) { return float1_t<B>(a) / b; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // int2_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::int_reg)) int2_t {
        using B = Backend;
        int1_t<B> x, y;

        int2_t() : x(0), y(0) {}
        int2_t(int1_t<B> x_) : x(x_), y(x_) {}
        int2_t(int1_t<B> x_, int1_t<B> y_) : x(x_), y(y_) {}
        int2_t(int v) : x(v), y(v) {}
        int2_t(int x_, int y_) : x(x_), y(y_) {}

        int2_t operator+(int2_t b) const { return { x + b.x, y + b.y }; }
        int2_t operator-(int2_t b) const { return { x - b.x, y - b.y }; }
        int2_t operator*(int2_t b) const { return { x * b.x, y * b.y }; }
        int2_t operator/(int2_t b) const { return { x / b.x, y / b.y }; }
        int2_t operator-()         const { return { -x, -y }; }
        int2_t operator+()         const { return *this; }

        int2_t& operator+=(int2_t b) { x += b.x; y += b.y; return *this; }
        int2_t& operator-=(int2_t b) { x -= b.x; y -= b.y; return *this; }
        int2_t& operator*=(int2_t b) { x *= b.x; y *= b.y; return *this; }
        int2_t& operator/=(int2_t b) { x /= b.x; y /= b.y; return *this; }

        int2_t operator+(int v) const { return { x + v, y + v }; }
        int2_t operator-(int v) const { return { x - v, y - v }; }
        int2_t operator*(int v) const { return { x * v, y * v }; }
        int2_t operator/(int v) const { return { x / v, y / v }; }
        int2_t& operator+=(int v) { x += v; y += v; return *this; }
        int2_t& operator-=(int v) { x -= v; y -= v; return *this; }
        int2_t& operator*=(int v) { x *= v; y *= v; return *this; }
        int2_t& operator/=(int v) { x /= v; y /= v; return *this; }

        int2_t operator&(int2_t b) const { return { x & b.x, y & b.y }; }
        int2_t operator|(int2_t b) const { return { x | b.x, y | b.y }; }
        int2_t operator^(int2_t b) const { return { x ^ b.x, y ^ b.y }; }
        int2_t operator~()         const { return { ~x, ~y }; }
        int2_t operator<<(int n)   const { return { x << n, y << n }; }
        int2_t operator>>(int n)   const { return { x >> n, y >> n }; }
    };

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // int3_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::int_reg)) int3_t {
        using B = Backend;
        int1_t<B> x, y, z;

        int3_t() : x(0), y(0), z(0) {}
        int3_t(int1_t<B> x_) : x(x_), y(x_), z(x_) {}
        int3_t(int1_t<B> x_, int1_t<B> y_, int1_t<B> z_) : x(x_), y(y_), z(z_) {}
        int3_t(int v) : x(v), y(v), z(v) {}
        int3_t(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
        int3_t(int2_t<B> xy, int1_t<B> z_) : x(xy.x), y(xy.y), z(z_) {}
        int3_t(int1_t<B> x_, int2_t<B> yz) : x(x_), y(yz.x), z(yz.y) {}

        int3_t operator+(int3_t b) const { return { x + b.x, y + b.y, z + b.z }; }
        int3_t operator-(int3_t b) const { return { x - b.x, y - b.y, z - b.z }; }
        int3_t operator*(int3_t b) const { return { x * b.x, y * b.y, z * b.z }; }
        int3_t operator/(int3_t b) const { return { x / b.x, y / b.y, z / b.z }; }
        int3_t operator-()         const { return { -x, -y, -z }; }
        int3_t operator+()         const { return *this; }

        int3_t& operator+=(int3_t b) { x += b.x; y += b.y; z += b.z; return *this; }
        int3_t& operator-=(int3_t b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
        int3_t& operator*=(int3_t b) { x *= b.x; y *= b.y; z *= b.z; return *this; }
        int3_t& operator/=(int3_t b) { x /= b.x; y /= b.y; z /= b.z; return *this; }

        int3_t operator+(int v) const { return { x + v, y + v, z + v }; }
        int3_t operator-(int v) const { return { x - v, y - v, z - v }; }
        int3_t operator*(int v) const { return { x * v, y * v, z * v }; }
        int3_t operator/(int v) const { return { x / v, y / v, z / v }; }
        int3_t& operator+=(int v) { x += v; y += v; z += v; return *this; }
        int3_t& operator-=(int v) { x -= v; y -= v; z -= v; return *this; }
        int3_t& operator*=(int v) { x *= v; y *= v; z *= v; return *this; }
        int3_t& operator/=(int v) { x /= v; y /= v; z /= v; return *this; }

        int3_t operator&(int3_t b) const { return { x & b.x, y & b.y, z & b.z }; }
        int3_t operator|(int3_t b) const { return { x | b.x, y | b.y, z | b.z }; }
        int3_t operator^(int3_t b) const { return { x ^ b.x, y ^ b.y, z ^ b.z }; }
        int3_t operator~()         const { return { ~x, ~y, ~z }; }
        int3_t operator<<(int n)   const { return { x << n, y << n, z << n }; }
        int3_t operator>>(int n)   const { return { x >> n, y >> n, z >> n }; }

        int1_t<B> dot(int3_t b)   const { return x * b.x + y * b.y + z * b.z; }
        int3_t    cross(int3_t b) const {
            return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x };
        }

        int1_t<B>& operator[](int i) { if (i == 0) return x; if (i == 1) return y; return z; }
        const int1_t<B>& operator[](int i) const { if (i == 0) return x; if (i == 1) return y; return z; }
    };

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // int4_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::int_reg)) int4_t {
        using B = Backend;
        int1_t<B> x, y, z, w;

        int4_t() : x(0), y(0), z(0), w(0) {}
        int4_t(int1_t<B> x_) : x(x_), y(x_), z(x_), w(x_) {}
        int4_t(int1_t<B> x_, int1_t<B> y_, int1_t<B> z_, int1_t<B> w_) : x(x_), y(y_), z(z_), w(w_) {}
        int4_t(int v) : x(v), y(v), z(v), w(v) {}
        int4_t(int x_, int y_, int z_, int w_) : x(x_), y(y_), z(z_), w(w_) {}
        int4_t(int2_t<B> xy, int2_t<B> zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
        int4_t(int3_t<B> xyz, int1_t<B> w_) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_) {}
        int4_t(int1_t<B> x_, int3_t<B> yzw) : x(x_), y(yzw.x), z(yzw.y), w(yzw.z) {}

        int4_t operator+(int4_t b) const { return { x + b.x, y + b.y, z + b.z, w + b.w }; }
        int4_t operator-(int4_t b) const { return { x - b.x, y - b.y, z - b.z, w - b.w }; }
        int4_t operator*(int4_t b) const { return { x * b.x, y * b.y, z * b.z, w * b.w }; }
        int4_t operator/(int4_t b) const { return { x / b.x, y / b.y, z / b.z, w / b.w }; }
        int4_t operator-()         const { return { -x, -y, -z, -w }; }
        int4_t operator+()         const { return *this; }

        int4_t& operator+=(int4_t b) { x += b.x; y += b.y; z += b.z; w += b.w; return *this; }
        int4_t& operator-=(int4_t b) { x -= b.x; y -= b.y; z -= b.z; w -= b.w; return *this; }
        int4_t& operator*=(int4_t b) { x *= b.x; y *= b.y; z *= b.z; w *= b.w; return *this; }
        int4_t& operator/=(int4_t b) { x /= b.x; y /= b.y; z /= b.z; w /= b.w; return *this; }

        int4_t operator+(int v) const { return { x + v, y + v, z + v, w + v }; }
        int4_t operator-(int v) const { return { x - v, y - v, z - v, w - v }; }
        int4_t operator*(int v) const { return { x * v, y * v, z * v, w * v }; }
        int4_t operator/(int v) const { return { x / v, y / v, z / v, w / v }; }
        int4_t& operator+=(int v) { x += v; y += v; z += v; w += v; return *this; }
        int4_t& operator-=(int v) { x -= v; y -= v; z -= v; w -= v; return *this; }
        int4_t& operator*=(int v) { x *= v; y *= v; z *= v; w *= v; return *this; }
        int4_t& operator/=(int v) { x /= v; y /= v; z /= v; w /= v; return *this; }

        int4_t operator&(int4_t b) const { return { x & b.x, y & b.y, z & b.z, w & b.w }; }
        int4_t operator|(int4_t b) const { return { x | b.x, y | b.y, z | b.z, w | b.w }; }
        int4_t operator^(int4_t b) const { return { x ^ b.x, y ^ b.y, z ^ b.z, w ^ b.w }; }
        int4_t operator~()         const { return { ~x, ~y, ~z, ~w }; }
        int4_t operator<<(int n)   const { return { x << n, y << n, z << n, w << n }; }
        int4_t operator>>(int n)   const { return { x >> n, y >> n, z >> n, w >> n }; }

        int1_t<B>& operator[](int i) { if (i == 0) return x; if (i == 1) return y; if (i == 2) return z; return w; }
        const int1_t<B>& operator[](int i) const { if (i == 0) return x; if (i == 1) return y; if (i == 2) return z; return w; }
    };

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float2_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::float_reg)) float2_t {
        using B = Backend;
        float1_t<B> x, y;

        float2_t() : x(0.f), y(0.f) {}
        float2_t(float1_t<B> x_) : x(x_), y(x_) {}
        float2_t(float1_t<B> x_, float1_t<B> y_) : x(x_), y(y_) {}
        float2_t(float v) : x(v), y(v) {}
        float2_t(float x_, float y_) : x(x_), y(y_) {}
        explicit float2_t(int v) : x((float)v), y((float)v) {}
        explicit float2_t(int x_, int y_) : x((float)x_), y((float)y_) {}

        float2_t operator+(float2_t b) const { return { x + b.x, y + b.y }; }
        float2_t operator-(float2_t b) const { return { x - b.x, y - b.y }; }
        float2_t operator*(float2_t b) const { return { x * b.x, y * b.y }; }
        float2_t operator/(float2_t b) const { return { x / b.x, y / b.y }; }
        float2_t operator-()           const { return { -x, -y }; }
        float2_t operator+()           const { return *this; }

        float2_t& operator+=(float2_t b) { x += b.x; y += b.y; return *this; }
        float2_t& operator-=(float2_t b) { x -= b.x; y -= b.y; return *this; }
        float2_t& operator*=(float2_t b) { x *= b.x; y *= b.y; return *this; }
        float2_t& operator/=(float2_t b) { x /= b.x; y /= b.y; return *this; }

        float2_t operator+(float v) const { return { x + v, y + v }; }
        float2_t operator-(float v) const { return { x - v, y - v }; }
        float2_t operator*(float v) const { return { x * v, y * v }; }
        float2_t operator/(float v) const { return { x / v, y / v }; }
        float2_t operator+(int v)   const { return { x + (float)v, y + (float)v }; }
        float2_t operator-(int v)   const { return { x - (float)v, y - (float)v }; }
        float2_t operator*(int v)   const { return { x * (float)v, y * (float)v }; }
        float2_t operator/(int v)   const { return { x / (float)v, y / (float)v }; }

        float2_t& operator+=(float v) { x += v; y += v; return *this; }
        float2_t& operator-=(float v) { x -= v; y -= v; return *this; }
        float2_t& operator*=(float v) { x *= v; y *= v; return *this; }
        float2_t& operator/=(float v) { x /= v; y /= v; return *this; }
        float2_t& operator+=(int v) { x += (float)v; y += (float)v; return *this; }
        float2_t& operator-=(int v) { x -= (float)v; y -= (float)v; return *this; }
        float2_t& operator*=(int v) { x *= (float)v; y *= (float)v; return *this; }
        float2_t& operator/=(int v) { x /= (float)v; y /= (float)v; return *this; }
    };

    template<typename B> inline float2_t<B> operator+(float a, float2_t<B> b) { return float2_t<B>(a) + b; }
    template<typename B> inline float2_t<B> operator-(float a, float2_t<B> b) { return float2_t<B>(a) - b; }
    template<typename B> inline float2_t<B> operator*(float a, float2_t<B> b) { return float2_t<B>(a) * b; }
    template<typename B> inline float2_t<B> operator/(float a, float2_t<B> b) { return float2_t<B>(a) / b; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float3_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::float_reg)) float3_t {
        using B = Backend;
        float1_t<B> x, y, z;

        float3_t() : x(0.f), y(0.f), z(0.f) {}
        float3_t(float1_t<B> x_) : x(x_), y(x_), z(x_) {}
        float3_t(float1_t<B> x_, float1_t<B> y_, float1_t<B> z_) : x(x_), y(y_), z(z_) {}
        float3_t(float v) : x(v), y(v), z(v) {}
        float3_t(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        float3_t(int v) : x((float)v), y((float)v), z((float)v) {}
        float3_t(float2_t<B> xy, float1_t<B> z_) : x(xy.x), y(xy.y), z(z_) {}
        float3_t(float1_t<B> x_, float2_t<B> yz) : x(x_), y(yz.x), z(yz.y) {}

        float3_t operator+(float3_t b) const { return { x + b.x, y + b.y, z + b.z }; }
        float3_t operator-(float3_t b) const { return { x - b.x, y - b.y, z - b.z }; }
        float3_t operator*(float3_t b) const { return { x * b.x, y * b.y, z * b.z }; }
        float3_t operator/(float3_t b) const { return { x / b.x, y / b.y, z / b.z }; }
        float3_t operator-()           const { return { -x, -y, -z }; }
        float3_t operator+()           const { return *this; }

        float3_t& operator+=(float3_t b) { x += b.x; y += b.y; z += b.z; return *this; }
        float3_t& operator-=(float3_t b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
        float3_t& operator*=(float3_t b) { x *= b.x; y *= b.y; z *= b.z; return *this; }
        float3_t& operator/=(float3_t b) { x /= b.x; y /= b.y; z /= b.z; return *this; }

        float3_t operator+(float v) const { return { x + v, y + v, z + v }; }
        float3_t operator-(float v) const { return { x - v, y - v, z - v }; }
        float3_t operator*(float v) const { return { x * v, y * v, z * v }; }
        float3_t operator/(float v) const { return { x / v, y / v, z / v }; }
        float3_t operator+(int v)   const { return { x + (float)v, y + (float)v, z + (float)v }; }
        float3_t operator-(int v)   const { return { x - (float)v, y - (float)v, z - (float)v }; }
        float3_t operator*(int v)   const { return { x * (float)v, y * (float)v, z * (float)v }; }
        float3_t operator/(int v)   const { return { x / (float)v, y / (float)v, z / (float)v }; }

        float3_t& operator+=(float v) { x += v; y += v; z += v; return *this; }
        float3_t& operator-=(float v) { x -= v; y -= v; z -= v; return *this; }
        float3_t& operator*=(float v) { x *= v; y *= v; z *= v; return *this; }
        float3_t& operator/=(float v) { x /= v; y /= v; z /= v; return *this; }
        float3_t& operator+=(int v) { x += (float)v; y += (float)v; z += (float)v; return *this; }
        float3_t& operator-=(int v) { x -= (float)v; y -= (float)v; z -= (float)v; return *this; }
        float3_t& operator*=(int v) { x *= (float)v; y *= (float)v; z *= (float)v; return *this; }
        float3_t& operator/=(int v) { x /= (float)v; y /= (float)v; z /= (float)v; return *this; }
    };

    template<typename B> inline float3_t<B> operator+(float a, float3_t<B> b) { return float3_t<B>(a) + b; }
    template<typename B> inline float3_t<B> operator-(float a, float3_t<B> b) { return float3_t<B>(a) - b; }
    template<typename B> inline float3_t<B> operator*(float a, float3_t<B> b) { return float3_t<B>(a) * b; }
    template<typename B> inline float3_t<B> operator/(float a, float3_t<B> b) { return float3_t<B>(a) / b; }
    template<typename B> inline float3_t<B> operator+(int a, float3_t<B> b) { return float3_t<B>((float)a) + b; }
    template<typename B> inline float3_t<B> operator-(int a, float3_t<B> b) { return float3_t<B>((float)a) - b; }
    template<typename B> inline float3_t<B> operator*(int a, float3_t<B> b) { return float3_t<B>((float)a) * b; }
    template<typename B> inline float3_t<B> operator/(int a, float3_t<B> b) { return float3_t<B>((float)a) / b; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float4_t
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename Backend>
    struct alignas(sizeof(typename Backend::float_reg)) float4_t {
        using B = Backend;
        float1_t<B> x, y, z, w;

        float4_t() : x(0.f), y(0.f), z(0.f), w(0.f) {}
        float4_t(float1_t<B> x_) : x(x_), y(x_), z(x_), w(x_) {}
        float4_t(float1_t<B> x_, float1_t<B> y_, float1_t<B> z_, float1_t<B> w_)
            : x(x_), y(y_), z(z_), w(w_) {
        }
        float4_t(float v) : x(v), y(v), z(v), w(v) {}
        float4_t(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
        float4_t(int v) : x((float)v), y((float)v), z((float)v), w((float)v) {}
        float4_t(float2_t<B> xy, float2_t<B> zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
        float4_t(float2_t<B> xy, float1_t<B> z_, float1_t<B> w_) : x(xy.x), y(xy.y), z(z_), w(w_) {}
        float4_t(float3_t<B> xyz, float1_t<B> w_) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_) {}
        float4_t(float1_t<B> x_, float3_t<B> yzw) : x(x_), y(yzw.x), z(yzw.y), w(yzw.z) {}

        float4_t operator+(float4_t b) const { return { x + b.x, y + b.y, z + b.z, w + b.w }; }
        float4_t operator-(float4_t b) const { return { x - b.x, y - b.y, z - b.z, w - b.w }; }
        float4_t operator*(float4_t b) const { return { x * b.x, y * b.y, z * b.z, w * b.w }; }
        float4_t operator/(float4_t b) const { return { x / b.x, y / b.y, z / b.z, w / b.w }; }
        float4_t operator-()           const { return { -x, -y, -z, -w }; }
        float4_t operator+()           const { return *this; }

        float4_t& operator+=(float4_t b) { x += b.x; y += b.y; z += b.z; w += b.w; return *this; }
        float4_t& operator-=(float4_t b) { x -= b.x; y -= b.y; z -= b.z; w -= b.w; return *this; }
        float4_t& operator*=(float4_t b) { x *= b.x; y *= b.y; z *= b.z; w *= b.w; return *this; }
        float4_t& operator/=(float4_t b) { x /= b.x; y /= b.y; z /= b.z; w /= b.w; return *this; }

        float4_t operator+(float v) const { return { x + v, y + v, z + v, w + v }; }
        float4_t operator-(float v) const { return { x - v, y - v, z - v, w - v }; }
        float4_t operator*(float v) const { return { x * v, y * v, z * v, w * v }; }
        float4_t operator/(float v) const { return { x / v, y / v, z / v, w / v }; }
        float4_t operator+(int v)   const { return { x + (float)v, y + (float)v, z + (float)v, w + (float)v }; }
        float4_t operator-(int v)   const { return { x - (float)v, y - (float)v, z - (float)v, w - (float)v }; }
        float4_t operator*(int v)   const { return { x * (float)v, y * (float)v, z * (float)v, w * (float)v }; }
        float4_t operator/(int v)   const { return { x / (float)v, y / (float)v, z / (float)v, w / (float)v }; }

        float4_t& operator+=(float v) { x += v; y += v; z += v; w += v; return *this; }
        float4_t& operator-=(float v) { x -= v; y -= v; z -= v; w -= v; return *this; }
        float4_t& operator*=(float v) { x *= v; y *= v; z *= v; w *= v; return *this; }
        float4_t& operator/=(float v) { x /= v; y /= v; z /= v; w /= v; return *this; }
        float4_t& operator+=(int v) { x += (float)v; y += (float)v; z += (float)v; w += (float)v; return *this; }
        float4_t& operator-=(int v) { x -= (float)v; y -= (float)v; z -= (float)v; w -= (float)v; return *this; }
        float4_t& operator*=(int v) { x *= (float)v; y *= (float)v; z *= (float)v; w *= (float)v; return *this; }
        float4_t& operator/=(int v) { x /= (float)v; y /= (float)v; z /= (float)v; w /= (float)v; return *this; }
    };

    template<typename B> inline float4_t<B> operator+(float a, float4_t<B> b) { return float4_t<B>(a) + b; }
    template<typename B> inline float4_t<B> operator-(float a, float4_t<B> b) { return float4_t<B>(a) - b; }
    template<typename B> inline float4_t<B> operator*(float a, float4_t<B> b) { return float4_t<B>(a) * b; }
    template<typename B> inline float4_t<B> operator/(float a, float4_t<B> b) { return float4_t<B>(a) / b; }
    template<typename B> inline float4_t<B> operator+(int a, float4_t<B> b) { return float4_t<B>((float)a) + b; }
    template<typename B> inline float4_t<B> operator-(int a, float4_t<B> b) { return float4_t<B>((float)a) - b; }
    template<typename B> inline float4_t<B> operator*(int a, float4_t<B> b) { return float4_t<B>((float)a) * b; }
    template<typename B> inline float4_t<B> operator/(int a, float4_t<B> b) { return float4_t<B>((float)a) / b; }

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // float4x4  --- column-major 4x4 float matrix, layout-compatible with glm::mat4
    //
    // Accepts any external float4x4-like type via from():
    //   st::float4x4 m = st::float4x4::from(glm_mat);  // no glm header needed here
    //   st::float4x4 m = st::float4x4::from(my_mat);     // works for any column-major float[16]
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    struct float4x4 {
        // column-major: col[0] = first column (compatible with glm::mat4, glm::value_ptr)
        float col[4][4];

        // identity
        float4x4() {
            for (int c = 0; c < 4; ++c)
                for (int r = 0; r < 4; ++r)
                    col[c][r] = (c == r) ? 1.f : 0.f;
        }

        // construct from raw float*  use glm::value_ptr(mat) or any column-major float[16]
        float4x4(const float* p) {
            for (int i = 0; i < 16; ++i)
                (&col[0][0])[i] = p[i];
        }

        const float* data() const { return &col[0][0]; }
        float* data() { return &col[0][0]; }

        float  operator()(int r, int c) const { return col[c][r]; }
        float& operator()(int r, int c) { return col[c][r]; }

        struct scalar_float4 { float x, y, z, w; };
        scalar_float4 column(int c) const {
            return { col[c][0], col[c][1], col[c][2], col[c][3] };
        }
    };


    using float1 = float1_t<backend::active>;
    using float2 = float2_t<backend::active>;
    using float3 = float3_t<backend::active>;
    using float4 = float4_t<backend::active>;
    using int1 = int1_t<backend::active>;
    using int2 = int2_t<backend::active>;
    using int3 = int3_t<backend::active>;
    using int4 = int4_t<backend::active>;

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // explicit backend aliases  (always available)
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    namespace sse {
        using float1 = float1_t<backend::sse>;
        using float2 = float2_t<backend::sse>;
        using float3 = float3_t<backend::sse>;
        using float4 = float4_t<backend::sse>;
        using int1 = int1_t<backend::sse>;
        using int2 = int2_t<backend::sse>;
        using int3 = int3_t<backend::sse>;
        using int4 = int4_t<backend::sse>;
    }
    namespace avx {
        using float1 = float1_t<backend::avx256>;
        using float2 = float2_t<backend::avx256>;
        using float3 = float3_t<backend::avx256>;
        using float4 = float4_t<backend::avx256>;
        using int1 = int1_t<backend::avx256>;
        using int2 = int2_t<backend::avx256>;
        using int3 = int3_t<backend::avx256>;
        using int4 = int4_t<backend::avx256>;
    }

} // namespace st
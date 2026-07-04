#pragma once
#include "types.h"

namespace st {

    // ------ float2 swizzles ------------------------------------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float2_t<B> xx(const float2_t<B>& v) { return { v.x, v.x }; }
    template<typename B> inline float2_t<B> xy(const float2_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline float2_t<B> yx(const float2_t<B>& v) { return { v.y, v.x }; }
    template<typename B> inline float2_t<B> yy(const float2_t<B>& v) { return { v.y, v.y }; }

    // ------ float3 --- float2 swizzles ---------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float2_t<B> xx(const float3_t<B>& v) { return { v.x, v.x }; }
    template<typename B> inline float2_t<B> xy(const float3_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline float2_t<B> xz(const float3_t<B>& v) { return { v.x, v.z }; }
    template<typename B> inline float2_t<B> yx(const float3_t<B>& v) { return { v.y, v.x }; }
    template<typename B> inline float2_t<B> yy(const float3_t<B>& v) { return { v.y, v.y }; }
    template<typename B> inline float2_t<B> yz(const float3_t<B>& v) { return { v.y, v.z }; }
    template<typename B> inline float2_t<B> zx(const float3_t<B>& v) { return { v.z, v.x }; }
    template<typename B> inline float2_t<B> zy(const float3_t<B>& v) { return { v.z, v.y }; }
    template<typename B> inline float2_t<B> zz(const float3_t<B>& v) { return { v.z, v.z }; }

    // ------ float3 --- float3 swizzles ---------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float3_t<B> xxx(const float3_t<B>& v) { return { v.x, v.x, v.x }; }
    template<typename B> inline float3_t<B> xxy(const float3_t<B>& v) { return { v.x, v.x, v.y }; }
    template<typename B> inline float3_t<B> xxz(const float3_t<B>& v) { return { v.x, v.x, v.z }; }
    template<typename B> inline float3_t<B> xyx(const float3_t<B>& v) { return { v.x, v.y, v.x }; }
    template<typename B> inline float3_t<B> xyy(const float3_t<B>& v) { return { v.x, v.y, v.y }; }
    template<typename B> inline float3_t<B> xyz(const float3_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline float3_t<B> xzx(const float3_t<B>& v) { return { v.x, v.z, v.x }; }
    template<typename B> inline float3_t<B> xzy(const float3_t<B>& v) { return { v.x, v.z, v.y }; }
    template<typename B> inline float3_t<B> xzz(const float3_t<B>& v) { return { v.x, v.z, v.z }; }
    template<typename B> inline float3_t<B> yxx(const float3_t<B>& v) { return { v.y, v.x, v.x }; }
    template<typename B> inline float3_t<B> yxy(const float3_t<B>& v) { return { v.y, v.x, v.y }; }
    template<typename B> inline float3_t<B> yxz(const float3_t<B>& v) { return { v.y, v.x, v.z }; }
    template<typename B> inline float3_t<B> yyx(const float3_t<B>& v) { return { v.y, v.y, v.x }; }
    template<typename B> inline float3_t<B> yyy(const float3_t<B>& v) { return { v.y, v.y, v.y }; }
    template<typename B> inline float3_t<B> yyz(const float3_t<B>& v) { return { v.y, v.y, v.z }; }
    template<typename B> inline float3_t<B> yzx(const float3_t<B>& v) { return { v.y, v.z, v.x }; }
    template<typename B> inline float3_t<B> yzy(const float3_t<B>& v) { return { v.y, v.z, v.y }; }
    template<typename B> inline float3_t<B> yzz(const float3_t<B>& v) { return { v.y, v.z, v.z }; }
    template<typename B> inline float3_t<B> zxx(const float3_t<B>& v) { return { v.z, v.x, v.x }; }
    template<typename B> inline float3_t<B> zxy(const float3_t<B>& v) { return { v.z, v.x, v.y }; }
    template<typename B> inline float3_t<B> zxz(const float3_t<B>& v) { return { v.z, v.x, v.z }; }
    template<typename B> inline float3_t<B> zyx(const float3_t<B>& v) { return { v.z, v.y, v.x }; }
    template<typename B> inline float3_t<B> zyy(const float3_t<B>& v) { return { v.z, v.y, v.y }; }
    template<typename B> inline float3_t<B> zyz(const float3_t<B>& v) { return { v.z, v.y, v.z }; }
    template<typename B> inline float3_t<B> zzx(const float3_t<B>& v) { return { v.z, v.z, v.x }; }
    template<typename B> inline float3_t<B> zzy(const float3_t<B>& v) { return { v.z, v.z, v.y }; }
    template<typename B> inline float3_t<B> zzz(const float3_t<B>& v) { return { v.z, v.z, v.z }; }

    // ------ float4 --- float2 swizzles ---------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float2_t<B> xx(const float4_t<B>& v) { return { v.x, v.x }; }
    template<typename B> inline float2_t<B> xy(const float4_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline float2_t<B> xz(const float4_t<B>& v) { return { v.x, v.z }; }
    template<typename B> inline float2_t<B> xw(const float4_t<B>& v) { return { v.x, v.w }; }
    template<typename B> inline float2_t<B> yx(const float4_t<B>& v) { return { v.y, v.x }; }
    template<typename B> inline float2_t<B> yy(const float4_t<B>& v) { return { v.y, v.y }; }
    template<typename B> inline float2_t<B> yz(const float4_t<B>& v) { return { v.y, v.z }; }
    template<typename B> inline float2_t<B> yw(const float4_t<B>& v) { return { v.y, v.w }; }
    template<typename B> inline float2_t<B> zx(const float4_t<B>& v) { return { v.z, v.x }; }
    template<typename B> inline float2_t<B> zy(const float4_t<B>& v) { return { v.z, v.y }; }
    template<typename B> inline float2_t<B> zz(const float4_t<B>& v) { return { v.z, v.z }; }
    template<typename B> inline float2_t<B> zw(const float4_t<B>& v) { return { v.z, v.w }; }
    template<typename B> inline float2_t<B> wx(const float4_t<B>& v) { return { v.w, v.x }; }
    template<typename B> inline float2_t<B> wy(const float4_t<B>& v) { return { v.w, v.y }; }
    template<typename B> inline float2_t<B> wz(const float4_t<B>& v) { return { v.w, v.z }; }
    template<typename B> inline float2_t<B> ww(const float4_t<B>& v) { return { v.w, v.w }; }

    // ------ float4 --- float3 swizzles ---------------------------------------------------------------------------------------------------------------------------------------
    template<typename B> inline float3_t<B> xxx(const float4_t<B>& v) { return { v.x, v.x, v.x }; }
    template<typename B> inline float3_t<B> xxy(const float4_t<B>& v) { return { v.x, v.x, v.y }; }
    template<typename B> inline float3_t<B> xyz(const float4_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline float3_t<B> xyw(const float4_t<B>& v) { return { v.x, v.y, v.w }; }
    template<typename B> inline float3_t<B> xzw(const float4_t<B>& v) { return { v.x, v.z, v.w }; }
    template<typename B> inline float3_t<B> yzw(const float4_t<B>& v) { return { v.y, v.z, v.w }; }
    template<typename B> inline float3_t<B> yyy(const float4_t<B>& v) { return { v.y, v.y, v.y }; }
    template<typename B> inline float3_t<B> zzz(const float4_t<B>& v) { return { v.z, v.z, v.z }; }
    template<typename B> inline float3_t<B> www(const float4_t<B>& v) { return { v.w, v.w, v.w }; }

    // ------ float4 --- float4 swizzles (common ones) ---------------------------------------------------------------------------------------------
    template<typename B> inline float4_t<B> xyzw(const float4_t<B>& v) { return { v.x, v.y, v.z, v.w }; }
    template<typename B> inline float4_t<B> xxxx(const float4_t<B>& v) { return { v.x, v.x, v.x, v.x }; }
    template<typename B> inline float4_t<B> yyyy(const float4_t<B>& v) { return { v.y, v.y, v.y, v.y }; }
    template<typename B> inline float4_t<B> zzzz(const float4_t<B>& v) { return { v.z, v.z, v.z, v.z }; }
    template<typename B> inline float4_t<B> wwww(const float4_t<B>& v) { return { v.w, v.w, v.w, v.w }; }
    template<typename B> inline float4_t<B> wzyx(const float4_t<B>& v) { return { v.w, v.z, v.y, v.x }; }
    template<typename B> inline float4_t<B> xyxy(const float4_t<B>& v) { return { v.x, v.y, v.x, v.y }; }
    template<typename B> inline float4_t<B> zwzw(const float4_t<B>& v) { return { v.z, v.w, v.z, v.w }; }

    // ------ int swizzles follow exact same pattern ---------------------------------------------------------------------------------------------
    template<typename B> inline int2_t<B> xx(const int2_t<B>& v) { return { v.x, v.x }; }
    template<typename B> inline int2_t<B> xy(const int2_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline int2_t<B> yx(const int2_t<B>& v) { return { v.y, v.x }; }
    template<typename B> inline int2_t<B> yy(const int2_t<B>& v) { return { v.y, v.y }; }

    template<typename B> inline int2_t<B> xy(const int3_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline int2_t<B> xz(const int3_t<B>& v) { return { v.x, v.z }; }
    template<typename B> inline int2_t<B> yz(const int3_t<B>& v) { return { v.y, v.z }; }
    template<typename B> inline int3_t<B> xyz(const int3_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline int3_t<B> zyx(const int3_t<B>& v) { return { v.z, v.y, v.x }; }
    template<typename B> inline int3_t<B> xzy(const int3_t<B>& v) { return { v.x, v.z, v.y }; }

    template<typename B> inline int2_t<B> xy(const int4_t<B>& v) { return { v.x, v.y }; }
    template<typename B> inline int2_t<B> zw(const int4_t<B>& v) { return { v.z, v.w }; }
    template<typename B> inline int3_t<B> xyz(const int4_t<B>& v) { return { v.x, v.y, v.z }; }
    template<typename B> inline int4_t<B> xyzw(const int4_t<B>& v) { return { v.x, v.y, v.z, v.w }; }
    template<typename B> inline int4_t<B> wzyx(const int4_t<B>& v) { return { v.w, v.z, v.y, v.x }; }

} // namespace st
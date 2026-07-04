#pragma once

// Set ONE of these before including, or pass via compiler flag e.g. -DST_AVX_256
//
//   #define ST_AVX_512    ->  st::float3 uses AVX-512  (16-wide)
//   #define ST_AVX_256    ->  st::float3 uses AVX-256  (8-wide)
//   (nothing)             ->  st::float3 uses SSE      (4-wide)
//
// Explicit-width types are always available regardless of the define:
//   st::sse::float3       ->  always SSE  4-wide
//   st::avx::float3       ->  always AVX  8-wide

#include "backend.h"
#include "types.h"
#include "math.h"
#include "swizzle.h"

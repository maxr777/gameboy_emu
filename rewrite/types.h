#ifndef TYPES_H
#define TYPES_H

#include <assert.h>
#include <stdint.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef float f32;
static_assert(sizeof(f32) == 4, "f32 must be 32-bit");

typedef double f64;
static_assert(sizeof(f64) == 8, "f64 must be 64-bit");

#endif

#ifndef BASE_H
#define BASE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// =============================
// >> STANDARD TYPE SIZE

#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef s32 b32;

typedef float f32;
typedef double f64;


// =============================
// >> USEFUL MACROS

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

// These are actually KiB, MiB,... but everyone knows that it is a power of two.
#define KB(n) ((n) << 10)
#define MB(n) ((n) << 20)
#define GB(n) ((u64)(n) << 30)

#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }


// =============================
// >> USEFUL STRUCTS

struct String {
    char *str;
    u32 size;
};

#endif

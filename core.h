#include <stdint.h>
#include <stddef.h>

#ifndef nil
#define nil (void *)0
#endif

typedef int8_t	i8;
typedef uint8_t	u8;
typedef uint8_t	byte;

typedef int16_t	i16;
typedef uint16_t	u16;

typedef int32_t	i32;
typedef uint32_t	u32;
typedef u32 b32;

typedef int64_t	i64;
typedef uint64_t	u64;

typedef float	f32;
typedef double f64;

typedef uintptr_t	uptr;
typedef size_t usize;

typedef struct string8 string8;
struct string8 {
  u8* base;
  u64 size;
};

u32 swap_bytes_u32(u32 n);
u16 swap_bytes_u16(u16 n);
u32 gcd_u32(u32 x, u32 y);

#define unused(x) (void)(x)
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#define STR8_LIT(s) (string8) { (u8*)(s), sizeof((s)) - 1 }

/* a runtime endian check */
const union {
    long l;
    char c;
} endian = { 1 };
#define IS_BE() (endian.c == 0)

#define swap_ints(i1, i2) do {  \
    i1 = i1 ^ i2;               \
    i2 = i2 ^ i1;               \
    i1 = i1 ^ i2;               \
} while (0)


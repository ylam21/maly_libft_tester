/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef BASE_CORE_H
#define BASE_CORE_H

////////////////////////////////
// Foreign Includes
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////
// Codebase Keywords
#define internal_function  static
#define local_persist      static
#define global             static
#define thread_static      __thread

#if   COMPILER_CLANG && OS_LINUX
    #define read_only __attribute__((section(".rodata")))
#elif COMPILER_CLANG && OS_MAC
    #define read_only __attribute__((section("__DATA,__const")))
#else
    #define read_only
#endif

////////////////////////////////
// Base Data Types
typedef uint8_t     U8;
typedef uint16_t    U16;
typedef uint32_t    U32;
typedef uint64_t    U64;
typedef int8_t      S8;
typedef int16_t     S16;
typedef int32_t     S32;
typedef int64_t     S64;
typedef float       F32;
typedef double      F64;

////////////////////////////////
// Units
#define KiB(n)         (((U64)(n)) << 10)
#define MiB(n)         (((U64)(n)) << 20)
#define GiB(n)         (((U64)(n)) << 30)
#define TiB(n)         (((U64)(n)) << 40)
#define Thousand(n)    ((n)*1000)
#define Million(n)     ((n)*1000000)
#define Billion(n)     ((n)*1000000000)

////////////////////////////////
// Mins, Maxes
#define Min(A,B) (((A)<(B))?(A):(B))
#define MinimumBetween(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define MaximumBetween(A,B) (((A)>(B))?(A):(B))

////////////////////////////////
// Type -> Alignment
#define AlignmentOf(type)    __alignof__(type)
#define AlignType(x)         __attribute__((aligned(x)))

////////////////////////////////
// Memory Operation Macros
#define MemoryCopy(dest, source, size)      memmove((dest), (source), (size))
#define MemorySet(dest, byte, size)         memset((dest), (byte), (size))
#define MemoryCompare(a, b, size)           memcmp((a), (b), (size))

#define MemoryCopyStruct(dest, source)      MemoryCopy((dest),(source),sizeof(*(dest)))
#define MemoryCopyStaticArray(dest, source) MemoryCopy((dest),(source),sizeof(dest))
#define MemoryCopyString8(dest, source)     MemoryCopy(dest, (source).str, (source).size)

#define MemoryZero(dest, size)              memset((dest),0,(size))
#define MemoryZeroStruct(dest)              MemoryZero((dest),sizeof(*(dest)))
#define MemoryZeroStaticArray(array)        MemoryZero((array),sizeof(array))

#define MemoryMatch(a,b,size)              (MemoryCompare((a),(b),(size)) == 0)
#define MemoryMatchStruct(a,b)              MemoryMatch((a),(b),sizeof(*(a)))
#define MemoryMatchStaticArray(a,b)         MemoryMatch((a),(b),sizeof(a))

////////////////////////////////
// Asserts
#define Trap() __builtin_trap()
#define AssertAlways(condition) do{if(!(condition)) {Trap();}}while(0)
#define StaticAssert(condition, ID) global U8 Glue(ID, __LINE__)[(condition) ? 1 : -1]

#if BUILD_DEBUG
# define Assert(condition) AssertAlways(condition)
#else
# define Assert(condition) (void)(condition)
#endif // BUILD_DEBUG

////////////////////////////////
// Helper Macros
#define CountOfStaticArray(array) (sizeof(array)/sizeof(array[0]))

#define AlignPow2(x, align)     (((x) + (align) - 1)&(~((align) - 1)))
#define IsPow2(x)               ((x)!=0 && ((x)&((x)-1))==0)
#define IsPow2OrZero(x)         ((((x) - 1)&(x)) == 0)

#define Glue_(A,B) A##B
#define Glue(A,B)  Glue_(A,B)

////////////////////////////////
// Basic Type Structures
typedef struct U8Array U8Array;
struct U8Array
{
  U8  *v;
  U64 count;
};

typedef struct U16Array U16Array;
struct U16Array
{
  U16 *v;
  U64 count;
};

typedef struct U32Array U32Array;
struct U32Array
{
  U32 *v;
  U64 count;
};

typedef struct U64Array U64Array;
struct U64Array
{
  U64 *v;
  U64 count;
};

////////////////////////////////
// Basic Constants
global const U32 sign32     = 0x80000000;
global const U32 exponent32 = 0x7F800000;
global const U32 mantissa32 = 0x007FFFFF;

global const F32 big_golden32   = 1.61803398875f;
global const F32 small_golden32 = 0.61803398875f;

global const F32 pi32 = 3.1415926535897f;

global const F64 machine_epsilon64 = 4.94065645841247e-324;

global const U64 max_U64 = 0xffffffffffffffffull;
global const U32 max_U32 = 0xffffffff;
global const U16 max_U16 = 0xffff;
global const U8  max_U8  = 0xff;

global const S64 max_S64 = (S64)0x7fffffffffffffffll;
global const S32 max_S32 = (S32)0x7fffffff;
global const S16 max_S16 = (S16)0x7fff;
global const S8  max_S8  = (S8)0x7f;

global const S64 min_S64 = (S64)0x8000000000000000ll;
global const S32 min_S32 = (S32)0x80000000;
global const S16 min_S16 = (S16)0x8000;
global const S8  min_S8  = (S8)0x80;

global const U32 bitmask1  = 0x00000001;
global const U32 bitmask2  = 0x00000003;
global const U32 bitmask3  = 0x00000007;
global const U32 bitmask4  = 0x0000000f;
global const U32 bitmask5  = 0x0000001f;
global const U32 bitmask6  = 0x0000003f;
global const U32 bitmask7  = 0x0000007f;
global const U32 bitmask8  = 0x000000ff;
global const U32 bitmask9  = 0x000001ff;
global const U32 bitmask10 = 0x000003ff;
global const U32 bitmask11 = 0x000007ff;
global const U32 bitmask12 = 0x00000fff;
global const U32 bitmask13 = 0x00001fff;
global const U32 bitmask14 = 0x00003fff;
global const U32 bitmask15 = 0x00007fff;
global const U32 bitmask16 = 0x0000ffff;
global const U32 bitmask17 = 0x0001ffff;
global const U32 bitmask18 = 0x0003ffff;
global const U32 bitmask19 = 0x0007ffff;
global const U32 bitmask20 = 0x000fffff;
global const U32 bitmask21 = 0x001fffff;
global const U32 bitmask22 = 0x003fffff;
global const U32 bitmask23 = 0x007fffff;
global const U32 bitmask24 = 0x00ffffff;
global const U32 bitmask25 = 0x01ffffff;
global const U32 bitmask26 = 0x03ffffff;
global const U32 bitmask27 = 0x07ffffff;
global const U32 bitmask28 = 0x0fffffff;
global const U32 bitmask29 = 0x1fffffff;
global const U32 bitmask30 = 0x3fffffff;
global const U32 bitmask31 = 0x7fffffff;
global const U32 bitmask32 = 0xffffffff;

global const U64 bitmask33 = 0x00000001ffffffffull;
global const U64 bitmask34 = 0x00000003ffffffffull;
global const U64 bitmask35 = 0x00000007ffffffffull;
global const U64 bitmask36 = 0x0000000fffffffffull;
global const U64 bitmask37 = 0x0000001fffffffffull;
global const U64 bitmask38 = 0x0000003fffffffffull;
global const U64 bitmask39 = 0x0000007fffffffffull;
global const U64 bitmask40 = 0x000000ffffffffffull;
global const U64 bitmask41 = 0x000001ffffffffffull;
global const U64 bitmask42 = 0x000003ffffffffffull;
global const U64 bitmask43 = 0x000007ffffffffffull;
global const U64 bitmask44 = 0x00000fffffffffffull;
global const U64 bitmask45 = 0x00001fffffffffffull;
global const U64 bitmask46 = 0x00003fffffffffffull;
global const U64 bitmask47 = 0x00007fffffffffffull;
global const U64 bitmask48 = 0x0000ffffffffffffull;
global const U64 bitmask49 = 0x0001ffffffffffffull;
global const U64 bitmask50 = 0x0003ffffffffffffull;
global const U64 bitmask51 = 0x0007ffffffffffffull;
global const U64 bitmask52 = 0x000fffffffffffffull;
global const U64 bitmask53 = 0x001fffffffffffffull;
global const U64 bitmask54 = 0x003fffffffffffffull;
global const U64 bitmask55 = 0x007fffffffffffffull;
global const U64 bitmask56 = 0x00ffffffffffffffull;
global const U64 bitmask57 = 0x01ffffffffffffffull;
global const U64 bitmask58 = 0x03ffffffffffffffull;
global const U64 bitmask59 = 0x07ffffffffffffffull;
global const U64 bitmask60 = 0x0fffffffffffffffull;
global const U64 bitmask61 = 0x1fffffffffffffffull;
global const U64 bitmask62 = 0x3fffffffffffffffull;
global const U64 bitmask63 = 0x7fffffffffffffffull;
global const U64 bitmask64 = 0xffffffffffffffffull;

global const U32 bit1  = (1<<0);
global const U32 bit2  = (1<<1);
global const U32 bit3  = (1<<2);
global const U32 bit4  = (1<<3);
global const U32 bit5  = (1<<4);
global const U32 bit6  = (1<<5);
global const U32 bit7  = (1<<6);
global const U32 bit8  = (1<<7);
global const U32 bit9  = (1<<8);
global const U32 bit10 = (1<<9);
global const U32 bit11 = (1<<10);
global const U32 bit12 = (1<<11);
global const U32 bit13 = (1<<12);
global const U32 bit14 = (1<<13);
global const U32 bit15 = (1<<14);
global const U32 bit16 = (1<<15);
global const U32 bit17 = (1<<16);
global const U32 bit18 = (1<<17);
global const U32 bit19 = (1<<18);
global const U32 bit20 = (1<<19);
global const U32 bit21 = (1<<20);
global const U32 bit22 = (1<<21);
global const U32 bit23 = (1<<22);
global const U32 bit24 = (1<<23);
global const U32 bit25 = (1<<24);
global const U32 bit26 = (1<<25);
global const U32 bit27 = (1<<26);
global const U32 bit28 = (1<<27);
global const U32 bit29 = (1<<28);
global const U32 bit30 = (1<<29);
global const U32 bit31 = (1<<30);
global const U32 bit32 = (1<<31);

global const U64 bit33 = (1ull<<32);
global const U64 bit34 = (1ull<<33);
global const U64 bit35 = (1ull<<34);
global const U64 bit36 = (1ull<<35);
global const U64 bit37 = (1ull<<36);
global const U64 bit38 = (1ull<<37);
global const U64 bit39 = (1ull<<38);
global const U64 bit40 = (1ull<<39);
global const U64 bit41 = (1ull<<40);
global const U64 bit42 = (1ull<<41);
global const U64 bit43 = (1ull<<42);
global const U64 bit44 = (1ull<<43);
global const U64 bit45 = (1ull<<44);
global const U64 bit46 = (1ull<<45);
global const U64 bit47 = (1ull<<46);
global const U64 bit48 = (1ull<<47);
global const U64 bit49 = (1ull<<48);
global const U64 bit50 = (1ull<<49);
global const U64 bit51 = (1ull<<50);
global const U64 bit52 = (1ull<<51);
global const U64 bit53 = (1ull<<52);
global const U64 bit54 = (1ull<<53);
global const U64 bit55 = (1ull<<54);
global const U64 bit56 = (1ull<<55);
global const U64 bit57 = (1ull<<56);
global const U64 bit58 = (1ull<<57);
global const U64 bit59 = (1ull<<58);
global const U64 bit60 = (1ull<<59);
global const U64 bit61 = (1ull<<60);
global const U64 bit62 = (1ull<<61);
global const U64 bit63 = (1ull<<62);
global const U64 bit64 = (1ull<<63);

////////////////////////////////
// Safe Casts
internal_function U16 safe_cast_u16(U32 x);
internal_function U32 safe_cast_u32(U64 x);
internal_function S32 safe_cast_s32(S64 x);

////////////////////////////////
// Memory Manipulation Functions
internal_function void memory_set_u32(void *memory, U32 value, U64 byte_size);

#endif // BASE_CORE_H

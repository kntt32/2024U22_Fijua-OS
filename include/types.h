#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <build.h>

#define in
#define out
#define optional

typedef char ascii;

typedef signed char sint8;
typedef unsigned char uint8;
typedef signed short sint16;
typedef unsigned short uint16;
typedef signed int sint32;
typedef unsigned int uint32;
typedef signed long long sint64;
typedef unsigned long long uint64;
typedef struct {signed long long ln; unsigned long long rn;} sint128;
typedef struct {unsigned long long ln; unsigned long long rn;} uint128;

typedef float float32;
typedef double float64;

#if PLATFORM_64BIT
typedef sint64 sintn;
typedef uint64 uintn;
#else
typedef sint32 sintn;
typedef uint32 uintn;
#endif

#endif

//////////////////////////////////////////////////////////////////////
// ToolConfig.h - Switches for all the DMcTools file
//
// Copyright David K. McAllister, 1996-2014.

#pragma once

#define DMC_USE_JPEG
//#define DMC_USE_TIFF
#define DMC_USE_PNG
//#define DMC_USE_MAT

// Define this to use the class "half" from ILM's OpenEXR.
#define DMC_USE_HALF_FLOAT

#ifdef __GNUC__
// This is a GCC compiler
#define DMC_MACHINE_gcc

#define DMC_LITTLE_ENDIAN

#define DMC_DECL inline

#if __x86_64__ || __ppc64__
#define DMC_BITNESS_64
#define DMC_BITNESS 64
#else
#define DMC_BITNESS_32
#define DMC_BITNESS 32
#endif

#endif

#if _WIN32 || _WIN64
// This is a Visual Studio compiler
#define DMC_MACHINE_win

#define DMC_LITTLE_ENDIAN

#ifdef __CUDACC__
#define DMC_DECL __forceinline __forceinline__ __host__ __device__
#define DMC_HDECL __forceinline __forceinline__ __host__
#else
#ifdef __INTEL_COMPILER
#define DMC_DECL inline
#define DMC_HDECL inline
#else
#define DMC_DECL __forceinline
#define DMC_HDECL __forceinline
#endif
#endif

#if _WIN64
#define DMC_BITNESS_64
#define DMC_BITNESS 64
#else
#define DMC_BITNESS_32
#define DMC_BITNESS 32
#endif

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996) // This function or variable may be unsafe.
#pragma warning(disable : 4244) // Conversion from '__int64' to 'int', possible loss of data
//#pragma warning(disable : 4305) // Disable bogus conversion warnings
//#pragma warning (disable:4800) // Disable bogus conversion warnings
//#pragma warning (disable:4786) // stupid symbol size limitation
//#pragma warning (disable:4018) // disable signed/unsigned mismatch
//#pragma warning (disable:4267) // disable 64-bit conversion warnings

#endif

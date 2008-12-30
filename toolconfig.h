//////////////////////////////////////////////////////////////////////
// toolconfig.h - Switches for all the DMcTools file.
//
// Copyright David K. McAllister, 1996-2007.

#ifndef TOOLCONFIG_H
#define TOOLCONFIG_H

#define DMC_USE_JPEG
#define DMC_USE_TIFF
#define DMC_USE_PNG
// Define this to use the class "half" from ILM's OpenEXR.
#define DMC_USE_HALF_FLOAT
//#define DMC_USE_MAT

#ifdef  __GNUC__
#define DMC_MACHINE_gcc
#ifndef DMCINT64
#define DMCINT64 long long
#endif
#define DMC_INLINE inline
#endif

#ifdef _WIN32
#define DMC_MACHINE_win
#define DMC_LITTLE_ENDIAN
#define inline __forceinline

#pragma warning (disable:4996) // disable deprecated warning
#pragma warning (disable:4244) // disable bogus conversion warnings
#pragma warning (disable:4305) // disable bogus conversion warnings
//#pragma warning (disable:4800) // disable bogus conversion warnings
//#pragma warning (disable:4786) // stupid symbol size limitation
//#pragma warning (disable:4018) // disable signed/unsigned mismatch
//#pragma warning (disable:4267) // disable 64-bit conversion warnings

#ifndef DMCINT64
#define DMCINT64 __int64
#endif

#ifdef __INTEL_COMPILER
#define DMC_INLINE
#else
#define DMC_INLINE __forceinline
#endif

#endif

#endif

//////////////////////////////////////////////////////////////////////
// Util.h - convert endian, random numbers, other random stuff.
//
// Copyright David K. McAllister, 1998.

#ifndef dmc_util_h
#define dmc_util_h

#include "toolconfig.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

// Returns 0 on MIPS, PA-RISC, SPARC
// Returns 1 on Intel x86
static DMC_INLINE bool AmLittleEndian()
{
    typedef union { int integer; char byte[4]; } endianTest;
    endianTest etest;
    etest.integer = 1;
    return etest.byte[3] == 0;
}

// Swap endian-ness of the array. length is the number of 8-byte words to swap.
static DMC_INLINE void ConvertDouble(double *array, int length)
{
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7;
    unsigned char *ptr = (unsigned char *)array;
    unsigned char *arr = (unsigned char *)array;

    while (length--) {
        b0 = *ptr++;
        b1 = *ptr++;
        b2 = *ptr++;
        b3 = *ptr++;
        b4 = *ptr++;
        b5 = *ptr++;
        b6 = *ptr++;
        b7 = *ptr++;
        arr[0] = b7;
        arr[1] = b6;
        arr[2] = b5;
        arr[3] = b4;
        arr[4] = b3;
        arr[5] = b2;
        arr[6] = b1;
        arr[7] = b0;
        arr += 8;
    }
}

// Swap endian-ness of the array. length is the number of 4-byte words to swap.
static DMC_INLINE void ConvertLong(unsigned int *array, int length)
{
    unsigned char b0, b1, b2, b3;
    unsigned char *ptr = (unsigned char *)array;
    unsigned char *arr = (unsigned char *)array;

    while (length--) {
        b0 = *ptr++;
        b1 = *ptr++;
        b2 = *ptr++;
        b3 = *ptr++;
        arr[0] = b3;
        arr[1] = b2;
        arr[2] = b1;
        arr[3] = b0;
        arr += 4;
    }
}

// Swap endian-ness of the array. length is the number of 2-byte shorts to swap.
static DMC_INLINE void ConvertShort(unsigned short *array, int length)
{
    unsigned char b0, b1;
    unsigned char *ptr = (unsigned char *)array;
    unsigned char *arr = (unsigned char *)array;

    while (length--) {
        b0 = *ptr++;
        b1 = *ptr++;
        arr[0] = b1;
        arr[1] = b0;
        arr += 2;
    }
}

// Given an argument vector, remove NumDel strings from it, starting at i.
static DMC_INLINE void RemoveArgs(int &argc, char **argv, int &i, int NumDel=1)
{
    argc -= NumDel;
    memmove(&(argv[i]), &(argv[i+NumDel]), (argc-i) * sizeof(char *));
    i--;
}

// Note: NRand (normal distribution) is in MiscMath.h

// A random number on 0.0 to 1.0.
DMC_INLINE double DRand()
{
#ifdef DMC_MACHINE_win
#define ONEOVERRAND_MAX (1.0 / double(RAND_MAX*RAND_MAX))
    return double((rand()<<15)|rand()) * ONEOVERRAND_MAX;
#else
    return drand48();
#endif
}

DMC_INLINE double DRand(const double high) { return DRand() * high; } // A random number on 0.0 to high.
DMC_INLINE double DRand(const double low, const double high) { return low + DRand() * (high - low); } // A random number on low to high.

// A random number on 0.0 to 1.0.
DMC_INLINE float DRandf()
{
#ifdef DMC_MACHINE_win
#define ONEOVERRAND_MAXF (1.0f / float(RAND_MAX*RAND_MAX))
    return float((rand()<<15)|rand()) * ONEOVERRAND_MAXF;
#else
    return drand48();
#endif
}

DMC_INLINE float DRandf(const float high) { return DRandf() * high; } // A random number on 0.0 to high.
DMC_INLINE float DRandf(const float low, const float high) { return low + DRandf() * (high - low); } // A random number on low to high.

// A random number.
DMC_INLINE int LRand()
{
#ifdef DMC_MACHINE_win
    return abs((rand() ^ (rand() << 15) ^ (rand() << 30)));
#else
    return int(lrand48());
#endif
}

DMC_INLINE int LRand(const int high) { return LRand() % high; } // A random number on 0 to high.
DMC_INLINE int LRand(const int low, const int high) { return low + (LRand() % (high - low)); } // A random number on low to high.

// Seeds the random number generator based on time and/or process ID.
extern void SRand();

// Return just the filename.extension portion of a path.
char *GetFileName(const char *inpath);

// Return just the extension of a path.
char *GetFileExtension(const char *inpath);

// Return just the filename of a path without dirs or extension.
char *GetFileBaseName(const char *inpath);

// Return just the path without a filename. Ends in '/' if any.
char *GetFilePath(const char *inpath);

DMC_INLINE void ToLower(char *str)
{
    for(; *str; str++)
        if((*str >= 'A') && (*str <= 'Z'))
            *str += 32;
}

DMC_INLINE void ToUpper(char *str)
{
    for(; *str; str++)
        if((*str >= 'a') && (*str <= 'z'))
            *str -= 32;
}

// Makes a fairly random 32-bit number from a string.
extern int HashString(const char *);

// Used for setting up the floating point output the way I like it.
DMC_INLINE void FloatFmt(std::ostream& os, int prec = 8)
{
    os.precision(prec);
    os.setf(std::ios_base::fixed,std::ios_base::floatfield);
    os.setf(std::ios_base::fixed|std::ios_base::showpoint|std::ios_base::showpos, std::ios_base::floatfield);
}

DMC_INLINE void cpuid(unsigned int cmd, unsigned int &a, unsigned int &b, unsigned int &c, unsigned int &d)
{
    unsigned int aa=0, bb=0, cc=0, dd=0;
#ifdef __GNUC__
	asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (cmd));
#else
    __asm {
      mov eax, cmd
      xor ebx, ebx
      xor ecx, ecx
      xor edx, edx
      cpuid
      mov aa, eax
      mov bb, ebx
      mov cc, ecx
      mov dd, edx
    }
#endif
    a = aa; b = bb; c = cc; d = dd;
}

DMC_INLINE bool HasMMX()
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);
    if(a < 1) return false;
    cpuid(1, a, b, c, d);
    if(d & (1<<23)) return true; else return false;
}

DMC_INLINE bool HasSSE()
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);
    if(a < 1) return false;
    cpuid(1, a, b, c, d);
    if(d & (1<<25)) return true; else return false;
}

DMC_INLINE bool HasSSE2()
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);
    if(a < 1) return false;
    cpuid(1, a, b, c, d);
    if(d & (1<<26)) return true; else return false;
}

DMC_INLINE bool HasSSE3()
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);
    if(a < 1) return false;
    cpuid(1, a, b, c, d);
    if(c & (1<<0)) return true; else return false;
}

DMC_INLINE unsigned int NumCores()
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);
    if(a < 4) return 1;
    cpuid(4, a, b, c, d);
    unsigned int num = (a>>26) + 1;
    return num;
}

// You must allocate at least 13 bytes, on a 4-byte boundary.
DMC_INLINE void CPUVendorString(char *val)
{
    unsigned int a=0, b=0, c=0, d=0;
    cpuid(0, a, b, c, d);

    *((unsigned int *) &val[0]) = b;
    *((unsigned int *) &val[4]) = d;
    *((unsigned int *) &val[8]) = c;
    val[12] = 0;
}

#endif

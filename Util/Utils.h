//////////////////////////////////////////////////////////////////////
// Util.h - convert endian, random numbers, other random stuff.
//
// Copyright David K. McAllister, 1998.

#ifndef _util_h
#define _util_h

#include "toolconfig.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>

// Returns 0 on MIPS, PA-RISC, SPARC
// Returns 1 on Intel x86
static inline bool AmLittleEndian()
{
    typedef union { int integer; char byte[4]; } endianTest;
    endianTest etest;
    etest.integer = 1;
    return etest.byte[3] == 0;
}

// Swap endian-ness of the array. length is the number of 8-byte words to swap.
static inline void ConvertDouble(double *array, int length)
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
static inline void ConvertLong(unsigned int *array, int length)
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
static inline void ConvertShort(unsigned short *array, int length)
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
static inline void RemoveArgs(int &argc, char **argv, int &i, int NumDel=1)
{
	argc -= NumDel;
	memmove(&(argv[i]), &(argv[i+NumDel]), (argc-i) * sizeof(char *));
	i--;
}

// Note: NRand (normal distribution) is in MiscMath.h

// A random number on 0.0 to 1.0.
inline double DRand()
{
#ifdef DMC_MACHINE_win
#define ONEOVERRAND_MAX (1.0 / double(RAND_MAX*RAND_MAX))
	return double((rand()<<15)|rand()) * ONEOVERRAND_MAX;
#else
	return drand48();
#endif
}

// A random number on 0.0 to high.
inline double DRand(double high)
{
#ifdef DMC_MACHINE_win
	return double((rand()<<15)|rand()) * (high * ONEOVERRAND_MAX);
#else
	return drand48() * high;
#endif
}

// A random number on low to high.
inline double DRand(double low, double high)
{
	double span = high - low;
#ifdef DMC_MACHINE_win
	return low + double((rand()<<15)|rand()) * (span * ONEOVERRAND_MAX);
#else
	return low + drand48() * span;
#endif
}

// A random number.
inline int LRand()
{
#ifdef DMC_MACHINE_win
	return abs((rand() ^ (rand() << 15) ^ (rand() << 30)));
#else
	return int(lrand48());
#endif
}

inline int LRand(int high)
{
	return LRand() % high;
}

// Seeds the random number generator based on time and/or process ID.
extern void SRand();

// Return just the fname.xxx portion of a path.
char *GetFileName(const char *inpath);

// Return just the extension of a path.
char *GetFileExtension(const char *inpath);

// Return just the extension of a path.
char *GetFileBaseName(const char *inpath);

// Return just the path without a filename. Ends in '/' if any.
char *GetFilePath(const char *inpath);

inline void ToLower(char *str)
{
    for(; *str; str++)
        if((*str >= 'A') && (*str <= 'Z'))
            *str += 32;
}

inline void ToUpper(char *str)
{
    for(; *str; str++)
        if((*str >= 'a') && (*str <= 'z'))
            *str -= 32;
}

// Makes a fairly random 32-bit number from a string.
extern int HashString(const char *);

// Used for setting up the floating point output the way I like it.
inline void FloatFmt(std::ostream& os, int prec = 8)
{
	os.precision(prec);
	os.setf(std::ios_base::fixed,std::ios_base::floatfield);
	os.setf(std::ios_base::fixed|std::ios_base::showpoint|std::ios_base::showpos, std::ios_base::floatfield);
}

#endif

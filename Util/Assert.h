//////////////////////////////////////////////////////////////////////
// Assert.h - Utility for specifying data invariants (Assertions)
//
// Changes Copyright David K. McAllister, Dec. 1998.
// Originally written by Steven G. Parker, Feb. 1994.

#ifndef _dmc_assert_h
#define _dmc_assert_h

#include "toolconfig.h"

#ifdef DMC_MACHINE_sgi
#include <iostream>
#endif

#ifdef DMC_MACHINE_win
#include <iostream>
#endif

#ifdef DMC_MACHINE_hp
#include <iostream.h>
#endif

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif
#include <stdlib.h>

// This assertion always causes a fatal error. It doesn't depend on debug mode.
#ifndef ASSERTERR
#ifdef _DEBUG
#define ASSERTERR(condition,msg) {if(!(condition)){std::cerr << "Fatal Error: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; assert(0);}}
#else
#define ASSERTERR(condition,msg) {if(!(condition)){std::cerr << "Fatal Error: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; exit(1);}}
#endif
#endif

// This kind does not depend on debug mode.
// Always terminates on failure.
#ifndef ASSERT0
#define ASSERT0(condition) {if(!(condition)){std::cerr << "Assertion0 Failed: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << std::endl; assert(0);}}
#endif

// Prints a message; doesn't terminate.
#ifndef ASSERT0M
#ifdef _DEBUG
#define ASSERT0M(condition,msg) {if(!(condition)){std::cerr << "Assertion0 Failed: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl;}}
#else
#define ASSERT0M(condition,msg) {if(!(condition)){std::cerr << "Assertion0 Failed: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl;}}
#endif
#endif

// These two depend on debug mode.
#if DMC_ASSERTION_LEVEL >= 1
#ifndef ASSERT1
#define ASSERT1(condition) {if(!(condition)){std::cerr << "Assertion1 Failed: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << std::endl; assert(0);}}
#endif
#ifndef ASSERT1M
#define ASSERT1M(condition,msg) {if(!(condition)){std::cerr << "Assertion1 Failed: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; assert(0);}}
#endif
#else
#ifndef ASSERT1
#define ASSERT1(condition)
#endif
#ifndef ASSERT1M
#define ASSERT1M(condition,msg)
#endif
#endif

#ifndef ASSERT
#define ASSERT(condition) ASSERT1(condition)
#endif

#ifndef GL_ASSERT
#define GL_ASSERT() {GLenum DMC_err; while ((DMC_err = glGetError()) != GL_NO_ERROR) \
			std::cerr << "OpenGL error: " << (char *)gluErrorString(DMC_err) << " at " << __FILE__ <<":" << __LINE__ << std::endl;}
#endif

#endif

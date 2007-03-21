//////////////////////////////////////////////////////////////////////
// Assert.h - Utility for specifying data invariants (Assertions)
//
// Changes Copyright David K. McAllister, Dec. 1998.
// Originally written by Steven G. Parker, Feb. 1994.

#ifndef _dmc_assert_h
#define _dmc_assert_h

#include "toolconfig.h"

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif

#include <stdlib.h>

#ifdef OLD_ASSERTS
#include <iostream>

// These are always fatal and depend on debug mode.
#ifdef _DEBUG
// Always terminates on failure.
#define ASSERT_RM(condition,msg) {if(!(condition)){std::cerr << "Assert_RM: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush(); assert(0);}}
#define ASSERT_R(condition) {if(!(condition)){std::cerr << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << std::endl; std::cerr.flush(); assert(0);}}

// Terminates since debug mode.
#define ASSERT_DM(condition,msg) {if(!(condition)){std::cerr << "Assert_DM: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush(); assert(0);}}
#define ASSERT_D(condition) {if(!(condition)){std::cerr << "Assert_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << std::endl; std::cerr.flush(); assert(0);}}

// Prints a message since debug mode; doesn't terminate.
#define WARN_D(condition,msg) {if(!(condition)){std::cerr << "Warn_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}
#else
// Always terminates on failure.
#define ASSERT_RM(condition,msg) {if(!(condition)){std::cerr << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush(); exit(1);}}
#define ASSERT_R(condition) {if(!(condition)){std::cerr << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << std::endl; std::cerr.flush(); exit(1);}}

#define ASSERT_DM(condition)
#define ASSERT_D(condition)

#define WARN_D(condition,msg)
#endif

// Always prints a message; doesn't terminate.
#define WARN_R(condition,msg) {if(!(condition)){std::cerr << "Warn_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}

#define GL_ASSERT() {GLenum DMC_err; while ((DMC_err = glGetError()) != GL_NO_ERROR) \
            std::cerr << "OpenGL error: " << (char *)gluErrorString(DMC_err) << " at " << __FILE__ <<":" << __LINE__ << std::endl; std::cerr.flush();}

#else

#include <sstream>
#include <string>

// The error class to throw in an assert and elsewhere in DMcTools.
struct DMcError {
    std::string Er;
    DMcError(const std::string &st = "DMcTools error") : Er(st) {}
};

// These are always fatal and depend on debug mode.
#ifdef _DEBUG
// Always terminates on failure.
#define ASSERT_RM(condition,msg) {if(!(condition)){std::ostringstream Er; Er << "Assert_RM: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg); throw DMcError(Er.str()); }}
#define ASSERT_R(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); }}

// Terminates since debug mode.
#define ASSERT_DM(condition,msg) {if(!(condition)){std::ostringstream Er; Er << "Assert_DM: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg); throw DMcError(Er.str()); }}
#define ASSERT_D(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); }}

// Prints a message since debug mode; doesn't terminate.
#define WARN_D(condition,msg) {if(!(condition)){std::cerr << "Warn_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}
#else
// Always terminates on failure.
#define ASSERT_RM(condition,msg) {if(!(condition)){std::ostringstream Er; Er << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg); throw DMcError(Er.str()); }}
#define ASSERT_R(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); }}

#define ASSERT_DM(condition)
#define ASSERT_D(condition)

#define WARN_D(condition,msg)
#endif

// Always prints a message; doesn't terminate.
#define WARN_R(condition,msg) {if(!(condition)){std::cerr << "Warn_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}

#define GL_ASSERT() {GLenum DMC_err; while ((DMC_err = glGetError()) != GL_NO_ERROR) \
    { std::ostringstream Er; Er << "OpenGL error: " << (char *)gluErrorString(DMC_err) << " at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); } }

#endif
#endif

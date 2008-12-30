//////////////////////////////////////////////////////////////////////
// Assert.h - Utility for specifying assertions
//
// Copyright David K. McAllister, 1998-2007.

#ifndef dmc_assert_h
#define dmc_assert_h

#include "toolconfig.h"

#include <cstdlib>
#include <sstream>
#include <string>

// The error class to throw in an assert and elsewhere in DMcTools.
struct DMcError {
    std::string Er;
    DMcError(const std::string &st = "DMcTools error") : Er(st) {}
};

// Always terminates on failure.
#define ASSERT_RM(condition,msg) {if(!(condition)){std::ostringstream Er; Er << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg); throw DMcError(Er.str()); }}
#define ASSERT_R(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); }}

// Always prints a message; doesn't terminate.
#define WARN_R(condition,msg) {if(!(condition)){std::cerr << "Warn_R: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}

// These are always fatal and depend on debug mode.
#ifdef _DEBUG
// Terminates in debug mode.
#define ASSERT_DM(condition,msg) {if(!(condition)){std::ostringstream Er; Er << "Assert_DM: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg); throw DMcError(Er.str()); }}
#define ASSERT_D(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); }}
// Use this version for breakpoints.
// #define ASSERT_D(condition) {if(!(condition)){std::ostringstream Er; Er << "Assert_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__; while(1); }}

// Prints a message in debug mode; doesn't terminate.
#define WARN_D(condition,msg) {if(!(condition)){std::cerr << "Warn_D: (" << (#condition) << ") at " << __FILE__ <<":" << __LINE__ << " " << (#msg) << std::endl; std::cerr.flush();}}
#else

#define ASSERT_DM(condition,msg)
#define ASSERT_D(condition)

#define WARN_D(condition,msg)
#endif

#define GL_ASSERT() {GLenum DMC_err; while ((DMC_err = glGetError()) != GL_NO_ERROR) \
    { std::ostringstream Er; Er << "OpenGL error: " << (char *)gluErrorString(DMC_err) << " at " << __FILE__ <<":" << __LINE__; throw DMcError(Er.str()); } }

#endif

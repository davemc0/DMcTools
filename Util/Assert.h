//////////////////////////////////////////////////////////////////////
// Assert.h - Utility for specifying assertions
//
// Copyright David K. McAllister, 1998-2007.

#pragma once

#include "Util/toolconfig.h"

#include <cstdlib>
#include <string>

// The error class to throw in an assert and elsewhere in DMcTools
struct DMcError {
    std::string Er;
    DMcError(const std::string& st = "DMcTools error") : Er(st) {}
};

// On failure, prints msg and terminates in debug or release mode
#define ASSERT_RM(condition, msg)                                                                                                                     \
    {                                                                                                                                                 \
        if (!(condition)) throw DMcError(std::string("Assert_RM: (") + #condition + ") at " + __FILE__ + ":" + std::to_string(__LINE__) + " " + msg); \
    }
// On failure, terminates in debug or release mode
#define ASSERT_R(condition)                                                                                                              \
    {                                                                                                                                    \
        if (!(condition)) throw DMcError(std::string("Assert_R: (") + #condition + ") at " + __FILE__ + ":" + std::to_string(__LINE__)); \
    }
// On failure, prints msg in debug or release mode
#define WARN_R(condition, msg)                                                                                             \
    {                                                                                                                      \
        if (!(condition)) {                                                                                                \
            std::cerr << "Warn_R: (" << (#condition) << ") at " << __FILE__ << ":" << __LINE__ << " " << msg << std::endl; \
            std::cerr.flush();                                                                                             \
        }                                                                                                                  \
    }
// Prints msg in debug or release mode
#define INFO_R(msg)                    \
    {                                  \
        std::cerr << msg << std::endl; \
        std::cerr.flush();             \
    }

#if defined(_DEBUG) && !defined(__CUDA_ARCH__)

// On failure, prints msg and terminates in debug mode
#define ASSERT_DM(condition, msg)                                                                                                                         \
    {                                                                                                                                                     \
        if (!(condition)) { throw DMcError(std::string("Assert_DM: (") + #condition + ") at " + __FILE__ + ":" + std::to_string(__LINE__) + " " + msg); } \
    }
// On failure, terminates in debug mode
#define ASSERT_D(condition)                                                                                                                  \
    {                                                                                                                                        \
        if (!(condition)) { throw DMcError(std::string("Assert_D: (") + #condition + ") at " + __FILE__ + ":" + std::to_string(__LINE__)); } \
    }
// On failure, prints msg in debug mode
#define WARN_D(condition, msg)                                                                                             \
    {                                                                                                                      \
        if (!(condition)) {                                                                                                \
            std::cerr << "Warn_D: (" << (#condition) << ") at " << __FILE__ << ":" << __LINE__ << " " << msg << std::endl; \
            std::cerr.flush();                                                                                             \
        }                                                                                                                  \
    }
// Prints msg in debug mode
#define INFO(msg)                      \
    {                                  \
        std::cerr << msg << std::endl; \
        std::cerr.flush();             \
    }

#else

#define ASSERT_DM(condition, msg) \
    {                             \
    }
#define ASSERT_D(condition) \
    {                       \
    }
#define WARN_D(condition, msg) \
    {                          \
    }
#define INFO(msg) \
    {             \
    }
#endif

#define GL_ASSERT()                                                                                                                                \
    {                                                                                                                                              \
        GLenum DMC_err;                                                                                                                            \
        while ((DMC_err = glGetError()) != GL_NO_ERROR) {                                                                                          \
            throw DMcError(std::string("OpenGL error: (") + (char*)gluErrorString(DMC_err) + ") at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        }                                                                                                                                          \
    }\

//////////////////////////////////////////////////////////////////////
// Utils.cpp - convert endian, cpuid stuff, other random stuff
//
// Copyright David K. McAllister, 1998.

#include "Util/Utils.h"

#include "Util/ToolConfig.h"

#include <iostream>
#include <string>

#ifdef DMC_MACHINE_win
#include <process.h>
#include <time.h>
#endif

// Declared in Random.h
unsigned int SRand(unsigned int seed /* = 0 */)
{
#ifdef DMC_MACHINE_win
    seed = seed ? seed : (unsigned int)time(NULL) * _getpid();
#else
    seed = seed ? seed : (unsigned int) time(NULL) * getpid());
#endif
    srand(seed);

    return seed;
}

// Return just the path without a filename. Ends in '/' if any.
char* GetFilePath(const std::string& inpath)
{
    char* outpath = strdup(inpath.c_str());
    char* outfname = strrchr(outpath, '/');
    char* outfname2 = strrchr(outpath, '\\');
    if (outfname > outfname2)
        *(outfname + 1) = '\0';
    else if (outfname2)
        *(outfname2 + 1) = '\0';
    else
        *outpath = '\0';

    return outpath;
}

// Return just the fname.abc portion of a path.
char* GetFileName(const std::string& inpath)
{
    char* outpath = strdup(inpath.c_str());
    char* outfname = strrchr(outpath, '/');
    if (outfname) {
        *outfname = '\0';
        outfname++;
    } else
        outfname = outpath;

    char* outfname2 = strrchr(outfname, '\\');
    if (outfname2) {
        *outfname2 = '\0';
        outfname2++;
    } else {
        outfname2 = outfname;
    }

    char* outfname3 = strchr(outfname2, '\n');
    if (outfname3) { *outfname3 = '\0'; }

    outfname3 = strchr(outfname2, '\r');
    if (outfname3) { *outfname3 = '\0'; }

    char* gogo = strdup(outfname2);
    delete[] outpath;

    return gogo;
}

// Return just the fname portion of a path.
char* GetFileBaseName(const std::string& inpath)
{
    char* outpath = strdup(inpath.c_str());
    char* outfname = strrchr(outpath, '/');
    if (outfname) {
        *outfname = '\0';
        outfname++;
    } else
        outfname = outpath;

    char* outfname2 = strrchr(outfname, '\\');
    if (outfname2) {
        *outfname2 = '\0';
        outfname2++;
    } else {
        outfname2 = outfname;
    }

    char* outfname3 = strchr(outfname2, '\n');
    if (outfname3) { *outfname3 = '\0'; }

    outfname3 = strchr(outfname2, '\r');
    if (outfname3) { *outfname3 = '\0'; }

    outfname3 = strchr(outfname2, '.');
    if (outfname3) { *outfname3 = '\0'; }

    char* gogo = strdup(outfname2);
    delete[] outpath;

    return gogo;
}

// Return just the xxx portion of path/fname.xxx.
char* GetFileExtension(const std::string& inpath)
{
    char* outpath = strdup(inpath.c_str());
    char* outfname = strrchr(outpath, '.');
    if (outfname) {
        *outfname = '\0';
        outfname++;
    } else
        outfname = outpath;

    char* outfname3 = strchr(outfname, '\n');
    if (outfname3) { *outfname3 = '\0'; }

    outfname3 = strchr(outfname, '\r');
    if (outfname3) { *outfname3 = '\0'; }

    char* gogo = strdup(outfname);
    delete[] outpath;

    return gogo;
}

void CopyFile(const std::string& inFName, const std::string& outFName)
{
    std::cerr << "Copying from " << inFName << " to " << outFName << '\n';
    std::string cmd = std::string("COPY /Y /V \"") + inFName + "\" \"" + outFName + "\"";

    try {
        system(cmd.c_str());
    }
    catch (...) {
        std::cerr << "CopyFile failed: " << cmd << std::endl;
    }
}

// Makes a fairly random 32-bit number from a string.
int HashString(const char* s)
{
    int H = 0, i = 0, j = 0;
    while (*s) {
        H ^= int(*s) << i;
        i += 6;

        if (i > 24) {
            j = (j + 1) % 8;
            i = j;
        }
        s++;
    }

    return H;
}

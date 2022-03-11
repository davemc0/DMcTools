//////////////////////////////////////////////////////////////////////
// TextureDB.cpp - Represent a texture and a list of textures
//
// Copyright David K. McAllister, July 1999.

#include "Model/TextureDB.h"

#include "Util/Utils.h"

using namespace std;

// Load this texture into memory if it's not yet.
// If dir is specified, it replaces the path with dir.
// If prepend is true it instead prepends dir to TexFName.
// Returns false on success.
bool TexInfo::LoadToAppMemory(char* dir, bool prepend)
{
    if (Im)
        // Need to call ClearFromAppMemory() first.
        return true;

    char fname[2048];
    if (dir) {
        if (prepend)
            sprintf(fname, "%s/%s", dir, TexFName);
        else {
            char* basen = GetFileName(TexFName);
            sprintf(fname, "%s/%s", dir, basen);
            delete[] basen;
        }
    } else
        sprintf(fname, "%s", TexFName);

    bool ishdr = true;
    char* exten = GetFileExtension(fname);
    if (strlen(exten) == 3) {
        ishdr = (exten[0] == 'h' || exten[0] == 'H') && (exten[1] == 'd' || exten[1] == 'D') && (exten[2] == 'r' || exten[2] == 'R');
    } else
        ishdr = false;

    std::cerr << "Loading " << fname << std::endl;

    if (ishdr) {
        f3Image HDR(fname);
        if (HDR.size() > 0) {
            HDR *= f3Pixel(ExposureScale);
            Im = new uc3Image(HDR);
            ASSERT_RM(Im, "new uc3Image failed");
            IsLinear = true;
        }
    } else {
        Im = LoadtImage(fname);
    }

    if (Im->pv_virtual()) {
        // std::cerr << "Succeeded.\n";
        return false;
    } else {
        // std::cerr << "Failed.\n";
        delete Im;
        Im = NULL;
        return true;
    }
}

void TexInfo::Dump()
{
    std::cerr << TexID << " " << TexFName << " " << TextureTarget << " " << (IsMIPMap ? "Is MIP" : "NotMIP") << " Mult=" << MultiTexIDs[0] << ","
              << MultiTexIDs[1] << "," << MultiTexIDs[2] << std::endl;
}

// Returns -1 if not found.
TexInfo* TextureDB::FindByName(const char* name)
{
    ASSERT_R(name);
    char* namelower = strdup(name);
    ToLower(namelower);
    for (int tind = 0; tind < (int)TexList.size(); tind++) {
        char* nl = strdup(TexList[tind]->TexFName);
        ToLower(nl);
        if (!strcmp(nl, namelower)) {
            delete[] nl;
            return TexList[tind];
        }
        delete[] nl;
    }

    delete[] namelower;

    return NULL;
}

// Adds it if not found.
TexInfo* TextureDB::FindByNameOrAdd(const char* name)
{
    ASSERT_R(name);
    TexInfo* x = FindByName(name);
    if (x == NULL) {
        x = new TexInfo(name, NULL, -1);

        TexList.push_back(x);
    }

    return x;
}

void TextureDB::Dump()
{
    for (int i = 0; i < (int)TexList.size(); i++) {
        std::cerr << i << ": ";
        TexList[i]->Dump();
    }
}

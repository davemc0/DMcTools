//////////////////////////////////////////////////////////////////////
// TextureDB.cpp - Represent a texture and a list of textures.
//
// Copyright David K. McAllister, July 1999.

#include <Model/TextureDB.h>
#include <Util/Utils.h>
#include <Image/tImage.h>

// Load this texture into memory if it's not yet.
// If dir is specified, it replaces the path with dir.
// If prepend is true it instead prepends dir to TexFName.
// Returns false on success.
bool TexInfo::LoadToAppMemory(char *dir, bool prepend)
{
    if(Im)
        // Need to call ClearFromAppMemory() first.
        return true;
    
    char fname[2048];
    if(dir) {
        if(prepend)
            sprintf(fname, "%s/%s", dir, TexFName);
        else {
            char *basen = GetFileName(TexFName);
            sprintf(fname, "%s/%s", dir, basen);
            delete [] basen;
        }
    } else
        sprintf(fname, "%s", TexFName);
    
    bool ishdr = true;
    char *exten = GetFileExtension(fname);
    if(strlen(exten) == 3) {
        ishdr = (exten[0]=='h' || exten[0]=='H') &&
            (exten[1]=='d' || exten[1]=='D') &&
            (exten[2]=='r' || exten[2]=='R');
    } else
        ishdr = false;
    
    cerr << "Loading " << fname << endl;
    
    if(ishdr) {
        Im = new ucImage;
        f3Image HDR(fname);
        if(HDR.size() > 0) {
            HDR *= f3Pixel(ExposureScale);
            uc3Image NewIm = HDR;
            ASSERTERR(Im, "memory alloc failed");
            Im->Set(NewIm.w(), NewIm.h(), 3);
            memcpy(Im->Pix, NewIm.pp(), Im->dsize);
            IsLinear = true;
        }
    } else {
        Im = new ucImage(fname);
        ASSERTERR(Im, "memory alloc failed");	
    }
    
    if(Im->size > 0) {
        // cerr << "Succeeded.\n";
        return false;
    } else {
        // cerr << "Failed.\n";
        delete Im;
        Im = NULL;
        return true;
    }
}

void TexInfo::Dump()
{
    cerr << TexID << " " << TexFName << " " << TextureTarget << " " 
        << (IsMIPMap?"Is MIP":"NotMIP") << " Mult="<<MultiTexIDs[0]<<","<<MultiTexIDs[1]<<","<<MultiTexIDs[2]<<endl;
}

// Returns -1 if not found.
TexInfo *TextureDB::FindByName(const char *name)
{
    ASSERT0(name);
    char *namelower = strdup(name);
    ToLower(namelower);
    for(int tind=0; tind<TexList.size(); tind++) {
        char *nl = strdup(TexList[tind]->TexFName);
        ToLower(nl);
        if(!strcmp(nl, namelower)) {
            delete [] nl;
            return TexList[tind];
        }
        delete [] nl;
    }
    
    delete [] namelower;
    
    return NULL;
}

// Adds it if not found.
TexInfo *TextureDB::FindByNameOrAdd(const char *name)
{
    ASSERT0(name); 
    TexInfo *x = FindByName(name);
    if(x == NULL) {
        x = new TexInfo(name, NULL, -1);
        
        TexList.push_back(x);
    } 
    
    return x;
}

void TextureDB::Dump()
{
    for(int i=0; i<TexList.size(); i++) {
        cerr << i << ": ";
        TexList[i]->Dump();
    }
}

//////////////////////////////////////////////////////////////////////
// TextureDB.h - Represent a texture and a list of textures.
//
// Copyright David K. McAllister, July 1999.

#ifndef _texturedb_h
#define _texturedb_h

#include <Image/ucImage.h>

#include <vector>

#define NUM_TEX_IDS 10

struct TexInfo
{
	const char *TexFName;
	ucImage *Im;
    void *UserData0; // Used for storing pointer to SVB.
    void *UserData1; // Used for storing pointer to SVBtoImage.
	int TexID; // This is the OpenGL texture object ID.
	unsigned int TextureTarget; // The OpenGL texture target (like GL_TEXTURE_2D).
    int MultiTexIDs[10]; // Total hack. 0=diffuse 1=lobe shape 2=lobe albedo
    int NumLobes; // How many SBRDF lobes are here.
    float ExposureScale; // For loading an HDR and converting to normal.
    bool IsMIPMap; // What did we actually send down?
    bool IsLinear; // True if the image is linear light instead of gamma 2.2.
	
    inline TexInfo(const char *n, ucImage *im, int t)
    {
        TexFName = n;
        Im = im;
        TexID = t;
        TextureTarget = 0;
        UserData0 = NULL;
        UserData1 = NULL;
        IsMIPMap = false;
        IsLinear = false;
        ExposureScale = 1.0f;
        for(int i=0; i<NUM_TEX_IDS; i++)
            MultiTexIDs[i] = 0;
    }
    
    inline TexInfo()
    {
        TexFName = NULL; Im = NULL; TexID = -1;
        TextureTarget = 0;
        UserData0 = NULL;
        UserData1 = NULL;
        IsMIPMap = false;
        IsLinear = false;
        ExposureScale = 1.0f;
        for(int i=0; i<NUM_TEX_IDS; i++)
            MultiTexIDs[i] = 0;
    }
    
    // Load this texture into memory if it's not yet.
    // If dir is specified, it replaces the path with dir.
    // If prepend is true it instead prepends dir to TexFName.
    // Returns false on success.
    bool LoadToAppMemory(char *dir=NULL, bool prepend=false);
    
    inline void ClearFromAppMemory()
    {
        delete Im;
        Im = NULL;
    }

    void Dump();
};

class TextureDB
{
public:
	std::vector<TexInfo *> TexList;
	
	// Returns -1 if not found.
	inline TexInfo *FindByName(const char *name);
    
    // Adds it if not found.
    TexInfo *FindByNameOrAdd(const char *name);
    
	void Dump();
};

#endif

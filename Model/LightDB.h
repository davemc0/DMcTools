//////////////////////////////////////////////////////////////////////
// LightDB.h - Represent a Light and a list of Lights.
//
// Copyright David K. McAllister, July 1999.

#ifndef dmc_Lightdb_h
#define dmc_Lightdb_h

#include "Util/Utils.h"

#include <vector>

struct LightInfo
{
    char *LightName;
    bool Enabled;
    int LightID; // This is the OpenGL Light object ID.
    int LightType; // 0 = Directional, 1 = Point, 2 = Spot.
    float Intensity;
    Vector Color;
    Vector Position; // Direction or location.
    float DropOffRate;
    float CutOffAngle;

    DMC_INLINE LightInfo()
    {
        LightName = NULL;
        Enabled = true;
        LightID = -1;
        LightType = -1;
        Intensity = 1;
        Color = Vector(1,1,1);
        Position = Vector(0,0,1);
        DropOffRate = 0;
        CutOffAngle = 0.785398;
    }

    DMC_INLINE void Dump()
    {
        std::cerr << LightName<<" "<<(Enabled?"on ":"off ")<<LightID<<" "<<LightType<<" "
            <<Intensity<<" "<<Color<<" "<<Position
            <<" "<<DropOffRate<<" "<<CutOffAngle<<std::endl;
    }
};

class LightDB
{
public:
    std::vector<LightInfo *> LightList;

    // Returns -1 if not found.
    DMC_INLINE LightInfo *FindByName(const char *name)
    {
        ASSERT_R(name);
        for(int tind=0; tind<(int)LightList.size(); tind++) {
            if(!strcmp(LightList[tind]->LightName, name))
                return LightList[tind];
        }

        return NULL;
    }

    DMC_INLINE LightInfo *Add(char *name=NULL)
    {
        LightInfo *x = new LightInfo();
        x->LightName = name;

        LightList.push_back(x);

        return x;
    }

    // Adds it if not found.
    DMC_INLINE LightInfo *FindByNameOrAdd(const char *name)
    {
        ASSERT_R(name);
        LightInfo *x = FindByName(name);
        if(x == NULL) {
            x = Add();
        }

        return x;
    }

    DMC_INLINE void Dump()
    {
        for(int i=0; i<(int)LightList.size(); i++) {
            std::cerr << i << ": ";
            LightList[i]->Dump();
        }
    }
};

#endif

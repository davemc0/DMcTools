//////////////////////////////////////////////////////////////////////
// AElements.h - Mesh elements with attributes
//
// Copyright David K. McAllister, March 2000.

#pragma once

#include "Model/BaseObject.h"
#include "Model/MeshElements.h"

// This should be true if the space exists for the attributes.
// (that is, for anything derived from AElement).
#define HAS_ATTRIBS 0x80

struct AVertex : public Vertex {
    f3vec Col;
    f3vec Nor;
    f3vec Tex;
    f3vec Tan;

    DMC_DECL ~AVertex()
    {
        // INFO("~AVertex");
    }
};

DMC_DECL bool VertexDataSame(const AVertex* V0, const AVertex* V1, const unsigned int VertData)
{
    bool IsSame = V0->V == V1->V;

    if (VertData & OBJ_COLORS) IsSame = IsSame && (V0->Col == V1->Col);
    if (VertData & OBJ_NORMALS) IsSame = IsSame && (V0->Nor == V1->Nor);
    if (VertData & OBJ_TEXCOORDS) IsSame = IsSame && (V0->Tex == V1->Tex);
    if (VertData & OBJ_TANGENTS) IsSame = IsSame && (V0->Tan == V1->Tan);

#if 0
    if(!IsSame) {
        cerr << "Diff:\n";
        cerr <<"V"<<V0->V << V1->V << endl;
        if(VertData & OBJ_COLORS)    cerr <<"C"<<V0->Col << V1->Col << endl;
        if(VertData & OBJ_NORMALS)   cerr <<"N"<<V0->Nor << V1->Nor << endl;
        if(VertData & OBJ_TEXCOORDS) cerr <<"X"<<V0->Tex << V1->Tex << endl;
        if(VertData & OBJ_TANGENTS)  cerr <<"T"<<V0->Tan << V1->Tan << endl;
    }
#endif
    return IsSame;
}

struct AEdge : public Edge {
    DMC_DECL ~AEdge()
    {
        // INFO("~AEdge");
    }
};

struct AFace : public Face {
    f3vec Col;
    f3vec Nor;
    f3vec Tex;
    f3vec Tan;

    DMC_DECL ~AFace()
    {
        // INFO("~AFACE");
    }
};

// Factories for making a new one of these.
extern Vertex* AVertexFactory();
extern Edge* AEdgeFactory();
extern Face* AFaceFactory();

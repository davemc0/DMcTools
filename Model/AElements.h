//////////////////////////////////////////////////////////////////////
// AElements.h - Mesh elements with attributes.
//
// Copyright David K. McAllister, March 2000.

#ifndef AElements_h
#define AElements_h

#include "Model/MeshElements.h"
#include "Model/BaseObject.h"

// This should be true if the space exists for the attributes.
// (that is, for anything derived from AElement).
#define HAS_ATTRIBS 0x80

struct AVertex : public Vertex
{
    Vector Col;
    Vector Nor;
    Vector Tex;
    Vector Tan;

    DMC_INLINE ~AVertex()
    {
#ifdef DMC_MESH_DEBUG
    //cerr << "~AVertex\n";
#endif
    }
};

DMC_INLINE bool VertexDataSame(const AVertex *V0, const AVertex *V1, const unsigned int VertData)
{
    bool IsSame = V0->V == V1->V;

    if(VertData & OBJ_COLORS)    IsSame = IsSame && (V0->Col == V1->Col);
    if(VertData & OBJ_NORMALS)   IsSame = IsSame && (V0->Nor == V1->Nor);
    if(VertData & OBJ_TEXCOORDS) IsSame = IsSame && (V0->Tex == V1->Tex);
    if(VertData & OBJ_TANGENTS)  IsSame = IsSame && (V0->Tan == V1->Tan);

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

struct AEdge : public Edge
{
    DMC_INLINE ~AEdge()
    {
#ifdef DMC_MESH_DEBUG
    //cerr << "~AEdge\n";
#endif
    }
};

struct AFace : public Face
{
    Vector Col;
    Vector Nor;
    Vector Tex;
    Vector Tan;

    DMC_INLINE ~AFace()
    {
#ifdef DMC_MESH_DEBUG
        //cerr << "~AFace\n";
#endif
    }
};

// Factories for making a new one of these.
extern Vertex *AVertexFactory();
extern Edge *AEdgeFactory();
extern Face *AFaceFactory();

#endif

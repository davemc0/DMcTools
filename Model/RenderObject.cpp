//////////////////////////////////////////////////////////////////////
// RenderObject.cpp - Represent a single general triangle begin/end pair with rendering state.
//
// Copyright David K. McAllister, Dec. 2001.

#include <Model/Model.h>
#include "Model/RenderObject.h"

void RenderObject::Dump() const
{
#if 0
    cerr << "Name: " << Name << " ObjID: " << ObjID << endl;
    cerr << "Vertex count: " << verts.size() << endl;

    cerr << "Specular color: " << scolor << endl
        << "Emissive color: " << ecolor << endl
        << "Ambient color: " << acolor << endl
        << "Shininess: " << shininess << " PrimType: " << PrimType << endl
        << "RenderObject BBox: " << Box << "\n\nVertex \t\tNormal\t\tTexcoord\t\tDColor\n";

    for(int i=0; i<verts.size(); i++) {
        cerr << verts[i] << "\t";
        if(i<normals.size()) cerr << normals[i] << "\t"; else cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if(i<texcoords.size()) cerr << texcoords[i] << "\t"; else cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if(i<dcolors.size()) cerr << dcolors[i] << "\t"; else cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if(i<alphas.size()) cerr << alphas[i];
        cerr << endl;
    }
    cerr << endl << endl;
#endif
}

void RenderObject::RebuildBBox()
{
#if 0
    Box.Reset();

    for(int i=0; i<verts.size(); i++)
        Box += verts[i];
#endif
}

// Transform every vertex in the model.
// Transform every normal and tangent by using ProjectDirection, which is not
// the inverse transpose, but merely the upper 3x3.
void RenderObject::ApplyTransform(Matrix44 &Mat)
{
    ASSERT_R(0);
#if 0
    Box.Reset();

    int i;
    for(i=0; i<verts.size(); i++) {
        verts[i] = Mat * verts[i];
        Box += verts[i];
    }

    for(i=0; i<normals.size(); i++) {
        normals[i] = Mat.ProjectDirection(normals[i]);
    }

    // XXX Would it be better to recompute these?
    for(i=0; i<tangents.size(); i++) {
        tangents[i] = Mat.ProjectDirection(tangents[i]);
    }
#endif
}

void RenderObject::ApplyTextureTransform(Matrix44 &Mat)
{
    ASSERT_R(VertexType & OBJ_TEXCOORDS);

    int i;
    for(i=0; i<(int)verts.size(); i++) {
        texcoords[i] = Mat * texcoords[i];
    }
}

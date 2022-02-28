//////////////////////////////////////////////////////////////////////
// RenderObject.cpp - Represent a single general triangle begin/end pair with rendering state
//
// Copyright David K. McAllister, Dec. 2001.

#include "Model/RenderObject.h"

#include "Model/Model.h"

void RenderObject::Dump() const
{
#if 1
    std::cerr << "Name: " << Name << " ObjID: " << ObjID << std::endl;
    std::cerr << "Vertex count: " << verts.size() << std::endl;

    std::cerr << "Specular color: " << scolor << std::endl
              << "Emissive color: " << ecolor << std::endl
              << "Ambient color: " << acolor << std::endl
              << "Shininess: " << shininess << " ObjectType: " << ObjectType << std::endl
              << "RenderObject BBox: " << Box << "\n\nVertex \t\tNormal\t\tTexcoord\t\tDColor\n";

    for (int i = 0; i < verts.size(); i++) {
        std::cerr << verts[i] << "\t";
        if (i < normals.size())
            std::cerr << normals[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if (i < texcoords.size())
            std::cerr << texcoords[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if (i < dcolors.size())
            std::cerr << dcolors[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        // if (i < alphas.size()) std::cerr << alphas[i];
        std::cerr << std::endl;
    }
    std::cerr << std::endl << std::endl;
#endif
}

void RenderObject::RebuildBBox()
{
#if 0
    Box.reset();

    for(int i=0; i<verts.size(); i++)
        Box += verts[i];
#endif
}

// Transform every vertex in the model.
// Transform every normal and tangent by using ProjectDirection, which is not
// the inverse transpose, but merely the upper 3x3.
void RenderObject::ApplyTransform(Matrix44<f3vec>& Mat)
{
    ASSERT_R(0);
#if 0
    Box.reset();

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

void RenderObject::ApplyTextureTransform(Matrix44<f3vec>& Mat)
{
    ASSERT_R(VertexType & OBJ_TEXCOORDS);

    int i;
    for (i = 0; i < (int)verts.size(); i++) { texcoords[i] = Mat * texcoords[i]; }
}

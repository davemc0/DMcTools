//////////////////////////////////////////////////////////////////////
// RenderObject.h - Represent a single general begin/end pair with rendering state
//
// Copyright David K. McAllister, July 1999.

#pragma once

#include "Model/BaseObject.h"

#include <vector>

struct RenderObject : public BaseObject {
    std::vector<f3vec> verts;
    std::vector<f3vec> normals;   // Must have a length of 0 or VertexCount, or arbitrary non-zero if fnindices > 0.
    std::vector<f3vec> tangents;  // Must have a length of 0 or verts.size().
    std::vector<f3vec> texcoords; // Must have a length of 0 or verts.size().
    std::vector<f3vec> dcolors;   // Must have a length of 0 or verts.size().
    std::vector<int> indices;     // Indices into the list of verts.
    std::vector<int> fnindices;   // Indices into the list of normals. One per face, if being used. Length 0 otherwise.

    DMC_DECL RenderObject()
    {
        // std::cerr << "Making RenderObject.\n";
        ObjectType = DMC_RENDER_OBJECT;
    }

    virtual void Dump() const;

    // Sets the OBJ_WHATEVER flag.
    virtual void GenColors() { ASSERT_R(0); }
    virtual void GenNormals() { ASSERT_R(0); }
    virtual void GenTexCoords() { ASSERT_R(0); }
    virtual void GenTangents() { ASSERT_R(0); }

    // Clears the OBJ_WHATEVER flag.
    virtual void RemoveColors()
    {
        dcolors.clear();
        VertexType = VertexType & (~OBJ_COLORS);
    }
    virtual void RemoveNormals()
    {
        normals.clear();
        VertexType = VertexType & (~OBJ_NORMALS);
    }
    virtual void RemoveTexCoords()
    {
        texcoords.clear();
        VertexType = VertexType & (~OBJ_TEXCOORDS);
    }
    virtual void RemoveTangents()
    {
        tangents.clear();
        VertexType = VertexType & (~OBJ_TANGENTS);
    }

    virtual void RebuildBBox();

    // Transform all vertices by this matrix.
    // Also rebuilds the BBox.
    virtual void ApplyTransform(Matrix44<f3vec>& Mat);

    // Transform all texcoords by this matrix.
    virtual void ApplyTextureTransform(Matrix44<f3vec>& Mat);

    virtual size_t FaceCount() const { return indices.size() / 3u; }

    virtual size_t VertexCount() const { return verts.size(); }

    virtual size_t EdgeCount() const { return 0; } // This isn't tracked for RenderObject.
};

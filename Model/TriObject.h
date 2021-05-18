//////////////////////////////////////////////////////////////////////
// TriObject.h - Represent a single general begin/end pair with rendering state
// Typically triangles, but could be other primitive.
//
// Copyright David K. McAllister, July 1999.

// This data type is meant to be suitable for rendering. However, it used to use the double
// data type, so it was slower, plus it uses general triangles, rather than indexed triangles,
// so it's slower on Nvidia hardware.
// It has been superceded by RenderObject.
// The OBJ reader and writer handle TriObject, instead of Mesh, unfortunately.
// TriObject has some geometry processing capabilities, but I recommend instead to do
// all processing as a Mesh and then convert to RenderObject.

#pragma once

#include "Model/BaseObject.h"

#include <vector>

// Ripped directly from "GL/gl.h"
#define L_POINTS 0x0000
#define L_LINES 0x0001
#define L_LINE_LOOP 0x0002
#define L_LINE_STRIP 0x0003
#define L_TRIANGLES 0x0004
#define L_TRIANGLE_STRIP 0x0005
#define L_TRIANGLE_FAN 0x0006
#define L_QUADS 0x0007
#define L_QUAD_STRIP 0x0008
#define L_POLYGON 0x0009

struct TriObject : public BaseObject {
    int PrimType; // GL_TRIANGLE_STRIP, etc.

    std::vector<f3Vector> verts;
    std::vector<f3Vector> normals;        // Must have a length of 0, 1, or verts.size().
    std::vector<f3Vector> tangents;       // Must have a length of 0, 1, or verts.size().
    std::vector<f3Vector> texcoords;      // Must have a length of 0, 1, or verts.size().
    std::vector<f3Vector> dcolors;        // Must have a length of 0, 1, or verts.size().
    std::vector<f3Vector::ElType> alphas; // Must have a length of 0 or dcolors.size().

    DMC_DECL TriObject()
    {
        // INFO("Making TriObject.");
        PrimType = L_TRIANGLES;
        ObjectType = DMC_TRI_OBJECT;
    }

    void QuadsToTris(bool KeepBad = true);

    virtual void Dump() const;

    // Sets the OBJ_WHATEVER flag.
    virtual void GenColors() { ASSERT_R(0); }
    virtual void GenNormals();
    virtual void GenTexCoords() { ASSERT_R(0); }
    virtual void GenTangents();

    // Clears the OBJ_WHATEVER flag.
    virtual void RemoveColors()
    {
        dcolors.clear();
        alphas.clear();
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
    virtual void ApplyTransform(Matrix44<typename f3Vector::ElType>& Mat);

    // Transform all texcoords by this matrix.
    virtual void ApplyTextureTransform(Matrix44<typename f3Vector::ElType>& Mat);

    virtual size_t FaceCount() const
    {
        if (PrimType == L_TRIANGLES)
            return verts.size() / 3u;
        else
            return -1;
    }

    virtual size_t VertexCount() const { return verts.size(); }

    virtual size_t EdgeCount() const { return 0; } // This isn't tracked for TriObject.
};

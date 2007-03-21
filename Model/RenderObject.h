//////////////////////////////////////////////////////////////////////
// RenderObject.h - Represent a single general begin/end pair with rendering state.
//
// Copyright David K. McAllister, July 1999.

#ifndef render_object_h
#define render_object_h

#include <Model/BaseObject.h>

#include <vector>

struct RenderObject : public BaseObject
{
    std::vector<f3Vector> verts;
    std::vector<f3Vector> normals; // Must have a length of 0 or VertexCount.
    std::vector<f3Vector> tangents; // Must have a length of 0 or verts.size().
    std::vector<f3Vector> texcoords; // Must have a length of 0 or verts.size().
    std::vector<f3Vector> dcolors; // Must have a length of 0 or verts.size().
    std::vector<int> indices; // Pointers into the other lists.

    inline RenderObject()
    {
        // cerr << "Making RenderObject.\n";
        ObjectType = DMC_RENDER_OBJECT;
    }

    virtual void Dump() const;

    // Sets the OBJ_WHATEVER flag.
    virtual void GenColors() {ASSERT_R(0);}
    virtual void GenNormals() {ASSERT_R(0);}
    virtual void GenTexCoords() {ASSERT_R(0);}
    virtual void GenTangents() {ASSERT_R(0);}

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
    virtual void ApplyTransform(Matrix44 &Mat);

    // Transform all texcoords by this matrix.
    virtual void ApplyTextureTransform(Matrix44 &Mat);
};

#endif

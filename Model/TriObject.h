//////////////////////////////////////////////////////////////////////
// TriObject.h - Represent a single general begin/end pair with rendering state.
// Typically triangles, but could be other primitive.
//
// Copyright David K. McAllister, July 1999.

// This data type is meant to be suitable for rendering. However, it uses the double
// data type, so it's slower, plus it uses general triangles, rather than indexed triangles,
// so it's slower on Nvidia hardware.
// It has been superceded by RenderObject.
// The OBJ reader and writer handle TriObject, instead of Mesh, unfortunately.
// TriObject has some geometry processing capabililities, but I recommend instead to do
// all processing as a Mesh and then convert to RenderObject.

#ifndef tri_object_h
#define tri_object_h

#include <Model/BaseObject.h>

#include <vector>
using namespace std;

// Ripped directly from <GL/gl.h>
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

struct TriObject : public BaseObject
{
	int PrimType; // GL_TRIANGLE_STRIP, etc.

    vector<Vector> verts;
	vector<Vector> normals; // Must have a length of 0, 1, or verts.size().
	vector<Vector> tangents; // Must have a length of 0, 1, or verts.size().
	vector<Vector> texcoords; // Must have a length of 0, 1, or verts.size().
	vector<Vector> dcolors; // Must have a length of 0, 1, or verts.size().
	vector<double> alphas; // Must have a length of 0 or dcolors.size().
	
	inline TriObject()
	{
		// cerr << "Making TriObject.\n";
		PrimType = 4;
		ObjectType = DMC_TRI_OBJECT;
	}
	
	void QuadsToTris(bool KeepBad = true);

	virtual void Dump() const;

    // Sets the OBJ_WHATEVER flag.
    virtual void GenColors() {ASSERT0(0);}
    virtual void GenNormals();
    virtual void GenTexCoords() {ASSERT0(0);}
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
	
    // Transform all verticess by this matrix.
    // Also rebuilds the BBox.
	virtual void ApplyTransform(Matrix44 &Mat);
	
    // Transform all texcoords by this matrix.
	virtual void ApplyTextureTransform(Matrix44 &Mat);
};

#endif

//////////////////////////////////////////////////////////////////////
// BaseObject.h - Container class for all kinds of objects.
//
// Copyright David K. McAllister, Dec. 2001.

#ifndef base_object_h
#define base_object_h

#include <Model/TextureDB.h>
#include <Math/BBox.h>
#include <Math/Matrix44.h>
#include <Math/Vector.h>

#include <vector>
using namespace std;

// These should be true if the data is valid, i.e. intended to be used.
#define OBJ_NONE 0
#define OBJ_COLORS 1
#define OBJ_NORMALS 2
#define OBJ_TEXCOORDS 4
#define OBJ_TANGENTS 8
#define OBJ_ALL (OBJ_COLORS | OBJ_NORMALS | OBJ_TEXCOORDS | OBJ_TANGENTS)

typedef enum {DMC_BASE_OBJECT, DMC_TRI_OBJECT, DMC_MESH_OBJECT, DMC_RENDER_OBJECT} ObjectTypes;

struct BaseObject
{
	char Name[64];
    int DispLists[OBJ_ALL+1]; // So you can have a disp list for each set of attribs.
	Matrix44 Transform; // The current transformation.
	Matrix44 TexTransform; // The current texture transformation.
	BBox Box;
    Vector dcolor; // The object-wide colors.
    double alpha;
	Vector scolor, ecolor, acolor;
	double shininess, creaseAngle; // Used for later generation of normals.
	int ObjID; // OpenGL display list ID.
	TexInfo *TexPtr; // Index into the texture database.
    ObjectTypes ObjectType; // My cheat for RTTI.
	bool DColorValid, AlphaValid, SColorValid, EColorValid, AColorValid, ShininessValid; // Are object-wide values valid?
    bool CullBack; // Rendering instructions.
    bool RenderOnOff; // Toggle whether to display this object or not.
    bool RenderAsSBRDF; // Application specific, obviously.
    float RenderPriority; // Use this to order the objects in some way. Used by operator<.

    // These counts are mainly for user information.
    int FaceCount, VertexCount, EdgeCount;
    
    // This is a mask of bits regarding the fields of the mesh
    // elements.  See the #defines in AElements.h and (and maybe other
    // user-defined ones like QElements.h) Some bits tell what
    // elements exist. Other bits tell what elements are valid or used.
    unsigned int FaceType, VertexType, EdgeType;
	
	inline BaseObject()
	{
		// cerr << "Making BaseObject.\n";
        ObjectType = DMC_BASE_OBJECT;
		Name[0] = '\0';
		TexPtr = NULL;
        ObjID = -1;
		SColorValid = EColorValid = AColorValid = false;
		DColorValid = ShininessValid = AlphaValid = false;
		alpha = 1.0;
		creaseAngle = M_PI * 0.5;
		shininess = 32.0;
		dcolor = Vector(0,1,0);
		scolor = Vector(0,0,0);
		ecolor = Vector(0,0,0);
		acolor = Vector(0.2,0.2,0.2);
        EdgeCount = VertexCount = FaceCount = 0;
        EdgeType = VertexType = FaceType = OBJ_NONE;
        CullBack = true;
        RenderOnOff = true;
        RenderAsSBRDF = false;
        RenderPriority = 1000;
        for(int i=0; i<OBJ_ALL+1; i++)
            DispLists[i] = -1;
	}

	virtual void Dump() const = 0;
	
	virtual void GenColors() = 0;
	virtual void GenNormals() = 0;
	virtual void GenTexCoords() = 0;
	virtual void GenTangents() = 0;
	
	virtual void RemoveColors() = 0;
	virtual void RemoveNormals() = 0;
	virtual void RemoveTexCoords() = 0;
	virtual void RemoveTangents() = 0;
	
	virtual void RebuildBBox() = 0;
	
	virtual void ApplyTransform(Matrix44 &Mat) = 0;
	
    // Transform all texcoords by this matrix.
	virtual void ApplyTextureTransform(Matrix44 &Mat) = 0;
};

#endif

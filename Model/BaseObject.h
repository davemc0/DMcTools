//////////////////////////////////////////////////////////////////////
// BaseObject.h - Container class for all kinds of 3D objects
//
// Copyright David K. McAllister, Dec. 2001.

#pragma once

#include "Model/TextureDB.h"
#include "Math/BBox.h"
#include "Math/Matrix44.h"
#include "Math/Vector.h"

#include <vector>

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
    Matrix44<typename f3Vector::ElType> Transform; // The current transformation.
    Matrix44<typename f3Vector::ElType> TexTransform; // The current texture transformation.
    BBox<f3Vector> Box;
    f3Vector dcolor; // The object-wide colors.
    f3Vector::ElType alpha;
    f3Vector scolor, ecolor, acolor;
    f3Vector::ElType shininess, creaseAngle; // Used for later generation of normals.
    int ObjID; // OpenGL display list ID.
    TexInfo *TexPtr; // Index into the texture database.
    ObjectTypes ObjectType; // My cheat for RTTI.
    bool DColorValid, AlphaValid, SColorValid, EColorValid, AColorValid, ShininessValid; // Are object-wide values valid?
    bool CullBack; // Rendering instructions.
    bool RenderOnOff; // Toggle whether to display this object or not.
    bool RenderAsSBRDF; // Application specific, obviously.
    f3Vector::ElType RenderPriority; // Use this to order the objects in some way. Used by operator<.

    // This is a mask of bits regarding the fields of the mesh
    // elements.  See the #defines in AElements.h and (and maybe other
    // user-defined ones like QElements.h) Some bits tell what
    // elements exist. Other bits tell what elements are valid or used.
    unsigned int FaceType, VertexType, EdgeType;

    DMC_DECL BaseObject()
    {
        //INFO("Making BaseObject.");
        ObjectType = DMC_BASE_OBJECT;
        Name[0] = '\0';
        TexPtr = NULL;
        ObjID = -1;
        SColorValid = EColorValid = AColorValid = false;
        DColorValid = ShininessValid = AlphaValid = false;
        alpha = 1.0;
        creaseAngle = M_PI * 0.5;
        shininess = 32.0;
        dcolor = f3Vector(0,1,0);
        scolor = f3Vector(0,0,0);
        ecolor = f3Vector(0,0,0);
        acolor = f3Vector(0.2,0.2,0.2);
        EdgeType = VertexType = FaceType = OBJ_NONE;
        CullBack = true;
        RenderOnOff = true;
        RenderAsSBRDF = false;
        RenderPriority = 1000;
        for(int i=0; i<OBJ_ALL+1; i++)
            DispLists[i] = -1;
    }

    virtual ~BaseObject()
    {
        // INFO("~BaseObject()");
    }

    virtual void Dump() const = 0;

    // These counts are mainly for user information.
    virtual size_t FaceCount() const = 0;
    virtual size_t VertexCount() const = 0;
    virtual size_t EdgeCount() const = 0;

    virtual void GenColors() = 0;
    virtual void GenNormals() = 0;
    virtual void GenTexCoords() = 0;
    virtual void GenTangents() = 0;

    virtual void RemoveColors() = 0;
    virtual void RemoveNormals() = 0;
    virtual void RemoveTexCoords() = 0;
    virtual void RemoveTangents() = 0;

    virtual void RebuildBBox() = 0;

    virtual void ApplyTransform(Matrix44<typename f3Vector::ElType> &Mat) = 0;

    // Transform all texcoords by this matrix.
    virtual void ApplyTextureTransform(Matrix44<typename f3Vector::ElType> &Mat) = 0;
};

//////////////////////////////////////////////////////////////////////
// LoadVRML.h - Load a VRML 1.0 file into a Model
//
// Copyright David K. McAllister, July 1999.

#pragma once

#include "Model/TriObject.h"

#include <cstdlib>

#ifdef DMC_MACHINE_win
// This gets alloca() for Bison.
#include <malloc.h>
#endif

#define YY_NEVER_INTERACTIVE 1
#define YY_SKIP_YYWRAP 1
extern int yywrap();

#define PER_VERTEX_INDEXED 1
#define PER_FACE_INDEXED 2
#define PER_VERTEX 3
#define PER_FACE 4
#define OVERALL 5

// "Configuration" definitions for compiler
#define VRML_ATOF(a) (atof(a))
#define VRML_ATOI(a) (atoi(a))

// This object contains all the code necessary to parse an object from a VRML file.
struct YYObject : public TriObject
{
    f3Vector Translation; // Used only in a Transform or Texture2Transform node.
    f3Vector::ElType ScaleOrientationAngle; // Used only in a Transform or Texture2Transform node.
    f3Vector ScaleOrientationAxis; // Used only in a Transform or Texture2Transform node.
    f3Vector::ElType RotationAngle; // Used only in a Transform or Texture2Transform node.
    f3Vector RotationAxis; // Used only in a Transform or Texture2Transform node.
    f3Vector Scale; // Used only in a Transform or Texture2Transform node.
    f3Vector Center; // Used only in a Transform or Texture2Transform node.
    f3Vector::ElType ObjWidth, ObjHeight, ObjDepth, ObjRadius; // Used for making cubes, cylinders, etc..

    int MaterialBinding, NormalBinding;
    bool InTexture2Transform, InTransform;

    std::vector<int> MaterialIndices;
    std::vector<int> NormalIndices;
    std::vector<int> TexCoordIndices;
    std::vector<int> VertexIndices;

    YYObject()
    {
        creaseAngle = 0.5; // Radians
        MaterialBinding = OVERALL;
        NormalBinding = OVERALL;
        InTransform = InTexture2Transform = false;
    }
};

// These are all given to BisonMe.y.
extern void s_Vertices(std::vector<f3Vector> *);
extern void s_Normals(std::vector<f3Vector> *);
extern void s_TexCoords(std::vector<f3Vector> *);
extern void s_AmbientColors(std::vector<f3Vector> *);
extern void s_DiffuseColors(std::vector<f3Vector> *);
extern void s_EmissiveColors(std::vector<f3Vector> *);
extern void s_Shininesses(std::vector<f3Vector::ElType> *);
extern void s_SpecularColors(std::vector<f3Vector> *);
extern void s_Transparencies(std::vector<f3Vector::ElType> *);

extern void s_CoordIndices(std::vector<int> *);
extern void s_MaterialIndices(std::vector<int> *);
extern void s_NormalIndices(std::vector<int> *);
extern void s_TexCoordIndices(std::vector<int> *);

extern void s_NormalBinding(int Binding);
extern void s_MaterialBinding(int Binding);

extern void s_CreaseAngle(f3Vector::ElType);
extern void s_OutputIndexedFaceSet();
extern void s_Texture2_filename(const char *);
extern void s_Info(char *);
extern void s_Matrix(f3Vector::ElType *mat);
extern void s_Center(f3Vector *);
extern void s_Scale(f3Vector *);
extern void s_Rotation(f3Vector::ElType, f3Vector * = NULL);
extern void s_Translation(f3Vector *);
extern void s_DEF_name(char *objname);
extern void s_Separator_begin();
extern void s_Separator_end();
extern void s_Tex2Tran_begin();
extern void s_Tex2Tran_end();
extern void s_Transform_begin();
extern void s_Transform_end();
extern void s_InitCube();
extern void s_EndCube();
extern void s_InitLight(int);
extern void s_EndLight();
extern void s_LightParam1(f3Vector::ElType, int);
extern void s_LightParam3(f3Vector *, int);
extern void s_InitCamera(int);
extern void s_EndCamera();
extern void s_CameraParam1(f3Vector::ElType, int);
extern void s_CameraParam3(f3Vector *, f3Vector::ElType, int);
extern void s_InitCylinder();
extern void s_EndCylinder();
extern void s_ObjWidth(f3Vector::ElType);
extern void s_ObjHeight(f3Vector::ElType);
extern void s_ObjDepth(f3Vector::ElType);
extern void s_ObjRadius(f3Vector::ElType);

extern void yyerror(const char *s);

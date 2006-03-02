//////////////////////////////////////////////////////////////////////
// LoadVRML.h - Load a VRML 1.0 file into a Model.
//
// Copyright David K. McAllister, July 1999.

#ifndef _readvrml_h
#define _readvrml_h

#include <Model/TriObject.h>

#include <stdlib.h>

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
	Vector Translation; // Used only in a Transform or Texture2Transform node.
	double ScaleOrientationAngle; // Used only in a Transform or Texture2Transform node.
	Vector ScaleOrientationAxis; // Used only in a Transform or Texture2Transform node.
	double RotationAngle; // Used only in a Transform or Texture2Transform node.
	Vector RotationAxis; // Used only in a Transform or Texture2Transform node.
	Vector Scale; // Used only in a Transform or Texture2Transform node.
	Vector Center; // Used only in a Transform or Texture2Transform node.
	double ObjWidth, ObjHeight, ObjDepth, ObjRadius; // Used for making cubes, cylinders, etc..

	int MaterialBinding, NormalBinding;
	bool InTexture2Transform, InTransform;
	
	vector<int> MaterialIndices;
	vector<int> NormalIndices;
	vector<int> TexCoordIndices;
	vector<int> VertexIndices;
	
	YYObject()
	{
		creaseAngle = 0.5; // Radians
		MaterialBinding = OVERALL;
		NormalBinding = OVERALL;
		InTransform = InTexture2Transform = false;
	}
};

// These are all given to BisonMe.y.
extern void s_Vertices(vector<Vector> *);
extern void s_Normals(vector<Vector> *);
extern void s_TexCoords(vector<Vector> *);
extern void s_AmbientColors(vector<Vector> *);
extern void s_DiffuseColors(vector<Vector> *);
extern void s_EmissiveColors(vector<Vector> *);
extern void s_Shininesses(vector<double> *);
extern void s_SpecularColors(vector<Vector> *);
extern void s_Transparencies(vector<double> *);

extern void s_CoordIndices(vector<int> *);
extern void s_MaterialIndices(vector<int> *);
extern void s_NormalIndices(vector<int> *);
extern void s_TexCoordIndices(vector<int> *);

extern void s_NormalBinding(int Binding);
extern void s_MaterialBinding(int Binding);

extern void s_CreaseAngle(float);
extern void s_OutputIndexedFaceSet();
extern void s_Texture2_filename(char *);
extern void s_Info(char *);
extern void s_Matrix(double *mat);
extern void s_Center(Vector *);
extern void s_Scale(Vector *);
extern void s_Rotation(float, Vector * = NULL);
extern void s_Translation(Vector *);
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
extern void s_LightParam1(float, int);
extern void s_LightParam3(Vector *, int);
extern void s_InitCamera(int);
extern void s_EndCamera();
extern void s_CameraParam1(float, int);
extern void s_CameraParam3(Vector *, float, int);
extern void s_InitCylinder();
extern void s_EndCylinder();
extern void s_ObjWidth(float);
extern void s_ObjHeight(float);
extern void s_ObjDepth(float);
extern void s_ObjRadius(float);

extern void yyerror(char *s);

#endif

//////////////////////////////////////////////////////////////////////
// LoadVRML.cpp - Parse most of VRML 1.0 and return it as a Model
//
// Changes Copyright David K. McAllister, July 1999.
// Originally written by Bill Mark, June 1998.

#include "Model/LoadVRML.h"
#include "Model/Mesh.h"
#include "Model/AElements.h"
#include "Model/Model.h"
#include "Model/BisonMe.h"
#include "Model/LightDB.h"
#include "Model/CameraDB.h"

#include <cstdio>
#include <cstring>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Prototypes

extern int my_linecount; // number of lines read. Maintained by bison.
extern char my_linetext[]; // text of the current line, used in bison.

extern int yyparse(void); // The function that bison makes.
extern int yynerrs; // Number of errors so far, maintained by bison.

//////////////////////////////////////////////////////////////////////
// Global variables

FILE *InFile; // the flex code looks at this.

static vector<YYObject> Stack; // The state stack.
// When we pop the stack we set this if the thing we're finishing has a name field.
// Then when the DEF happens, it writes it here.
static char **ThingToName = NULL;
static char *PlaceToName = NULL;
static bool CopyNameThing; // If true, it's an object, so copy the name instead.
static Model *thisModel = NULL;
static unsigned int ReqAttribs = OBJ_NONE, AccAttribs = OBJ_ALL;

extern LightDB LitDB;
extern CameraDB CamDB;

//////////////////////////////////////////////////////////////////////
// Callback routines from bison
//////////////////////////////////////////////////////////////////////

// This is libfl.a
#if 1
int yywrap()
{
    return 1;
}
#endif

// Error handler for bison
void yyerror(const char *s)
{
    fprintf(stderr, "* %s, on line %i:\n", s, my_linecount);
    fprintf(stderr, "%s\n", my_linetext); // str includes newline, add one
    ASSERT_R(0);
}

// Handle info field.
void s_Info(char *infostr)
{
    //fprintf(stderr, "# VRML_INFO_FIELD: %s\n", infostr);
}

void s_Separator_begin()
{
    // Copy the top element.
    Stack.push_back(Stack.back());
    // fprintf(stderr, "Pushing the stack: %d\n", Stack.size());
}

void s_DEF_name(char *objname)
{
    if(objname) {
        // cerr << "N: " << objname << endl;
        // Name something.
        if(CopyNameThing && PlaceToName) {
            strncpy(PlaceToName, objname, 63);
            PlaceToName = NULL;
        } else if(ThingToName) {
            *ThingToName = strdup(objname);
            ThingToName = NULL;
        }
    }
}

void s_Separator_end()
{
    ASSERT_RM(Stack.size() > 1, "LoadVRML: Internal Separator{} stack bug.");

    Stack.pop_back();
}

void s_Tex2Tran_begin()
{
    YYObject &S = Stack.back();

    ASSERT_RM((!S.InTexture2Transform) && (!S.InTransform), "Shouldn't be in a Texture2Transform.");

    S.InTexture2Transform = true;
    S.RotationAngle = 0;
    S.RotationAxis = f3Vector(0,0,1);
    S.Translation = f3Vector(0,0,0);
    S.Center = f3Vector(0,0,0);
    S.Scale = f3Vector(1,1,1);
}

void s_Tex2Tran_end()
{
    YYObject &S = Stack.back();

    ASSERT_RM(S.InTexture2Transform, "Should be in a Texture2Transform.");

    S.InTexture2Transform = false;

    // Make the texture transformation matrix.
    S.TexTransform.LoadIdentity();
    S.TexTransform.Translate(S.Translation);
    S.TexTransform.Translate(S.Center);
    S.TexTransform.Rotate(S.RotationAngle, S.RotationAxis);
    S.TexTransform.Scale(S.Scale);
    S.TexTransform.Translate(-S.Center);
}

void s_Transform_begin()
{
    YYObject &S = Stack.back();

    ASSERT_RM((!S.InTexture2Transform) && (!S.InTransform), "Shouldn't be in a Texture2Transform.");

    S.InTransform = true;
    S.RotationAngle = 0;
    S.RotationAxis = f3Vector(0,0,1);
    S.ScaleOrientationAngle = 0;
    S.ScaleOrientationAxis = f3Vector(0,0,1);
    S.Translation = f3Vector(0,0,0);
    S.Center = f3Vector(0,0,0);
    S.Scale = f3Vector(1,1,1);
}

void s_Transform_end()
{
    YYObject &S = Stack.back();

    ASSERT_RM(S.InTransform, "Should be in a Texture2Transform.");

    S.InTransform = false;

    // Make the texture transformation matrix.
    // S.Transform.LoadIdentity();
    S.Transform.Translate(S.Translation);
    S.Transform.Translate(S.Center);
    S.Transform.Rotate(S.RotationAngle, S.RotationAxis);
    S.Transform.Rotate(S.ScaleOrientationAngle, S.ScaleOrientationAxis);
    S.Transform.Scale(S.Scale);
    S.Transform.Rotate(-S.ScaleOrientationAngle, -S.ScaleOrientationAxis);
    S.Transform.Translate(-S.Center);
}

void s_CreaseAngle(f3Vector::ElType cr)
{
    YYObject &S = Stack.back();

    S.creaseAngle = cr;
}

void s_InitCylinder()
{
    s_Separator_begin();
    YYObject &S = Stack.back();

    S.ObjRadius = 1;
    S.ObjHeight = 2;
}

// XXX Doesn't generate endcaps.
// Texcoords range 0 to 1 top to bottom and around it.
void s_EndCylinder()
{
    YYObject &S = Stack.back();

    // XXX Bug: We are replacing the current vertex list.
    S.verts.clear();
    S.VertexIndices.clear();

    S.texcoords.clear();
    S.TexCoordIndices.clear();

    f3Vector P0(cos(0.) * S.ObjRadius, sin(0.) * S.ObjRadius, S.ObjHeight * 0.5);
    S.verts.push_back(P0);
    S.verts.push_back(f3Vector(P0.x, P0.y, -P0.z));

    S.texcoords.push_back(f3Vector(0,0,0));
    S.texcoords.push_back(f3Vector(1,0,0));

#define NUM_CYL_SIDES 32
    int i=1;
    for(i=1; i<NUM_CYL_SIDES; i++) {
        f3Vector::ElType t = i / f3Vector::ElType(NUM_CYL_SIDES);
        f3Vector P(cos(M_PI * 2.0 * t) * S.ObjRadius,
            sin(M_PI * 2.0 * t) * S.ObjRadius, S.ObjHeight * 0.5);
        S.verts.push_back(P);
        S.verts.push_back(f3Vector(P.x, P.y, -P.z));

        S.VertexIndices.push_back((i-1)*2);
        S.VertexIndices.push_back((i-1)*2+1);
        S.VertexIndices.push_back(i*2);
        S.VertexIndices.push_back(-1);
        S.VertexIndices.push_back(i*2);
        S.VertexIndices.push_back((i-1)*2+1);
        S.VertexIndices.push_back(i*2+1);
        S.VertexIndices.push_back(-1);

        S.texcoords.push_back(f3Vector(0,t,0));
        S.texcoords.push_back(f3Vector(1,t,0));

        S.TexCoordIndices.push_back((i-1)*2);
        S.TexCoordIndices.push_back((i-1)*2+1);
        S.TexCoordIndices.push_back(i*2);
        S.TexCoordIndices.push_back(-1);
        S.TexCoordIndices.push_back(i*2);
        S.TexCoordIndices.push_back((i-1)*2+1);
        S.TexCoordIndices.push_back(i*2+1);
        S.TexCoordIndices.push_back(-1);

    }

    S.VertexIndices.push_back((i-1)*2);
    S.VertexIndices.push_back((i-1)*2+1);
    S.VertexIndices.push_back(0*2);
    S.VertexIndices.push_back(-1);
    S.VertexIndices.push_back(0*2);
    S.VertexIndices.push_back((i-1)*2+1);
    S.VertexIndices.push_back(0*2+1);
    S.VertexIndices.push_back(-1);

    S.TexCoordIndices.push_back((i-1)*2);
    S.TexCoordIndices.push_back((i-1)*2+1);
    S.TexCoordIndices.push_back(0*2);
    S.TexCoordIndices.push_back(-1);
    S.TexCoordIndices.push_back(0*2);
    S.TexCoordIndices.push_back((i-1)*2+1);
    S.TexCoordIndices.push_back(0*2+1);
    S.TexCoordIndices.push_back(-1);

    s_OutputIndexedFaceSet();
    s_Separator_end();
}

void s_InitCube()
{
    s_Separator_begin();
    YYObject &S = Stack.back();

    S.ObjWidth = S.ObjHeight = S.ObjDepth = 2;
}

void s_EndCube()
{
    YYObject &S = Stack.back();

    S.ObjWidth *= 0.5;
    S.ObjHeight *= 0.5;
    S.ObjDepth *= 0.5;

    // XXX Bug: We are replacing the current vertex list.
    S.verts.clear();
    S.verts.push_back(f3Vector(-S.ObjWidth, -S.ObjHeight, -S.ObjDepth));
    S.verts.push_back(f3Vector(-S.ObjWidth, -S.ObjHeight, S.ObjDepth));
    S.verts.push_back(f3Vector(-S.ObjWidth, S.ObjHeight, -S.ObjDepth));
    S.verts.push_back(f3Vector(-S.ObjWidth, S.ObjHeight, S.ObjDepth));
    S.verts.push_back(f3Vector(S.ObjWidth, -S.ObjHeight, -S.ObjDepth));
    S.verts.push_back(f3Vector(S.ObjWidth, -S.ObjHeight, S.ObjDepth));
    S.verts.push_back(f3Vector(S.ObjWidth, S.ObjHeight, -S.ObjDepth));
    S.verts.push_back(f3Vector(S.ObjWidth, S.ObjHeight, S.ObjDepth));

    S.VertexIndices.clear();
    int listi[] = {1, 3, 7, 5, -1, 0, 2, 6, 4, -1,
        4, 5, 7, 6, -1, 0, 1, 3, 2, -1,
        2, 3, 7, 6, -1, 0, 1, 5, 4, -1};

    for(int i=0; i<5*3*2; i++)
        S.VertexIndices.push_back(listi[i]);

    s_OutputIndexedFaceSet();
    s_Separator_end();
}

// Lights are just added to a global list of lights.
void s_InitLight(int LightType)
{
    LightInfo *L = LitDB.Add();
    L->LightType = LightType;
    ThingToName = &L->LightName;
    CopyNameThing = false;
}

void s_EndLight()
{
}

void s_LightParam1(f3Vector::ElType f, int p)
{
    LightInfo *L = LitDB.LightList.back();
    if(p == 1)
        L->Enabled = f!=0.0f;
    else if(p == 2)
        L->Intensity = f;
    else if(p == 6)
        L->DropOffRate = f;
    else if(p == 7)
        L->CutOffAngle = f;
}

void s_LightParam3(f3Vector *v, int p)
{
    LightInfo *L = LitDB.LightList.back();
    if(p == 3)
        L->Color = *v;
    if(p == 4) {
        L->Position = *v;
        L->Position.normalize();
    }
    if(p == 5) {
        L->Position = *v;
    }
}

// Cameras are just added to a global list of Cameras.
void s_InitCamera(int CameraType)
{
    CameraInfo *L = CamDB.Add();
    L->CameraType = CameraType;
    ThingToName = (char **)&L->CameraName;
    CopyNameThing = false;
}

void s_EndCamera()
{
}

void s_CameraParam1(f3Vector::ElType f, int p)
{
    CameraInfo *L = CamDB.CameraList.back();
    if(p == 3)
        L->FocalDistance = f;
    else if(p == 4)
        L->HeightValue = f;
}

void s_CameraParam3(f3Vector *v, f3Vector::ElType f, int p)
{
    CameraInfo *L = CamDB.CameraList.back();
    if(p == 1)
        L->Position = *v;
    if(p == 2) {
        L->Orientation = *v;
        L->OrientationTh = f;
    }
}

void s_ObjRadius(f3Vector::ElType v)
{
    YYObject &S = Stack.back();

    S.ObjRadius = v;
}

void s_ObjWidth(f3Vector::ElType v)
{
    YYObject &S = Stack.back();

    S.ObjWidth = v;
}

void s_ObjHeight(f3Vector::ElType v)
{
    YYObject &S = Stack.back();

    S.ObjHeight = v;
}

void s_ObjDepth(f3Vector::ElType v)
{
    YYObject &S = Stack.back();

    S.ObjDepth = v;
}

// MatrixTransform
void s_Matrix(f3Vector::ElType *mat)
{
    Matrix44<typename f3Vector::ElType> Right(mat, true);

    YYObject &S = Stack.back();
    S.Transform *= Right;
}

// Scale
void s_Scale(f3Vector *s)
{
    YYObject &S = Stack.back();

    if(S.InTexture2Transform)
        S.Scale = *s;
    else if(S.InTransform)
        S.Scale = *s;
    else
        S.Transform.Scale(*s);
}

// Rotation
void s_Rotation(f3Vector::ElType angle, f3Vector *axis)
{
    YYObject &S = Stack.back();

    if(S.InTexture2Transform)
        S.RotationAngle = angle;
    else if(S.InTransform)
    {
        S.RotationAngle = angle;
        S.RotationAxis = *axis;
    }
    else
        S.Transform.Rotate(angle, *axis);
}

// Translation
void s_Translation(f3Vector *t)
{
    YYObject &S = Stack.back();

    if(S.InTexture2Transform)
        S.Translation = *t;
    else if(S.InTransform)
        S.Translation = *t;
    else
        S.Transform.Translate(*t);
}

// Center
void s_Center(f3Vector *c)
{
    YYObject &S = Stack.back();

    if(S.InTexture2Transform)
        S.Center = *c;
    else if(S.InTransform)
        S.Center = *c;
    else
        ASSERT_R(0);
}

// Load the specified texture.
void s_Texture2_filename(const char *texFName)
{
    YYObject &S = Stack.back();

    if(texFName == NULL || texFName[0] == '\0') {
        // fprintf(stderr, "Texuring turned off.\n");
        S.TexPtr = NULL;
        return;
    }

#if 0
    // XXX Replace with a hack to distinguish an SBRDF from a standard texture.
    // XXX - Hack for Manuel's stuff
    for(int k=0; texFName[k]; k++) {
        if(texFName[k] == '.') {
            texFName[k+1] = 't';
            texFName[k+2] = 'i';
            texFName[k+3] = 'f';
            break;
        } else if(texFName[k] >= 'A' && texFName[k] <= 'Z') {
            texFName[k] += 32;
        }
    }
#endif

    S.TexPtr = Model::TexDB.FindByNameOrAdd(texFName);
}

// Save x,y,z coordinates
void s_Vertices(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.verts = *V;
}

// Save normals
void s_Normals(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.normals = *V;
}

// Save texture coordinates
void s_TexCoords(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.texcoords = *V;
}

// Compute the average of all the colors listed
// since we don't store them per-vertex except diffuse.
static f3Vector AvgColor(vector<f3Vector> *V)
{
    f3Vector A(0,0,0);
    int i;
    for(i=0; i<(int)V->size(); i++)
        A += (*V)[i];
    return A / f3Vector::ElType(i);
}

// Average ambient colors.
void s_AmbientColors(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.acolor = AvgColor(V);
    S.AColorValid = true;
}

// Save diffuse colors.
void s_DiffuseColors(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.dcolors = *V;
    S.dcolor = AvgColor(V);
    S.DColorValid = true;
}

// Average emissive colors.
void s_EmissiveColors(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.ecolor = AvgColor(V);
    S.EColorValid = true;
}

// Average shininesses.
void s_Shininesses(vector<f3Vector::ElType> *V)
{
    YYObject &S = Stack.back();
    S.shininess = 0;
    int i;
    for(i=0; i<(int)V->size(); i++)
        S.shininess += (*V)[i];
    S.shininess /= f3Vector::ElType(i);
    S.shininess *= 128.0; // Convert to OpenGL style.
    if(S.shininess >= 1.0 && S.shininess <= 127.0)
        S.ShininessValid = true;
}

// Average specular colors.
void s_SpecularColors(vector<f3Vector> *V)
{
    YYObject &S = Stack.back();
    S.scolor = AvgColor(V);
    S.SColorValid = true;
}

void s_Transparencies(vector<f3Vector::ElType> *V)
{
    // YYObject &S =
    YYObject& S = Stack.back();
}

void s_CoordIndices(vector<int> *V)
{
    YYObject &S = Stack.back();
    S.VertexIndices = *V;
}

void s_MaterialIndices(vector<int> *V)
{
    YYObject &S = Stack.back();
    S.MaterialIndices = *V;
}

void s_NormalIndices(vector<int> *V)
{
    YYObject &S = Stack.back();
    S.NormalIndices = *V;
}

void s_TexCoordIndices(vector<int> *V)
{
    YYObject &S = Stack.back();
    S.TexCoordIndices = *V;
}

void s_MaterialBinding(int Binding)
{
    YYObject &S = Stack.back();
    S.MaterialBinding = Binding;
}

void s_NormalBinding(int Binding)
{
    YYObject &S = Stack.back();
    S.NormalBinding = Binding;
}

// cii is coordinate index index.
// vic is vertex index count.
// Returns the index of the vertex, since it may change.
static int DoVertex(Mesh &B, YYObject &S, vector<AVertex *> &VertPtrs, int cii, int vic, int FaceNum, AFace *FF)
{
    int ind = S.VertexIndices[cii]; // The index into the list of vertices.
    ASSERT_RM(ind < (int)S.verts.size() && ind >= 0, "Vertex index out of range.");

    f3Vector ObjPos = S.verts[ind];
    f3Vector Vp = S.Transform * ObjPos;

    unsigned int VertData = 0;
    AVertex *Vrt = new AVertex;
    Vrt->V = Vp;
    B.Box += Vp;

    // cerr << ind << Vp << endl;

    if((int)S.TexCoordIndices.size() > 0) {
        ASSERT_RM(S.TexCoordIndices[cii] >= 0 && S.TexCoordIndices[cii] < (int)S.texcoords.size(),
            "texcoord index out of range");
        Vrt->Tex = S.TexTransform * S.texcoords[S.TexCoordIndices[cii]];
        VertData = VertData | OBJ_TEXCOORDS;
    }

    switch(S.NormalBinding)
    {
    case PER_VERTEX:
        // The vertex number indexes the Normal.
        ASSERT_RM(vic >= 0 && vic < (int)S.normals.size(), "normal out of range");
        Vrt->Nor = S.normals[vic];
        VertData = VertData | OBJ_NORMALS;
        B.VertexType = B.VertexType | OBJ_NORMALS;
        break;
    case PER_VERTEX_INDEXED:
        // The vertex number indexes the Normal indices, which index the Normal.
        ASSERT_RM(S.NormalIndices[cii] >= 0 && S.NormalIndices[cii] < (int)S.normals.size(),
            "normal index out of range");
        Vrt->Nor = S.normals[S.NormalIndices[cii]];
        VertData = VertData | OBJ_NORMALS;
        B.VertexType = B.VertexType | OBJ_NORMALS;
        break;
    case PER_FACE:
        // The face number indexes the Normal.
        // Replicate the normals.
        ASSERT_RM(FaceNum >= 0 && FaceNum < (int)S.normals.size(), "normal face num out of range");
        FF->Nor = S.normals[FaceNum];
        B.FaceType = B.FaceType | OBJ_NORMALS;
        break;
    case PER_FACE_INDEXED:
        // The face number indexes the Normal indices, which index the Normal.
        ASSERT_RM(FaceNum >= 0 && FaceNum < (int)S.NormalIndices.size(), "normal face num out of range");
        ASSERT_RM(S.NormalIndices[FaceNum] >= 0 && S.NormalIndices[FaceNum] < (int)S.normals.size(),
            "normal face index out of range");
        FF->Nor = S.normals[S.NormalIndices[FaceNum]];
        B.FaceType = B.FaceType | OBJ_NORMALS;
        break;
    case OVERALL:
        if((int)S.normals.size() > 0) {
            VertPtrs[ind]->Nor = S.normals[0];
            FF->Nor = S.normals[FaceNum];
            B.FaceType = B.FaceType | OBJ_NORMALS;
        }
        break;
    default:
        ASSERT_RM(0, "Unknown normal binding.");
        break;
    }

    switch(S.MaterialBinding)
    {
    case PER_VERTEX:
        // The vertex number indexes the color.
        ASSERT_RM(vic >= 0 && vic < (int)S.dcolors.size(), "Dcolor out of range");
        Vrt->Col = S.dcolors[vic];
        VertData = VertData | OBJ_COLORS;
        B.VertexType = B.VertexType | OBJ_COLORS;
        break;
    case PER_VERTEX_INDEXED:
        // The vertex number indexes the material indices, which index the color.
        ASSERT_RM(S.MaterialIndices[cii] >= 0 && S.MaterialIndices[cii] < (int)S.dcolors.size(),
            "Dcolor index out of range");
        Vrt->Col = S.dcolors[S.MaterialIndices[cii]];
        VertData = VertData | OBJ_COLORS;
        B.VertexType = B.VertexType | OBJ_COLORS;
        break;
    case PER_FACE:
        // The face number indexes the color.
        // Replicate the dcolors.
        ASSERT_RM(FaceNum >= 0 && FaceNum < (int)S.dcolors.size(), "Dcolor face num out of range");
        FF->Col = S.dcolors[FaceNum];
        B.FaceType = B.FaceType | OBJ_COLORS;
        break;
    case PER_FACE_INDEXED:
        // The face number indexes the material indices, which index the color.
        ASSERT_RM(FaceNum >= 0 && FaceNum < (int)S.MaterialIndices.size(), "Dcolor face num out of range");
        ASSERT_RM(S.MaterialIndices[FaceNum] >= 0 && S.MaterialIndices[FaceNum] < (int)S.dcolors.size(),
            "Dcolor face index out of range");
        FF->Col = S.dcolors[S.MaterialIndices[FaceNum]];
        B.FaceType = B.FaceType | OBJ_COLORS;
        break;
    case OVERALL:
        if((int)S.dcolors.size() > 0)
            B.dcolor = S.dcolors[0];
        if((int)S.alphas.size() > 0)
            B.alpha = S.alphas[0];
        break;
    default:
        ASSERT_RM(0, "Unknown dcolor binding.");
        break;
    }

    // Insert the vertex into the mesh. VertData tells what data pieces it has.
    // If these are the same as the data in VertPtrs[ind], just return.
    // If not, add this vertex to the end of VertPtrs and return its index.
    if(VertPtrs[ind] == NULL) {
        // Haven't used this vertex index before. Just add it.
        Vrt->Tan.y = -1; // No other vertices match.
        B.AddVertex(Vrt->V, Vrt); // Add it to the Mesh's vertex list.
        VertPtrs[ind] = Vrt;
    } else {
        // This vertex has been accessed before. Does ours have the same data?
        // AVertex *OldV = VertPtrs[ind];
        // The data is the same. Bail.

        // The data is different. Add a new vertex.
        // Step through a crazily linked list of shared vertices.
        int ifoo;
        for(ifoo = ind; ifoo >= 0; ifoo = (int)VertPtrs[ifoo]->Tan.y) {
            // Only compare accepted attribs to avoid superfluous splits.
            if(VertexDataSame(VertPtrs[ifoo], Vrt, VertData & AccAttribs)) {
                // Found it. Do nothing.
                ind = ifoo;
                break;
            }
        }

        if(ifoo<0) {
            // This vertex has different data. Add the vertex and point to it.
            B.AddVertex(Vrt->V, Vrt);
            Vrt->Tan.y = VertPtrs[ind]->Tan.y; // Point new one to second one.
            VertPtrs.push_back(Vrt);
            int newind = int((int)VertPtrs.size()) - 1;
            VertPtrs[ind]->Tan.y = newind; // Point first one to new one.
            // cerr << ind << " -> " << newind << "Splitting.\n";
            ind = newind;
        }
    }

    return ind;
}

// Touches up the TriObject to convert from the file's format to the official
// semantics of TriObject and adds it to the Model.
void s_OutputIndexedFaceSet()
{
    YYObject &S = Stack.back();
    if((int)S.VertexIndices.size() < 1) {
        // fprintf(stderr, "No vertex indices.\n");
        return;
    }

    // This is mostly for copying state.
    // The rest of the function re-copies the vertex values.
    Mesh *Bp = new Mesh;
    *(BaseObject *)Bp = *((BaseObject *)&S);
    Mesh &B = *Bp;
    B.ObjectType = DMC_MESH_OBJECT;
    B.VertexType = HAS_ATTRIBS;
    B.EdgeType = HAS_ATTRIBS;
    B.FaceType = HAS_ATTRIBS;

    int N = int((int)S.verts.size());

    if(S.MaterialBinding == PER_VERTEX) {
        ASSERT_R((int)S.dcolors.size() == N);
    }
    if(S.NormalBinding == PER_VERTEX) {
        ASSERT_R((int)S.normals.size() == N);
    }
    if((int)S.TexCoordIndices.size() == (int)S.VertexIndices.size())
        B.VertexType = B.VertexType | OBJ_TEXCOORDS;

    // Create the vertices and add them to the mesh.
    vector<AVertex *> VertPtrs; // Map vertex indices to vertex structs.
    VertPtrs.resize(N);
    int i;
    for(i=0; i<N; i++) {
        VertPtrs[i] = NULL;
    }

    PlaceToName = B.Name; // Hack.
    CopyNameThing = true; // Indicate to copy into the given string, instead of point from it.

    // Make sure the texture coord index list is the right size.
    if(S.TexPtr && (int)S.TexCoordIndices.size() > 0)
        ASSERT_RM((int)S.TexCoordIndices.size() == (int)S.VertexIndices.size(),
        "Wrong num. of texcoord indices.");

    if((int)S.alphas.size() > 0)
        ASSERT_RM(S.alphas.size() == S.dcolors.size(), "Inconsistent alpha list.");

    int FaceNum = 0, ci = 0, vertsThisPoly = 0;
    // i is the index into VertexIndices. ci only counts non- negative 1s.
    for(i=0; i<(int)S.VertexIndices.size(); i++) {
        if(S.VertexIndices[i] != -1) {
            if(vertsThisPoly >= 2) {
                // Handle arbitrary convex polygons by issuing one tri.

                int vi0 = S.VertexIndices[i-vertsThisPoly];
                int vi1 = S.VertexIndices[i-1];
                int vi2 = S.VertexIndices[i];
                if(vi0 == vi1 || vi1 == vi2 || vi2 == vi0) {
                    cerr << "Degenerate tri.\n";
                } else {
                    AFace *FF = new AFace;
                    vi0 = DoVertex(B, S, VertPtrs, i-vertsThisPoly, ci-vertsThisPoly, FaceNum, FF);
                    vi1 = DoVertex(B, S, VertPtrs, i-1, ci-1, FaceNum, FF);
                    vi2 = DoVertex(B, S, VertPtrs, i, ci, FaceNum, FF);

                    // Now add the necessary edges and faces.
                    Edge *E0 = B.FindEdge(VertPtrs[vi0], VertPtrs[vi1], AEdgeFactory);
                    Edge *E1 = B.FindEdge(VertPtrs[vi1], VertPtrs[vi2], AEdgeFactory);
                    Edge *E2 = B.FindEdge(VertPtrs[vi2], VertPtrs[vi0], AEdgeFactory);

#ifdef DMC_MODEL_DEBUG
                    // XXX An error check:
                    // Check whether the face already exists.
                    for(int q=0; q < (int)E0->Faces.size(); q++) {
                        AFace *F = (AFace *)E0->Faces[q];
                        WARN_R(F->e0 != FF->e0 || F->e1 != FF->e1 || F->e2 != FF->e2, "EDup face 0");
                        WARN_R(F->e0 != FF->e0 || F->e1 != FF->e2 || F->e2 != FF->e1, "EDup face 0");
                        WARN_R(F->e0 != FF->e1 || F->e1 != FF->e0 || F->e2 != FF->e2, "EDup face 1");
                        WARN_R(F->e0 != FF->e1 || F->e1 != FF->e2 || F->e2 != FF->e0, "EDup face 1");
                        WARN_R(F->e0 != FF->e2 || F->e1 != FF->e0 || F->e2 != FF->e1, "EDup face 2");
                        WARN_R(F->e0 != FF->e2 || F->e1 != FF->e1 || F->e2 != FF->e0, "EDup face 2");
                    }
#endif

                    B.AddFace(VertPtrs[vi0], VertPtrs[vi1], VertPtrs[vi2], E0, E1, E2, FF);
                }
            }

            ci++;
            vertsThisPoly++;
        } else {
            FaceNum++;
            vertsThisPoly = 0;
        }
    }

    B.RemoveUnusedVertices();

    // Gen vertex data from face data.
    if((ReqAttribs & OBJ_COLORS) && (S.MaterialBinding == PER_FACE || S.MaterialBinding == PER_FACE_INDEXED)) {
        B.GenColorsFromFaceColors();
    }
    if((ReqAttribs & OBJ_NORMALS) && (S.NormalBinding == PER_FACE || S.NormalBinding == PER_FACE_INDEXED)) {
        B.GenNormalsFromFaceNormals();
    }
    if(ReqAttribs & OBJ_TEXCOORDS) {
        B.GenTexCoords();
    }

#ifdef DMC_MODEL_DEBUG
    // B.CheckIntegrity();

    // XXX Debug code.
    if(B.VertexType & OBJ_NORMALS) {
        // cerr << "Checking.\n";
        // Sanity check the normals.
        for(AVertex *V = (AVertex *)B.Verts; V; V = (AVertex *)V->next) {
            if(!Finite(V->Nor.x))
                cerr << "LoadVRML: Failed normal check: " << V->Nor << endl;
            f3Vector::ElType len2 = V->Nor.length2();
            if(len2 < 0.97 || len2 > 1.03)
                cerr << "LoadVRML: Failed normal check: " << V->Nor << endl;
        }
    }

    // XXX Debug code.
    if(B.VertexType & OBJ_TEXCOORDS) {
        // cerr << "Checking.\n";
        // Sanity check the normals.
        for(AVertex *V = (AVertex *)B.Verts; V; V = (AVertex *)V->next) {
            if(!Finite(V->Tex.x) || !Finite(V->Tex.y))
                cerr << "LoadVRML: BadTex: " << V->Tex << endl;
            if(Abs(V->Tex.x) > 1e5 || Abs(V->Tex.y) > 1e5)
                cerr << "LoadVRML: BadTex: " << V->Tex << endl;
        }
    }
#endif

    // The final result of this function is to add this object to the
    // list of objects that will be returned to the user.
    thisModel->Objs.push_back(Bp);

    // Update bounding box of the whole model.
    thisModel->Box += B.Box;
}

// Returns false on success.
bool Model::LoadVRML(const char *fname, const unsigned int RequiredAttribs,
        const unsigned int AcceptedAttribs)
{
    ReqAttribs = RequiredAttribs;
    AccAttribs = AcceptedAttribs;

    InFile = fopen(fname, "r");
    ASSERT_RM(InFile, "Error opening input file");

    // Read first line to verify that it's VRML 1.0. Best to do this
    // here, since it's hard to make lex'er distinguish between this
    // first line and a comment.
    char tmpbuf[256];
    char firstline[] = "#VRML V1.0 ascii";
    if(!fgets(tmpbuf, 256, InFile) || strncmp(firstline, tmpbuf, strlen(firstline))) {
        fprintf(stderr, "Input file is not VRML 1.0\n");
        fclose(InFile);
        return true;
    }

    // ObjectSet is a global pointer into our list of objects which
    // allows the global callback routines to store objects in our model.
    Objs.clear();
    thisModel = this;
    Stack.clear();
    YYObject First;
    Stack.push_back(First);

    // fprintf(stderr, "Starting parse.\n");
    int parseret = yyparse();
    fclose(InFile);

    if(parseret) {
        fprintf(stderr, "Parsing terminated with %i errors.\n", yynerrs);
        return true;
    } else {
        // fprintf(stderr, "Parsing completed with no errors.\n");
    }

    // Check that state stack is balanced -- should always be true.
    ASSERT_RM(Stack.size()==1, "LoadVRML:: Internal error -- Stack depth bad.");

    return false;
}

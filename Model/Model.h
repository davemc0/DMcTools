//////////////////////////////////////////////////////////////////////
// Model.h - A list of TriObject, Mesh, RenderObject, etc.
//
// Copyright David K. McAllister, July 1999.

#pragma once

#include "Model/TextureDB.h"
#include "Model/BaseObject.h"

#include <vector>

class Model
{
public:
    static TextureDB TexDB; // There is one global texture database. Sorry.
    BBox<f3Vector> Box;

    // Pointers actually to subclasses of BaseObject.
    std::vector<BaseObject *> Objs;
    int ObjID;

    DMC_DECL Model() {ObjID = -1;}

    // Indicate what PER VERTEX attribs to require and what to allow.
    DMC_DECL Model(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL)
    {
        // Make sure they accept what they require.
        ASSERT_R(AcceptedAttribs == (AcceptedAttribs | RequiredAttribs));

        ObjID = -1;
        Load(fname, RequiredAttribs, AcceptedAttribs);
    }

    DMC_DECL ~Model()
    {
        // Delete the objects pointed to by the array.
        for(size_t i=0; i<Objs.size(); i++) {
            // INFO("~Model deleting " << i);
            delete Objs[i];
        }
    }

    // User creates the object externally, but Model deletes it.
    void InsertObject(BaseObject *NewObj)
    {
        Box += NewObj->Box;
        Objs.push_back(NewObj);
    }

    // Generate and remove attribs as needed.
    void ModifyAttribs(const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);

    // Print info to stderr.
    void Dump() const;

    // Flatten this model into one object.
    void Flatten();

    // Flip facing of all faces to match the first face.
    void FixFacing();

    // Converts DMC_MESH_OBJECTs in this Model into DestType objects,
    // which is DMC_RENDER_OBJECT or DMC_TRI_OBJECT.
    void ObjectConvert(ObjectTypes DestType, unsigned int AcceptedAttribs = OBJ_ALL);

    // Generate a bounding box of all subobjects.
    void RebuildBBox();

    // Transform all subobjects by this matrix.
    // Also rebuilds the BBox.
    void ApplyTransform(Matrix44<typename f3Vector::ElType> &Mat);

    // Generates the given attribute based on the geometry of the mesh.
    // Sets the OBJ_WHATEVER flag.
    void GenColors();
    void GenNormals();
    void GenTexCoords();
    void GenTangents();

    // Clears the OBJ_WHATEVER flag.
    void RemoveColors();
    void RemoveNormals();
    void RemoveTexCoords();
    void RemoveTangents();

    // For the loaders, RequiredAttribs is just a hint. It may not be satisfied.
    // Call ModifyAttribs to force it (done by Load()).

    // Return false on success.
    bool Load(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);
    bool Save(const char *fname);

    bool LoadVRML(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);
    bool SaveVRML(const char *fname);

    bool LoadOBJ(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);
    bool SaveOBJ(const char *fname);

    bool LoadTRI(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);
    bool SaveTRI(const char *fname);

    bool LoadPLY(const char *fname, const unsigned int RequiredAttribs = OBJ_ALL,
        const unsigned int AcceptedAttribs = OBJ_ALL);
    bool SavePLY(const char *fname);

private:

  bool SaveTriObjectAsOBJ(const char * fname);
  bool SaveRenderObjectAsOBJ(const char * fname);

};

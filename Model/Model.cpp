//////////////////////////////////////////////////////////////////////
// Model.cpp - A list of TriObject, Mesh, RenderObject, etc.
//
// Copyright David K. McAllister, July 1999.

#include "Model/Model.h"

#include "Model/CameraDB.h"
#include "Model/LightDB.h"
#include "Model/Mesh.h"
#include "Model/RenderObject.h"

// Define this static dude.
TextureDB Model::TexDB;
LightDB LitDB;
CameraDB CamDB;

// Returns false on success; true on error.
bool Model::Save(const char* fname)
{
    ASSERT_RM(fname, "NULL filename");
    bool status = true;

    std::cerr << "Model has " << int(Objs.size()) << " objects.\n";

    const char* extc = strrchr(fname, '.');
    extc++;

    if (strlen(extc) != 3) {
        std::cerr << "Can't grok filename " << fname << std::endl;
        return status;
    }

    char* extp = strdup(extc);

    extp[0] |= 0x20;
    extp[1] |= 0x20;
    extp[2] |= 0x20;

    if (!strcmp(extp, "obj"))
        status = SaveOBJ(fname);
    else if (!strcmp(extp, "tri"))
        status = true; // SaveTRI(fname);
    else if (!strcmp(extp, "ply"))
        status = true; // SavePLY(fname);
    else {
        std::cerr << "Can't grok filename " << fname << std::endl;
        status = true;
    }

    return status;
}

// Generate and remove attribs as needed
void Model::ModifyAttribs(const unsigned int RequiredAttribs, const unsigned int AcceptedAttribs)
{
    if (RequiredAttribs & OBJ_COLORS) GenColors();
    if (RequiredAttribs & OBJ_NORMALS) GenNormals();
    if (RequiredAttribs & OBJ_TEXCOORDS) GenTexCoords();
    if (RequiredAttribs & OBJ_TANGENTS) GenTangents();

    // Remove undesired attribs.
    if (!(AcceptedAttribs & OBJ_COLORS)) RemoveColors();
    if (!(AcceptedAttribs & OBJ_NORMALS)) RemoveNormals();
    if (!(AcceptedAttribs & OBJ_TEXCOORDS)) RemoveTexCoords();
    if (!(AcceptedAttribs & OBJ_TANGENTS)) RemoveTangents();
}

// Returns false on success; true on error.
bool Model::Load(const char* fname, const unsigned int RequiredAttribs, const unsigned int AcceptedAttribs)
{
    ASSERT_RM(fname, "NULL filename");
    bool status = true;

    const char* extc = strrchr(fname, '.');
    extc++;

    if (strlen(extc) != 3) {
        std::cerr << "Can't grok filename " << fname << std::endl;
        return status;
    }

    char* extp = strdup(extc);

    extp[0] |= 0x20;
    extp[1] |= 0x20;
    extp[2] |= 0x20;

    if (!strcmp(extp, "obj"))
        status = LoadOBJ(fname, RequiredAttribs, AcceptedAttribs);
    else if (!strcmp(extp, "tri"))
        status = true; // LoadTRI(fname, RequiredAttribs, AcceptedAttribs);
    else if (!strcmp(extp, "ply"))
        status = true; // LoadPLY(fname, RequiredAttribs, AcceptedAttribs);
    else {
        std::cerr << "Can't grok filename " << fname << std::endl;
        status = true;
    }

    if (!status) ModifyAttribs(RequiredAttribs, AcceptedAttribs);

    return status;
}

// Converts DMC_MESH_OBJECTs in this Model into DestType objects,
// which is DMC_RENDER_OBJECT or DMC_TRI_OBJECT.
void Model::ObjectConvert(ObjectTypes DestType, unsigned int AcceptedAttribs)
{
    for (int i = 0; i < (int)Objs.size(); i++) {
        // std::cerr << i << std::endl;
        if (Objs[i]->ObjectType == DMC_MESH_OBJECT) {
            BaseObject* Ob;
            switch (DestType) {
            case DMC_RENDER_OBJECT:
                Ob = new RenderObject;
                ((Mesh*)Objs[i])->ExportRenderObject(*((RenderObject*)Ob), AcceptedAttribs);
                delete (Mesh*)Objs[i];
                Objs[i] = Ob;
                break;
            case DMC_TRI_OBJECT:
                Ob = new TriObject;
                ((Mesh*)Objs[i])->ExportTriObject(*((TriObject*)Ob), AcceptedAttribs);
                delete (Mesh*)Objs[i];
                Objs[i] = Ob;
                break;
            default: ASSERT_R(0);
            }
        }
    }
}

// XXX Need to generalize this to handle Mesh, at least.
// If the model consists of multiple TriObjects, collapse them all into a single TriObject.
void Model::Flatten()
{
    if ((int)Objs.size() < 2) return;

    TriObject* NOb = dynamic_cast<TriObject*>(Objs[0]);
    ASSERT_R(NOb);
    ASSERT_R(NOb->PrimType == L_TRIANGLES);

    for (size_t i = 1; i < Objs.size(); i++) {
        const TriObject* TOb = dynamic_cast<TriObject*>(Objs[i]);
        ASSERT_RM(TOb, "All objects in the Model must be TriObjects. Mesh is not allowed, for example.\n");

        if ((int)TOb->verts.size() < 1) continue;

        ASSERT_R(TOb->PrimType == L_TRIANGLES);

        if ((int)TOb->dcolors.size() > 1) ASSERT_RM(TOb->dcolors.size() == TOb->verts.size(), "TriObject Bad dcolors.size()");

        if ((int)TOb->normals.size() > 1) ASSERT_RM(TOb->normals.size() == TOb->verts.size(), "TriObject Bad normals.size()");

        if ((int)TOb->texcoords.size() > 1) ASSERT_RM(TOb->texcoords.size() == TOb->verts.size(), "TriObject Bad texcoords.size()");

        if ((int)NOb->dcolors.size() > 1) ASSERT_RM(NOb->dcolors.size() == NOb->verts.size(), "List Bad dcolors.size()");

        if ((int)NOb->normals.size() > 1) ASSERT_RM(NOb->normals.size() == NOb->verts.size(), "List Bad normals.size()");

        if ((int)NOb->texcoords.size() > 1) ASSERT_RM(NOb->texcoords.size() == NOb->verts.size(), "List Bad texcoords.size()");

        if ((int)NOb->dcolors.size() == 1 && ((TOb->dcolors.size() == 1 && TOb->dcolors[0] != NOb->dcolors[0]) || TOb->dcolors.size() > 1)) {
            // NOb was doing per-object color, but TOb doesn't match it, so convert NOb to per-vertex color.
            f3vec col = NOb->dcolors[0];
            NOb->dcolors.clear();
            NOb->dcolors.insert(NOb->dcolors.begin(), NOb->verts.size(), col);
        }

        if ((int)NOb->dcolors.size() != 1) {
            if ((int)TOb->dcolors.size() == 1) {
                // Expand its colors.
                f3vec col = TOb->dcolors[0];
                NOb->dcolors.insert(NOb->dcolors.end(), TOb->verts.size(), col);
            } else if ((int)TOb->dcolors.size() == 0) {
                if ((int)NOb->dcolors.size()) {
                    // Have to synthesize a bunch of them.
                    f3vec col(0, 1, 0);
                    NOb->dcolors.insert(NOb->dcolors.end(), TOb->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->dcolors.insert(NOb->dcolors.end(), TOb->dcolors.begin(), TOb->dcolors.end());
            }
        }

        if ((int)NOb->normals.size() == 1 && ((TOb->normals.size() == 1 && TOb->normals[0] != NOb->normals[0]) || TOb->normals.size() > 1)) {
            // Expand my normals.
            f3vec col = NOb->normals[0];
            NOb->normals.clear();
            NOb->normals.insert(NOb->normals.begin(), NOb->verts.size(), col);
        }

        if ((int)NOb->normals.size() != 1) {
            if ((int)TOb->normals.size() == 1) {
                // Expand its normals.
                f3vec col = TOb->normals[0];
                NOb->normals.insert(NOb->normals.end(), TOb->verts.size(), col);
            } else if ((int)TOb->normals.size() == 0) {
                if ((int)NOb->normals.size()) {
                    // Have to synthesize a bunch of them.
                    f3vec col(0, 1, 0);
                    NOb->normals.insert(NOb->normals.end(), TOb->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->normals.insert(NOb->normals.end(), TOb->normals.begin(), TOb->normals.end());
            }
        }

        if ((int)NOb->texcoords.size() == 1 && ((TOb->texcoords.size() == 1 && TOb->texcoords[0] != NOb->texcoords[0]) || TOb->texcoords.size() > 1)) {
            // Expand my texcoords.
            f3vec col = NOb->texcoords[0];
            NOb->texcoords.clear();
            NOb->texcoords.insert(NOb->texcoords.begin(), NOb->verts.size(), col);
        }

        if ((int)NOb->texcoords.size() != 1) {
            if ((int)TOb->texcoords.size() == 1) {
                // Expand its texcoords.
                f3vec col = TOb->texcoords[0];
                NOb->texcoords.insert(NOb->texcoords.end(), TOb->verts.size(), col);
            } else if (TOb->texcoords.size() == 0) {
                if (NOb->texcoords.size()) {
                    // Have to synthesize a bunch of them.
                    f3vec col(0, 1, 0);
                    NOb->texcoords.insert(NOb->texcoords.end(), TOb->verts.size(), col);
                }
            } else {
                // Copy its list to my list.
                NOb->texcoords.insert(NOb->texcoords.end(), TOb->texcoords.begin(), TOb->texcoords.end());
            }
        }

        NOb->verts.insert(NOb->verts.end(), TOb->verts.begin(), TOb->verts.end());
    }

    if (Objs.size() > 1) {
        for (size_t i = 1; i < Objs.size(); i++) delete Objs[i];
        Objs.erase(Objs.begin() + 1, Objs.end());
    }

    ASSERT_RM(NOb->verts.size() % 3 == 0, "Must have a multiple of three vertices in TriObject.");

    if (NOb->dcolors.size() > 1) ASSERT_RM(NOb->dcolors.size() == NOb->verts.size(), "Bad dcolors.size()");

    if (NOb->normals.size() > 1) ASSERT_RM(NOb->normals.size() == NOb->verts.size(), "Bad normals.size()");

    if (NOb->texcoords.size() > 1) ASSERT_RM(NOb->texcoords.size() == NOb->verts.size(), "Bad texcoords.size()");
}

void Model::Dump() const
{
    std::cerr << "Dumping Model ObjId: " << ObjID << "\nModel BBox: " << Box << std::endl << "NumObjects: " << int(Objs.size()) << std::endl << std::endl;
    for (int i = 0; i < (int)Objs.size(); i++) {
        std::cerr << "Object index: " << i << std::endl;
        Objs[i]->Dump();
    }
}

void Model::RebuildBBox()
{
    Box.reset();

    for (int i = 0; i < (int)Objs.size(); i++) {
        Objs[i]->RebuildBBox();
        Box.grow(Objs[i]->Box);
    }
}

// Also rebuilds the BBox.
void Model::ApplyTransform(Matrix44<f3vec>& Mat)
{
    Box.reset();

    for (int i = 0; i < (int)Objs.size(); i++) {
        Objs[i]->ApplyTransform(Mat);
        Box.grow(Objs[i]->Box);
    }
}

void Model::GenColors()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->GenColors();
}

void Model::GenNormals()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->GenNormals();
}

void Model::GenTexCoords()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->GenTexCoords();
}

void Model::GenTangents()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->GenTangents();
}

void Model::RemoveColors()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->RemoveColors();
}

void Model::RemoveNormals()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->RemoveNormals();
}

void Model::RemoveTexCoords()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->RemoveTexCoords();
}

void Model::RemoveTangents()
{
    for (int i = 0; i < (int)Objs.size(); i++) Objs[i]->RemoveTangents();
}

void Model::FixFacing()
{
    for (int i = 0; i < (int)Objs.size(); i++) {
        if (Objs[i]->ObjectType == DMC_MESH_OBJECT) ((Mesh*)Objs[i])->FixFacing();
    }
}

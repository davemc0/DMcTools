//////////////////////////////////////////////////////////////////////
// SaveOBJ.cpp - Write a Model as an OBJ file
//
// Copyright David K. McAllister, Aug. 1999.

#include "Model/Mesh.h"
#include "Model/Model.h"

#include <cstdio>

// Write an OBJ file for the model
// The Model must consist only of TriObject objects, not Meshes.
// If the Model contains more than one object it will create a flattened copy and save that.
// However, this doesn't work with Meshes.
// Would be better to save each object as an OBJ group.
bool Model::SaveOBJ(const char* fname)
{
    if (dynamic_cast<TriObject*>(Objs[0])) {
        if (Objs.size() > 1) {
            // XXX What should I do about materials?
            std::cerr << "Flattening a copy of your model and saving it as .OBJ\n";
            Model copyModel(*this);

            copyModel.Flatten();

            return copyModel.SaveOBJ(fname);
        }

        return SaveTriObjectAsOBJ(fname);
    } else if (dynamic_cast<RenderObject*>(Objs[0])) {
        // Can now handle multiple RenderObjects in one OBJ file.
        return SaveRenderObjectAsOBJ(fname);
    }

    ASSERT_RM(0, "Can't save a Model whose object is not a TriObject or RenderObject.");

    return true;
}

bool Model::SaveTriObjectAsOBJ(const char* fname)
{
    FILE* out = fopen(fname, "w");
    ASSERT_RM(out, "Couldn't open file to save OBJ file.");

    fprintf(out, "#Wavefront .OBJ format\n# Saved by DaveMc's SaveTriObjectAsOBJ.\n\ng\n");

    TriObject* Ob = dynamic_cast<TriObject*>(Objs[0]);
    ASSERT_RM(Ob, "Can't save a Model whose object is not a TriObject");

    bool DoNormals = Ob->normals.size() == Ob->verts.size();
    bool DoTexcoords = Ob->texcoords.size() == Ob->verts.size();

    for (size_t i = 0; i < (int)Ob->verts.size(); i++) fprintf(out, "v %f %f %f\n", Ob->verts[i].x, Ob->verts[i].y, Ob->verts[i].z);
    fprintf(out, "# %lld vertices.\n\n", Ob->verts.size());

    if (DoTexcoords) {
        for (size_t i = 0; i < (int)Ob->texcoords.size(); i++) fprintf(out, "vt %f %f\n", Ob->texcoords[i].x, Ob->texcoords[i].y);
    }
    fprintf(out, "# %lld texcoords.\n\n", Ob->texcoords.size());

    if (DoNormals) {
        for (size_t i = 0; i < (int)Ob->normals.size(); i++) fprintf(out, "vn %f %f %f\n", Ob->normals[i].x, Ob->normals[i].y, Ob->normals[i].z);
    }
    fprintf(out, "# %lld normals.\n\n", Ob->normals.size());

    for (size_t i = 0; i < (int)Ob->verts.size(); i += 3) {
        fprintf(out, "f ");
        for (int j = 0; j < 3; j++) {
            fprintf(out, "%lld", i + j + 1);
            if (DoTexcoords)
                fprintf(out, "/%lld", i + j + 1);
            else if (DoNormals)
                fprintf(out, "/");
            if (DoNormals) fprintf(out, "/%lld", i + j + 1);
            fprintf(out, " ");
        }
        fprintf(out, "\n");
    }

    fclose(out);

    return false;
}

bool Model::SaveRenderObjectAsOBJ(const char* fname)
{
    size_t vertofs = 0, texcoordofs = 0, normalofs = 0;

    FILE* out = fopen(fname, "w");
    ASSERT_RM(out, "Couldn't open file to save OBJ file.");

    fprintf(out, "#Wavefront .OBJ format\n# Saved by DaveMc's SaveRenderObjectAsOBJ.\n\n");

    for (size_t g = 0; g < Objs.size(); g++) {
        fprintf(out, "\ng obj%04d\n", (int)g);
        // printf("Saving group obj%d\n", (int)g);

        RenderObject* Ob = dynamic_cast<RenderObject*>(Objs[g]);
        ASSERT_RM(Ob, "Can't save a Model whose objects are not RenderObjects.");

        bool DoVertexNormals = Ob->normals.size() == Ob->verts.size();
        bool DoTexcoords = Ob->texcoords.size() == Ob->verts.size();
        bool DoFaceNormals = Ob->fnindices.size() > 0;

        if (DoFaceNormals) {
            ASSERT_RM(Ob->FaceCount() == Ob->fnindices.size(), "fnindinces must be 0 or FaceCount.");
            ASSERT_RM(Ob->normals.size() > 0, "Gotta define some normals because you have face normals.");
            DoVertexNormals = false;
        }

        for (size_t i = 0; i < (int)Ob->verts.size(); i++) fprintf(out, "v %f %f %f\n", Ob->verts[i].x, Ob->verts[i].y, Ob->verts[i].z);
        fprintf(out, "# %lld vertices.\n\n", Ob->verts.size());

        if (DoTexcoords) {
            for (size_t i = 0; i < (int)Ob->texcoords.size(); i++) fprintf(out, "vt %f %f\n", Ob->texcoords[i].x, Ob->texcoords[i].y);
        }
        fprintf(out, "# %lld texcoords.\n\n", Ob->texcoords.size());

        if (DoVertexNormals || DoFaceNormals) {
            for (size_t i = 0; i < (int)Ob->normals.size(); i++) fprintf(out, "vn %f %f %f\n", Ob->normals[i].x, Ob->normals[i].y, Ob->normals[i].z);
        }
        fprintf(out, "# %lld normals.\n\n", Ob->normals.size());

        for (size_t i = 0; i < (int)Ob->indices.size(); i += 3) {
            fprintf(out, "f ");
            for (int j = 0; j < 3; j++) {
                int vi = Ob->indices[i + j];
                fprintf(out, "%lld", vertofs + vi + 1);
                if (DoTexcoords)
                    fprintf(out, "/%lld", texcoordofs + vi + 1);
                else if (DoVertexNormals || DoFaceNormals)
                    fprintf(out, "/");
                if (DoVertexNormals) fprintf(out, "/%lld", normalofs + vi + 1);
                if (DoFaceNormals) fprintf(out, "/%lld", normalofs + Ob->fnindices[i / 3] + 1);
                fprintf(out, " ");
            }
            fprintf(out, "\n");
        }

        vertofs += Ob->verts.size();
        texcoordofs += Ob->texcoords.size();
        normalofs += Ob->normals.size();
    }

    fclose(out);

    return false;
}

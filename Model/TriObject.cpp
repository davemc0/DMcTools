//////////////////////////////////////////////////////////////////////
// TriObject.cpp - Represent a single general triangle begin/end pair with rendering state
//
// Copyright David K. McAllister, July 1999.

#include "Model/Mesh.h"
#include "Model/Model.h"

// Generate normals for each vertex.
// The smaller the crease angle, the more smoothing.
// If normals already exist, does not regenerate.
void TriObject::GenNormals()
{
    ASSERT_RM(PrimType == L_TRIANGLES, "Bad PrimType.");
    ASSERT_RM(creaseAngle >= 0 && creaseAngle <= M_PI, "Bad creaseAngle.");

    if (normals.size() == verts.size())
        // Already have normals.
        return;

    std::cerr << "dcolors.size = " << int(dcolors.size()) << std::endl;

    // Build a mesh. XXX !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Mesh Me(*this);
    Me.FixFacing();

    normals.clear();
    verts.clear();

    // This ensures that duplicate faces don't screw us up.
    TriObject TmpOb;
    Me.ExportTriObject(TmpOb);
    verts = TmpOb.verts;
    if (texcoords.size() != 1) texcoords = TmpOb.texcoords;
    if (dcolors.size() != 1) dcolors = TmpOb.dcolors;

    // std::cerr << creaseAngle << " dcolors.size = " << dcolors.size() << std::endl;

    // First compute the facet normals.
    Face* F = Me.Faces;
    int i;
    for (i = 0; i < (int)verts.size(); i += 3, F = F->next) {
        // I is currently the first vertex of the face.
        f3vec P0 = verts[i] - verts[i + 1];
        f3vec P1 = verts[i + 2] - verts[i + 1];

        f3vec* N = new f3vec(cross(P1, P0));
        N->normalize();

        // Store a pointer to this facet normal in F->e0.
        F->e0 = (Edge*)N;
    }

    // For all vertices that match this one, accumulate those with
    // an angle less than creaseAngle into a smooth normal.
    f3vec::ElType CosCrease = cos(creaseAngle);

    for (i = 0, F = Me.Faces; i < (int)verts.size(); i += 3, F = F->next) {
        ASSERT_D(F);

        f3vec& FN = *((f3vec*)F->e0);
        for (int j = 0; j < 3; j++) {
            Vertex* V;
            if (F->v0->V == verts[i + j])
                V = F->v0;
            else if (F->v1->V == verts[i + j])
                V = F->v1;
            else {
                ASSERT_D(F->v2->V == verts[i + j]);
                V = F->v2;
            }

            int cnt = 1;
            f3vec Accum(FN);

            // Loop on all the faces of this vertex.
            for (int k = 0; k < (int)V->Faces.size(); k++) {
                if (V->Faces[k] != F) {
                    // Compute cos of their dihedral angle.
                    // Dot goes from 1 to -1 as ang goes from 0 to PI.
                    f3vec& FNT = *((f3vec*)(V->Faces[k]->e0));
                    f3vec::ElType AngDot = dot(FN, FNT);

                    // If the angle < creaseAngle then Ang > CosCrease and we smooth.
                    if (AngDot > CosCrease) {
                        // std::cerr << creaseAngle << " " << CosCrease << " " << AngDot << std::endl;
                        // std::cerr << FN << FNT << std::endl;
                        // Smooth with this face.
                        Accum += FNT;
                        cnt++;
                    }
                }
            }

            // Now store the normal.
            Accum /= cnt;

            Accum.normalize();
            normals.push_back(Accum);
        }
    }

    // Remove the facet normals I made.
    for (F = Me.Faces; F; F = F->next) delete (f3vec*)F->e0;
}

// Generate tangents for each vertex.
// Texture coordinates must exist.
void TriObject::GenTangents() {}

// Convert the object from all quads to all tris.
// KeepBad keeps zero area triangles.
void TriObject::QuadsToTris(bool KeepBad)
{
    if (PrimType != L_QUADS) return;

    PrimType = L_TRIANGLES;

    ASSERT_RM(verts.size() % 4 == 0, "Must have a multiple of 4 vertices.");

    std::cerr << "Converting from " << (int(verts.size()) / 4) << " quads.\n";

    std::vector<f3vec> verts_, normals_, texcoords_, dcolors_;

    bool DoNormals = false, DoTexcoords = false, DoDColors = false;

    if (normals.size() == verts.size()) DoNormals = true;
    if (texcoords.size() == verts.size()) DoTexcoords = true;
    if (dcolors.size() == verts.size()) DoDColors = true;

    for (int i = 0; i < (int)verts.size(); i += 4) {
        bool DoFirst = KeepBad || !(verts[i] == verts[i + 1] || verts[i] == verts[i + 2] || verts[i + 2] == verts[i + 1]);
        bool DoSecond = KeepBad || !(verts[i] == verts[i + 3] || verts[i] == verts[i + 2] || verts[i + 2] == verts[i + 3]);

        if (DoFirst) {
            verts_.push_back(verts[i]);
            verts_.push_back(verts[i + 1]);
            verts_.push_back(verts[i + 2]);
        }
        if (DoSecond) {
            verts_.push_back(verts[i]);
            verts_.push_back(verts[i + 2]);
            verts_.push_back(verts[i + 3]);
        }

        if (DoNormals) {
            if (DoFirst) {
                normals_.push_back(normals[i]);
                normals_.push_back(normals[i + 1]);
                normals_.push_back(normals[i + 2]);
            }
            if (DoSecond) {
                normals_.push_back(normals[i]);
                normals_.push_back(normals[i + 2]);
                normals_.push_back(normals[i + 3]);
            }
        }

        if (DoTexcoords) {
            if (DoFirst) {
                texcoords_.push_back(texcoords[i]);
                texcoords_.push_back(texcoords[i + 1]);
                texcoords_.push_back(texcoords[i + 2]);
            }
            if (DoSecond) {
                texcoords_.push_back(texcoords[i]);
                texcoords_.push_back(texcoords[i + 2]);
                texcoords_.push_back(texcoords[i + 3]);
            }
        }

        if (DoDColors) {
            if (DoFirst) {
                dcolors_.push_back(dcolors[i]);
                dcolors_.push_back(dcolors[i + 1]);
                dcolors_.push_back(dcolors[i + 2]);
            }
            if (DoSecond) {
                dcolors_.push_back(dcolors[i]);
                dcolors_.push_back(dcolors[i + 2]);
                dcolors_.push_back(dcolors[i + 3]);
            }
        }
    }

    verts = verts_;
    if (DoNormals) normals = normals_;
    if (DoTexcoords) texcoords = texcoords_;
    if (DoDColors) dcolors = dcolors_;

    if (DoNormals) ASSERT_RM(normals.size() == verts.size(), "Bad normal count.");
    if (DoTexcoords) ASSERT_RM(texcoords.size() == verts.size(), "Bad texcoords count.");
    if (DoDColors) ASSERT_RM(dcolors.size() == verts.size(), "Bad dcolors count.");

    std::cerr << DoNormals << DoTexcoords << DoDColors << "Converted to " << (int(verts.size()) / 3) << " triangles.\n";
}

void TriObject::Dump() const
{
    std::cerr << "Name: " << Name << " ObjID: " << ObjID << std::endl;
    std::cerr << "Vertex count: " << int(verts.size()) << std::endl;

    std::cerr << "Specular color: " << scolor << std::endl
              << "Emissive color: " << ecolor << std::endl
              << "Ambient color: " << acolor << std::endl
              << "Shininess: " << shininess << " PrimType: " << PrimType << std::endl
              << "TriObject BBox: " << Box << "\n\nVertex \t\tNormal\t\tTexcoord\t\tDColor\n";

    for (int i = 0; i < (int)verts.size(); i++) {
        std::cerr << verts[i] << "\t";
        if (i < (int)normals.size())
            std::cerr << normals[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if (i < (int)texcoords.size())
            std::cerr << texcoords[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if (i < (int)dcolors.size())
            std::cerr << dcolors[i] << "\t";
        else
            std::cerr << "xxxxxxxxxxxxxxxxxxx\t";
        if (i < (int)alphas.size()) std::cerr << alphas[i];
        std::cerr << std::endl;
    }
    std::cerr << std::endl << std::endl;
}

void TriObject::RebuildBBox()
{
    Box.reset();

    for (int i = 0; i < (int)verts.size(); i++) Box.grow(verts[i]);
}

// Transform every vertex in the model.
// Transform every normal and tangent by using ProjectDirection, which is not
// the inverse transpose, but merely the upper 3x3.
void TriObject::ApplyTransform(Matrix44<f3vec>& Mat)
{
    Box.reset();

    int i;
    for (i = 0; i < (int)verts.size(); i++) {
        verts[i] = Mat * verts[i];
        Box.grow(verts[i]);
    }

    for (i = 0; i < (int)normals.size(); i++) { normals[i] = Mat.ProjectDirection(normals[i]); }

    // XXX Would it be better to recompute these?
    for (i = 0; i < (int)tangents.size(); i++) { tangents[i] = Mat.ProjectDirection(tangents[i]); }
}

void TriObject::ApplyTextureTransform(Matrix44<f3vec>&) { ASSERT_R(0); }

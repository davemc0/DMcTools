//////////////////////////////////////////////////////////////////////
// SaveVRML.h - Write a Model as a VRML 1.0 file
//
// Copyright David K. McAllister, July 1999.

#pragma once

#include "Model/Model.h"
#include "Model/Mesh.h"

#include <cstdio>

// Cast a TriObject to this to save it out as VRML.
// This is done by Model::SaveVRML.
class WritableVRMLTriObject : public TriObject
{
    FILE *out;
    int Ind;
    bool DoIndent;

    DMC_DECL void IncIndent()
    {
        Ind += 2;
    }

    DMC_DECL void DecIndent()
    {
        Ind -= 2;
    }

    void indent();
    void writeVector(const f3Vector &);
    void writeMaterials();
    void writeNormals();
    void writeTexCoords();
    void writeVertices();
    void writeIndices();
public:
    DMC_DECL WritableVRMLTriObject()
    {
        Ind = 0;
        out = NULL;
        DoIndent = false;
    }

    DMC_DECL WritableVRMLTriObject(TriObject o) : TriObject(o)
    {
        Ind = 0;
        out = NULL;
        DoIndent = false;
    }

    void Write(FILE *, int indlevel);
};

// Cast a Mesh object to WritableVRMLMesh to save it out as VRML.
class WritableVRMLMesh : public Mesh
{
    FILE *out; // For saving.
    int Ind;
    bool DoIndent;

    DMC_DECL void IncIndent()
    {
        Ind += 2;
    }

    DMC_DECL void DecIndent()
    {
        Ind -= 2;
    }

    void indent();
    void writeVector(const f3Vector &);
    void writeMaterials();
    void writeNormals();
    void writeTexCoords();
    void writeVertices();
    void writeIndices();

public:
    DMC_DECL WritableVRMLMesh()
    {
        Ind = 0;
        out = NULL;
        DoIndent = false;
    }

    // XXX Want to make sure it doesn't copy all the data.
    DMC_DECL WritableVRMLMesh(Mesh &Mo, bool DoIndent = false)
      : Mesh(Mo), DoIndent(DoIndent)
    {
        Ind = 0;
        out = NULL;
    }

    void Write(FILE *, int indlevel);
};

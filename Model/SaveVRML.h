//////////////////////////////////////////////////////////////////////
// SaveVRML.h - Write a Model as a VRML 1.0 file.
//
// Copyright David K. McAllister, July 1999.

#ifndef _savevrml_h
#define _savevrml_h

#include <Model/Model.h>
#include <Model/Mesh.h>

#include <stdio.h>

// Cast a TriObject to this to save it out as VRML.
// This is done by Model::SaveVRML.
class WritableVRMLTriObject : public TriObject
{
	FILE *out;
	int Ind;
	bool DoIndent;
	
	inline void IncIndent()
	{
		Ind += 2;
	}
	
	inline void DecIndent()
	{
		Ind -= 2;
	}
	
	void indent();
	void writeVector(const Vector &);
	void writeMaterials();
	void writeNormals();
	void writeTexCoords();
	void writeVertices();
	void writeIndices();
public:
	inline WritableVRMLTriObject()
	{
		Ind = 0;
		out = NULL;
		DoIndent = false;
	}
	
	inline WritableVRMLTriObject(TriObject o) : TriObject(o)
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
	
	inline void IncIndent()
	{
		Ind += 2;
	}
	
	inline void DecIndent()
	{
		Ind -= 2;
	}
	
	void indent();
	void writeVector(const Vector &);
	void writeMaterials();
	void writeNormals();
	void writeTexCoords();
	void writeVertices();
	void writeIndices();

public:
	inline WritableVRMLMesh()
	{
		Ind = 0;
		out = NULL;
		DoIndent = false;
	}
	
    // XXX Want to make sure it doesn't copy all the data.
	inline WritableVRMLMesh(Mesh &Mo, bool _DoIndent = false)
	  : Mesh(Mo), DoIndent(_DoIndent)
	{
		Ind = 0;
		out = NULL;
	}
	
	void Write(FILE *, int indlevel);
};

#endif

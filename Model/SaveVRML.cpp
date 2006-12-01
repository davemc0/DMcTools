//////////////////////////////////////////////////////////////////////
// SaveVRML.cpp - Write a Model as a VRML 1.0 file.
//
// Copyright David K. McAllister, July 1999.

#include <Model/Model.h>
#include <Model/SaveVRML.h>
#include <Model/AElements.h>

#include <stdio.h>
#include <string.h>

#include <map>

using namespace std;

// Assume an eight-space tab.
void WritableVRMLTriObject::indent()
{
	if(DoIndent)
    {
		int i;
		for(i=0; i<Ind-8; i+=8) fprintf(out, "\t");
		for(; i<Ind; i++) fprintf(out, " ");
    }
}

void WritableVRMLTriObject::writeVector(const Vector &V)
{
    // XXX Fix this precision. Also make it specifiable precision.
	fprintf(out, "%0.5f %0.5f %0.5f", V.x, V.y, V.z);
}

// All the lists have to be either length zero or the same length.
void WritableVRMLTriObject::writeMaterials()
{
	if(dcolors.size() < 1)
		return;
	
	indent(); fprintf(out, "Material\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	{
		indent(); fprintf(out, "diffuseColor [\n");
		IncIndent();
		for(int i=0; i<(int)dcolors.size(); i++)
		{
			indent(); writeVector(dcolors[i]);
			if(i==(int)dcolors.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
	}
	
	if(AColorValid)
	{
		indent(); fprintf(out, "ambientColor [\n");
		IncIndent();
		for(int i=0; i<(int)dcolors.size(); i++)
		{
			indent(); writeVector(acolor);
			if(i==(int)dcolors.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
	}
	
	if(SColorValid)
	{
		indent(); fprintf(out, "specularColor [\n");
		IncIndent();
		for(int i=0; i<(int)dcolors.size(); i++)
		{
			indent(); writeVector(scolor);
			if(i==(int)dcolors.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
	}
	
	if(EColorValid)
	{
		indent(); fprintf(out, "emissiveColor [\n");
		IncIndent();
		for(int i=0; i< (int)dcolors.size(); i++)
		{
			indent(); writeVector(ecolor);
			if(i==(int)dcolors.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
	}
	
	if(ShininessValid)
	{
		indent(); fprintf(out, "shininess [\n");
		IncIndent();
		for(int i=0; i< (int)dcolors.size(); i++)
		{
			indent();
			if(i==(int)dcolors.size()-1)
				fprintf(out, "%0.3f]\n\n", shininess / 128.0);
			else
				fprintf(out, "%0.3f,\n", shininess / 128.0);
		}
		DecIndent();
	}
	
	indent(); fprintf(out, "transparency [\n");
	IncIndent();
	if(alphas.size() == dcolors.size())
	{
		// Alpha per vertex.
		for(int i=0; i< (int)alphas.size(); i++)
		{
			indent();
			if(i==(int)alphas.size()-1)
				fprintf(out, "%0.3f]\n\n", 1-alphas[i]);
			else
				fprintf(out, "%0.3f,\n", 1-alphas[i]);
		}
	}
	else if(alphas.size() <= 1)
	{
		// Overall alpha
		float Alpha = 0; // This is lame.
		if(alphas.size() == 1)
			Alpha = 1 - alphas[0];
		
		for(int i=0; i< (int)dcolors.size(); i++)
		{
			indent();
			if(i==(int)dcolors.size()-1)
				fprintf(out, "%0.3f]\n\n", Alpha);
			else
				fprintf(out, "%0.3f,\n", Alpha);
		}
	}
	
	DecIndent();
	
	DecIndent();
	indent(); fprintf(out, "}\n\n");
	
	indent(); fprintf(out, "MaterialBinding\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	indent();
	if(dcolors.size() <= 1)
		fprintf(out, "value OVERALL\n");
	else
		fprintf(out, "value PER_VERTEX\n");
	DecIndent();
	indent(); fprintf(out, "}\n\n");
}

void WritableVRMLTriObject::writeNormals()
{
	if(normals.size())
	{
		indent(); fprintf(out, "Normal\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "vector [\n");
		IncIndent();
		for(int i=0; i< (int)normals.size(); i++)
		{
			indent(); writeVector(normals[i]);
			if(i==(int)normals.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
		
		indent(); fprintf(out, "NormalBinding\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		indent();
		if(normals.size() <= 1)
			fprintf(out, "value OVERALL\n");
		else
			fprintf(out, "value PER_VERTEX\n");
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

// XXX Handle paths here.
void WritableVRMLTriObject::writeTexCoords()
{
	if(texcoords.size())
	{
		// Even if there is no texture loaded we will put this node in
		// the file to make it easy to specify one later.
		indent(); fprintf(out, "Texture2\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		indent(); fprintf(out, "filename \"%s\"\n", (TexPtr ? TexPtr->TexFName : ""));
		DecIndent();
		indent(); fprintf(out, "}\n\n");
		
		indent(); fprintf(out, "TextureCoordinate2\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "point [\n");
		IncIndent();
		for(int i=0; i< (int)texcoords.size(); i++)
		{
			indent(); fprintf(out, "%0.3f %0.3f", texcoords[i].x, texcoords[i].y);
			if(i==(int)texcoords.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

void WritableVRMLTriObject::writeVertices()
{
	if(verts.size())
	{
		indent(); fprintf(out, "Coordinate3\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "point [\n");
		IncIndent();
		for(int i=0; i< (int)verts.size(); i++) {
			indent(); writeVector(verts[i]);
			if(i==(int)verts.size()-1)
				fprintf(out, "]\n\n");
			else
				fprintf(out, ",\n");
		}
		DecIndent();
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

void WritableVRMLTriObject::writeIndices()
{
	if(verts.size()) {
		indent(); fprintf(out, "IndexedFaceSet\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "coordIndex [\n");
		IncIndent();
		for(int i=0; i< (int)verts.size();)
		{
			indent();
			for(int j=0; j<((PrimType==L_TRIANGLES)?3:4); j++)
				fprintf(out, "%d, ", i++);
			if(i==(int)verts.size())
				fprintf(out, "-1]\n\n");
			else
				fprintf(out, "-1,\n");
		}
		DecIndent();
		
		if(dcolors.size() > 1)
		{
			// Have a material per vertex.
			indent(); fprintf(out, "materialIndex [\n");
			IncIndent();
			for(int i=0; i< (int)verts.size();)
			{
				indent();
				for(int j=0; j<((PrimType==L_TRIANGLES)?3:4); j++)
					fprintf(out, "%d, ", i++);
				if(i==(int)verts.size())
					fprintf(out, "-1]\n\n");
				else
					fprintf(out, "-1,\n");
			}
			DecIndent();
		}
		
		if(normals.size() > 1)
		{
			// Have a normal per vertex.
			indent(); fprintf(out, "normalIndex [\n");
			IncIndent();
			for(int i=0; i< (int)verts.size();)
			{
				indent();
				for(int j=0; j<((PrimType==L_TRIANGLES)?3:4); j++)
					fprintf(out, "%d, ", i++);
				if(i==(int)verts.size())
					fprintf(out, "-1]\n\n");
				else
					fprintf(out, "-1,\n");
			}
			DecIndent();
		}
		
		if(texcoords.size() > 1)
		{
			// Have a texcoord per vertex.
			indent(); fprintf(out, "textureCoordIndex [\n");
			IncIndent();
			for(int i=0; i< (int)verts.size();)
			{
				indent();
				for(int j=0; j<((PrimType==L_TRIANGLES)?3:4); j++)
					fprintf(out, "%d, ", i++);
				if(i==(int)verts.size())
					fprintf(out, "-1]\n\n");
				else
					fprintf(out, "-1,\n");
			}
			DecIndent();
		}
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

void WritableVRMLTriObject::Write(FILE *_out, int ind)
{
	out = _out;
	Ind = ind;
	
	indent(); fprintf(out, "Separator\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	writeMaterials();
	writeNormals();
	writeTexCoords();
	writeVertices();
	writeIndices();
	DecIndent();
	
	indent(); fprintf(out, "}\n");
}

//////////////////////////////////////////////////////////////////////

// Assume an eight-space tab.
void WritableVRMLMesh::indent()
{
	if(DoIndent)
    {
		int i;
		for(i=0; i<Ind-8; i+=8) fprintf(out, "\t");
		for(; i<Ind; i++) fprintf(out, " ");
    }
}

void WritableVRMLMesh::writeVector(const Vector &V)
{
	fprintf(out, "%0.5f %0.5f %0.5f", V.x, V.y, V.z);
}

// Write materials in one of three ways:
// * A single material or
// * A per-vertex diffuse list and the rest being empty
// * A per-vertex diffuse list and the other values being replicated.
void WritableVRMLMesh::writeMaterials()
{
	if(!((VertexType & OBJ_COLORS) || DColorValid || SColorValid ||
		EColorValid || AColorValid || ShininessValid || AlphaValid))
		return;
	
	indent(); fprintf(out, "Material\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	if(VertexType & OBJ_COLORS)
    {
		indent(); fprintf(out, "diffuseColor [\n");
		IncIndent();
		int vcnt = 0;
		for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next, vcnt++)
		{
			indent(); writeVector(V->Col);
			if(V->next)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
		
		ASSERT_R(vcnt == VertexCount);
		
		// Output the other valid attributes, replicated per vertex.
		if(AColorValid)
		{
			indent(); fprintf(out, "ambientColor [\n");
			IncIndent();
			for(int i=0; i<vcnt; i++)
			{
				indent(); writeVector(acolor);
				if(i==vcnt-1)
					fprintf(out, "]\n\n");
				else
					fprintf(out, ",\n");
			}
			DecIndent();
		}
		
		if(SColorValid)
		{
			indent(); fprintf(out, "specularColor [\n");
			IncIndent();
			for(int i=0; i<vcnt; i++)
			{
				indent(); writeVector(scolor);
				if(i==vcnt-1)
					fprintf(out, "]\n\n");
				else
					fprintf(out, ",\n");
			}
			DecIndent();
		}
		
		if(EColorValid)
		{
			indent(); fprintf(out, "emissiveColor [\n");
			IncIndent();
			for(int i=0; i<vcnt; i++)
			{
				indent(); writeVector(ecolor);
				if(i==vcnt-1)
					fprintf(out, "]\n\n");
				else
					fprintf(out, ",\n");
			}
			DecIndent();
		}
		
		if(ShininessValid)
		{
			indent(); fprintf(out, "shininess [\n");
			IncIndent();
			for(int i=0; i<vcnt; i++)
			{
				indent();
				if(i==vcnt-1)
					fprintf(out, "%0.3f]\n\n", shininess / 128.0);
				else
					fprintf(out, "%0.3f,\n", shininess / 128.0);
			}
			DecIndent();
		}
		
		if(AlphaValid)
		{
			indent(); fprintf(out, "transparency [\n");
			IncIndent();
			for(int i=0; i<vcnt; i++)
			{
				indent();
				if(i==vcnt-1)
					fprintf(out, "%0.3f]\n\n", 1 - alpha);
				else
					fprintf(out, "%0.3f,\n", 1 - alpha);
			}
			DecIndent();
		}
    }
	else
    {
		// There is no per-vertex color.
		// Output a single material.
		
		if(DColorValid)
		{
			indent(); fprintf(out, "diffuseColor [");
			writeVector(dcolor);
			fprintf(out, "]\n\n");
		}
		
		if(AColorValid)
		{
			indent(); fprintf(out, "ambientColor [");
			writeVector(acolor);
			fprintf(out, "]\n\n");
		}
		
		if(SColorValid)
		{
			indent(); fprintf(out, "specularColor [");
			writeVector(scolor);
			fprintf(out, "]\n\n");
		}
		
		if(EColorValid)
		{
			indent(); fprintf(out, "emissiveColor [");
			writeVector(ecolor);
			fprintf(out, "]\n\n");
		}
		
		if(ShininessValid)
		{
			indent(); fprintf(out, "shininess [");
			fprintf(out, "%0.3f]\n\n", shininess / 128.0);
		}
		
		if(AlphaValid)
		{
			indent(); fprintf(out, "transparency [\n");
			fprintf(out, "%0.3f]\n\n", 1-alpha);
		}
    }
	
	DecIndent();
	
	DecIndent();
	indent(); fprintf(out, "}\n\n");
	
	indent(); fprintf(out, "MaterialBinding\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	indent();
	if(VertexType & OBJ_COLORS)
		fprintf(out, "value OVERALL\n");
	else
		fprintf(out, "value PER_VERTEX_INDEXED\n");
	DecIndent();
	indent(); fprintf(out, "}\n\n");
}

void WritableVRMLMesh::writeNormals()
{
	if(VertexType & OBJ_NORMALS)
	{
		indent(); fprintf(out, "Normal\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "vector [\n");
		IncIndent();
		for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next)
		{
			indent(); writeVector(V->Nor);
			if(V->next)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
		
		indent(); fprintf(out, "NormalBinding\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		indent(); fprintf(out, "value PER_VERTEX_INDEXED\n");
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

void WritableVRMLMesh::writeTexCoords()
{
	if(VertexType & OBJ_TEXCOORDS)
	{
		// Even if there is no texture loaded we will put this node in
		// the file to make it easy to specify one later.
		indent(); fprintf(out, "Texture2\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		indent(); fprintf(out, "filename \"%s\"\n", (TexPtr ? TexPtr->TexFName : ""));
		DecIndent();
		indent(); fprintf(out, "}\n\n");
		
		indent(); fprintf(out, "TextureCoordinate2\n");
		indent(); fprintf(out, "{\n");
		IncIndent();
		
		indent(); fprintf(out, "point [\n");
		IncIndent();
		
		for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next)
		{
			indent(); fprintf(out, "%0.3f %0.3f", V->Tex.x, V->Tex.y);
			if(V->next)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
		
		DecIndent();
		indent(); fprintf(out, "}\n\n");
	}
}

void WritableVRMLMesh::writeVertices()
{
	if(!Verts)
		return;
	
	indent(); fprintf(out, "Coordinate3\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	indent(); fprintf(out, "point [\n");
	IncIndent();
	for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next)
	{
		indent(); writeVector(V->V);
		if(V->next)
			fprintf(out, ",\n");
		else
			fprintf(out, "]\n");
	}
	DecIndent();
	
	DecIndent();
	indent(); fprintf(out, "}\n\n");
}

void WritableVRMLMesh::writeIndices()
{
	if(Verts == NULL || Faces == NULL)
		return;
	
	// Create a map of vertex pointers to indices.
	map<Vertex *, int> VIndices;
	
	int ind = 0, i;
	for(Vertex *V = Verts; V; V = V->next, ind++)
		VIndices[V] = ind;
	
	// Make a list of the vertex indices in order.
	vector<int> VIndList;
	for(Face *F = Faces; F; F = F->next)
	{
		VIndList.push_back(VIndices[F->v0]);
		VIndList.push_back(VIndices[F->v1]);
		VIndList.push_back(VIndices[F->v2]);
	}
	
	indent(); fprintf(out, "IndexedFaceSet\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	indent(); fprintf(out, "coordIndex [\n");
	IncIndent();
	for(i=0; i<(int)VIndList.size(); )
	{
		indent();
		int i0 = VIndList[i++];
		int i1 = VIndList[i++];
		int i2 = VIndList[i++];
		fprintf(out, "%d, %d, %d, -1", i0, i1, i2);
		
		if(i < (int)VIndList.size()-1)
			fprintf(out, ",\n");
		else
			fprintf(out, "]\n\n");
	}
	DecIndent();
	
	if(VertexType & OBJ_COLORS)
	{
		// Have a material per vertex.
		indent(); fprintf(out, "materialIndex [\n");
		IncIndent();
		for(i=0; i<(int)VIndList.size(); )
		{
			indent();
			int i0 = VIndList[i++];
			int i1 = VIndList[i++];
			int i2 = VIndList[i++];
			fprintf(out, "%d, %d, %d, -1", i0, i1, i2);
			
			if(i < (int)VIndList.size()-1)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
	}
	
	if(VertexType & OBJ_NORMALS)
	{
		// Have a normal per vertex.
		indent(); fprintf(out, "normalIndex [\n");
		IncIndent();
		for(i=0; i<(int)VIndList.size(); )
		{
			indent();
			int i0 = VIndList[i++];
			int i1 = VIndList[i++];
			int i2 = VIndList[i++];
			fprintf(out, "%d, %d, %d, -1", i0, i1, i2);
			
			if(i < (int)VIndList.size()-1)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
	}
	
	if(VertexType & OBJ_TEXCOORDS)
	{
		// Have a texcoord per vertex.
		indent(); fprintf(out, "textureCoordIndex [\n");
		IncIndent();
		for(i=0; i<(int)VIndList.size(); )
		{
			indent();
			int i0 = VIndList[i++];
			int i1 = VIndList[i++];
			int i2 = VIndList[i++];
			fprintf(out, "%d, %d, %d, -1", i0, i1, i2);
			
			if(i < (int)VIndList.size()-1)
				fprintf(out, ",\n");
			else
				fprintf(out, "]\n\n");
		}
		DecIndent();
	}
	
	DecIndent();
	indent(); fprintf(out, "}\n\n");
}

void WritableVRMLMesh::Write(FILE *_out, int ind)
{
	out = _out;
	Ind = ind;
	
	indent(); fprintf(out, "Separator\n");
	indent(); fprintf(out, "{\n");
	IncIndent();
	
	writeMaterials();
	writeNormals();
	writeTexCoords();
	writeVertices();
	writeIndices();
	DecIndent();
	
	indent(); fprintf(out, "}\n");
}

// Returns false on success.
bool Model::SaveVRML(const char *fname)
{
	FILE *out = fopen(fname, "w");
	ASSERT_RM(out, "Couldn't open file to save VRML file.");
	
	fprintf(out, "#VRML V1.0 ascii\n\n");
	
	fprintf(out, "Separator\n{\n ShapeHints\n {\n vertexOrdering COUNTERCLOCKWISE\n creaseAngle %0.3f\n }\n\n",
		Objs[0]->creaseAngle);
	
	for(int i=0; i<(int)Objs.size(); i++) {
		cerr << "Saving Object: " << i << endl;
        if(Objs[i]->ObjectType == DMC_TRI_OBJECT)
            WritableVRMLTriObject(*((TriObject *)Objs[i])).Write(out, 2);
        else
            WritableVRMLMesh(*((Mesh *)Objs[i])).Write(out, 2);
	}
	
	fprintf(out, "}\n");
	
	fclose(out);
	
	return false;
}

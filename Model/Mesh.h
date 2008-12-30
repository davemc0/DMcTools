//////////////////////////////////////////////////////////////////////
// Mesh.h - Represent a 3D geometric object as a mesh.
//
// Copyright David K. McAllister, Aug. 1999.
//
// This represents arbitrary non-manifold meshes of triangles, edges, and faces.
// It is a kind of BaseObject. It represents a single primitive group of a model.
// Many meshes constitute a Model. It can import and export a TriObject.
// It can export a RenderObject.

#ifndef mesh_h
#define mesh_h

#define DMC_MESH_DEBUG

#include "Model/MeshElements.h"
#include "Model/KDVertex.h"
#include "Math/KDBoxTree.h"
#include "Model/TriObject.h"
#include "Model/RenderObject.h"

#include <set>
#include <vector>
#include <iostream>

struct Mesh : public BaseObject
{
    Vertex *Verts;
    Edge *Edges;
    Face *Faces;

    // This tree of pointers is used for quickly matching a vertex,
    // especially when adding a new vertex to the mesh.
    // It is a pointer so that it won't automatically get copied
    // when I copy a mesh.
    KDBoxTree<KDVertex> *VertTree;

    // When making a mesh, this tells how close two vertices must
    // be to be considered the same.
    double MeshMaxDist;

    bool FacesAreFixed; // True when FixFacing() has been run and nothing has changed since then.

    DMC_INLINE Mesh()
    {
        MeshMaxDist = 0;
        Verts = NULL;
        Edges = NULL;
        Faces = NULL;
        VertTree = NULL;
        ObjectType = DMC_MESH_OBJECT;
        FacesAreFixed = false;
    }

    // Make a Mesh from the given TriObject.
    DMC_INLINE Mesh(const TriObject &M, Vertex *(*VF)()=NULL,
        Edge *(*EF)()=NULL, Face *(*FF)()=NULL)
    {
        ObjectType = DMC_MESH_OBJECT;
        MeshMaxDist = 0;
        Verts = NULL;
        Edges = NULL;
        Faces = NULL;
        VertTree = NULL;
        EdgeCount = VertexCount = FaceCount = 0;
        EdgeType = VertexType = FaceType = 0;
        ImportTriObject(M, -1.0, VF, EF, FF);

        Name[0] = '\0';
        TexPtr = NULL;
        ObjID = -1;
        SColorValid = EColorValid = AColorValid = false;
        DColorValid = ShininessValid = AlphaValid = false;
        creaseAngle = M_PI * 0.5;
        shininess = 32.0;
        alpha = 1.0;
        dcolor = Vector(0,1,0);
        scolor = Vector(0,0,0);
        ecolor = Vector(0,0,0);
        acolor = Vector(0.2,0.2,0.2);
        FacesAreFixed = false;
    }

    ~Mesh();

    /////////////////////////////////////////////////////////////////
    // All subclasses of BaseObject must implement these:

    virtual void Dump() const
    {
        std::cerr << "Mesh vert count: " << VertexCount << " edge count: "
                  << EdgeCount << " face count: " << FaceCount << std::endl;
    }

    // For meshes with attributes, generates the given attribute.
    // based on the geometry of the mesh.
    // You need to make sure the facing is consistent before calling this.
    // Sets the OBJ_WHATEVER flag.
    virtual void GenColors() {ASSERT_R(0);}
    virtual void GenNormals();
    virtual void GenTexCoords();
    virtual void GenTangents();

    // Clears the OBJ_WHATEVER flag.
    virtual void RemoveColors() {VertexType = VertexType & (~OBJ_COLORS);}
    virtual void RemoveNormals() {VertexType = VertexType & (~OBJ_NORMALS);}
    virtual void RemoveTexCoords() {VertexType = VertexType & (~OBJ_TEXCOORDS);}
    virtual void RemoveTangents() {VertexType = VertexType & (~OBJ_TANGENTS);}

    virtual void RebuildBBox();

    // Transform all verticess by this matrix.
    // Also rebuilds the BBox.
    virtual void ApplyTransform(Matrix44 &Mat);

    // Transform all texcoords by this matrix.
    virtual void ApplyTextureTransform(Matrix44 &Mat);

    void GenColorsFromFaceColors();
    void GenNormalsFromFaceNormals();
    void GenFaceNormals();
    void GenTangentsFromFaceTangents();
    void GenFaceTangents();

    /////////////////////////////////////////////////////////////////

    // Add the given TriObject to the mesh.
    // If you want the MeshMaxDist to be set as a multiple of the
    // bounding box size then pass in that multiple here.
    // Otherwise it uses the existing value of MeshMaxDist.
    //
    // The elements created are base Vertex, Edge, and Face unless
    // the incoming TriObject has normals, colors, or texture
    // coordinates. In this case, the vertices are actually
    // AVertex objects.
    //
    // To make something else, pass in a factory function for it.
    void ImportTriObject(const TriObject &M, double MeshDistFactor = -1.0,
        Vertex *(*VF)()=NULL, Edge *(*EF)()=NULL, Face *(*FF)()=NULL);

    // Return a TriObject made from this Mesh.
    // AcceptedAttribs tells what attributes to export if they exist.
    // The mask is defined in AElements.h.
    void ExportTriObject(TriObject &Ob, unsigned int AcceptedAttribs = OBJ_ALL);

    // Convert the Mesh to a RenderObject and return it.
    // AcceptedAttribs has bits to tell what vertex properties to export,
    // assuming they exist. It is a mask of OBJ_ bits from AElements.h.
    // Doesn't generate non-existing required attribs. Do that beforehand.
    void ExportRenderObject(RenderObject &Ob, unsigned int AcceptedAttribs = OBJ_ALL);

    // Make every face in the mesh get the same winding as the
    // face listed first in the linked list.
    void FixFacing();

    // Remove vertices that have no edges or faces.
    void RemoveUnusedVertices();

    /////////////////////////////////////////////////////////////////
    // The debug interface.

    // Makes sure the mesh is sane and counts everything, too.
    void CheckIntegrity(const bool Detailed = false);
    bool CheckSize(const BBox<Vector> &Box);

    /////////////////////////////////////////////////////////////////
    // The single-element interface.

    // Add the vertex without seeing if it already exists.
    // Doesn't make anything point to this vertex.
    // If the Vertex is really a subclass, make it yourself and pass it in.
    DMC_INLINE Vertex *AddVertex(const Vector &Pos, Vertex *Ver = NULL)
    {
        if(Ver == NULL)
            Ver = new Vertex;
        Ver->V = Pos;

        Ver->next = Verts;
        Ver->prev = NULL;
        if(Verts)
            Verts->prev = Ver;
        Verts = Ver;
        VertexCount++;

        return Ver;
    }

    // Makes the vertices point to this edge.
    // If the Edge is really a subclass, make it yourself and pass it in.
    DMC_INLINE Edge *AddEdge(Vertex *v0, Vertex *v1, Edge *E = NULL)
    {
        // Add the edge to the front.
        if(E == NULL)
            E = new Edge;
        E->v0 = v0;
        E->v1 = v1;
        E->next = Edges;
        E->prev = NULL;
        if(Edges)
            Edges->prev = E;
        Edges = E;
        EdgeCount++;

        v0->Edges.push_back(E);
        v1->Edges.push_back(E);

        return E;
    }

    // Makes the edges and vertices point to this face.
    // If the Face is really a subclass, make it yourself and pass it in.
    DMC_INLINE Face *AddFace(Vertex *v0, Vertex *v1, Vertex *v2, Edge *e0, Edge *e1, Edge *e2, Face *F = NULL)
    {
        // Create the face.
        if(F == NULL)
            F = new Face;
        F->next = Faces;
        F->prev = NULL;
        if(Faces)
            Faces->prev = F;
        Faces = F;
        F->v0 = v0;
        F->v1 = v1;
        F->v2 = v2;
        F->e0 = e0;
        F->e1 = e1;
        F->e2 = e2;

        // Add the face index to the vertices and edges.
        v0->Faces.push_back(F);
        v1->Faces.push_back(F);
        v2->Faces.push_back(F);
        e0->Faces.push_back(F);
        e1->Faces.push_back(F);
        e2->Faces.push_back(F);

        FaceCount++;

        return F;
    }

    DMC_INLINE Vertex *FindVertex(const Vector &V)
    {
        Vertex Ver;
        Ver.V = V;
        KDVertex InV(&Ver);
        std::cerr << "F";
        KDVertex OutV;
        if(VertTree->find(InV, OutV))
        {
            return OutV.Vert;
        }
        else
            return NULL;
    }

    Vertex *FindVertexInEdgeList(const std::vector<Edge *> &EdgeList, const Vector &V,
        Edge * &FoundEdge) const;

    // Searches these vertices to find an edge between them.
    // Returns NULL if the edge doesn't exist.
    Edge *FindEdge(Vertex *v0, Vertex *v1, Edge *(*EF)()=NULL);

    DMC_INLINE void DeleteVertex(Vertex *V)
    {
        if(V->next) {
            // Not list tail.
            V->next->prev = V->prev;
        }
        if(V->prev) {
            V->prev->next = V->next;
        } else {
            // List head.
            Verts = V->next;
        }
        VertexCount--;
        V->next = V->prev = NULL;

        delete V;
    }

private:
    // Returns a count of flipped faces.
    int FlipMe(Face *F, std::set<Face *> &Visited,
        Vertex *v0, Vertex *v1, Vertex *v2);

    // Called by SplitVertexAtFace.
    Edge *DoEdge(Edge *E, Vertex *V, Vertex *SplitV);

    Vertex *SplitVertexAtFace(Face *F, Vertex *V, Vertex *SV);
};

#endif

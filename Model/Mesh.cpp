//////////////////////////////////////////////////////////////////////
// Mesh.cpp - Represent a model as a mesh.
//
// Copyright David K. McAllister, Aug. 1999.

#include "Model/Mesh.h"
#include "Model/AElements.h"

#include <map>
using namespace std;

// The factory functions create an item of the correct type,
// but return it as the base class.
Vertex *VertexFactory()
{
    return new Vertex;
}

Edge *EdgeFactory()
{
    return new Edge;
}

Face *FaceFactory()
{
    return new Face;
}

Vertex *AVertexFactory()
{
    return new AVertex;
}

Edge *AEdgeFactory()
{
    return new AEdge;
}

Face *AFaceFactory()
{
    return new AFace;
}

// Return true if vector is bad.
DMC_INLINE bool CheckVec(const Vector &V)
{
    double len2 = V.length2();
    return (len2 < 0.97 || len2 > 1.03 || !Finite(V.x) || !Finite(V.y) || !Finite(V.z));
}

// Return true if vector is bad.
DMC_INLINE bool CompareVecs(const Vector &V0, const Vector &V1, double Eps = 1e-1)
{
    Vector F = Abs(CompDiv(V0, V1) - 1.);
    //if( F.x>Eps || F.y>Eps || F.z>Eps)
    //    cerr << F << endl;
    return F.x>Eps || F.y>Eps || F.z>Eps;
}

// The single-element interface.

// Given a vertex, find a vertex in the edge list that should be adjacent.
Vertex *Mesh::FindVertexInEdgeList(const vector<Edge *> &Edg, const Vector &V, Edge * &e) const
{
    for(int j=0; j<(int)Edg.size(); j++) {
        if(VecEq(Edg[j]->v0->V, V, Sqr(MeshMaxDist))) {
            e = Edg[j]; return Edg[j]->v0;
        } else if(VecEq(Edg[j]->v1->V, V, Sqr(MeshMaxDist))) {
            e = Edg[j]; return Edg[j]->v1;
        }
    }

    e = NULL;
    return NULL;
}

// Returns a pointer to this edge. Creates it if necessary.
Edge *Mesh::FindEdge(Vertex *v0, Vertex *v1, Edge *(*EF)())
{
    ASSERT_D(v0 != v1);

    for(int i=0; i<(int)v0->Edges.size(); i++) {
        Edge *Ed = v0->Edges[i];
        if((Ed->v0 == v0 && Ed->v1 == v1) || (Ed->v0 == v1 && Ed->v1 == v0))
            return Ed;
    }

    Edge *newE = EF ? EF() : NULL;
    Edge *N = AddEdge(v0, v1, newE);

    return N;
}

Mesh::~Mesh()
{
    // cerr << int(this) << " ~Mesh\n";

    while(Verts)
    {
        Vertex *tmp = Verts;
        Verts = Verts->next;
        if(VertexType & HAS_ATTRIBS)
            delete (AVertex *)tmp;
        else
            delete tmp;
    }

    while(Edges)
    {
        Edge *tmp = Edges;
        Edges = Edges->next;
        if(EdgeType & HAS_ATTRIBS)
            delete (AEdge *)tmp;
        else
            delete tmp;
    }

    while(Faces)
    {
        Face *tmp = Faces;
        Faces = Faces->next;
        if(FaceType & HAS_ATTRIBS)
            delete (AFace *)tmp;
        else
            delete tmp;
    }

    if(VertTree)
        delete VertTree;
}

// Import the incoming TriObject into this Mesh.
// This Mesh may either be populated or not.
void Mesh::ImportTriObject(const TriObject &Ob, double MeshDistFactor,
                           Vertex *(*VF)(), Edge *(*EF)(), Face *(*FF)())
{
    // ASSERT_RM(0, "There's a bug in here.");
    //cerr << "Converting TriObject to Mesh.\n";

    // Set the MeshMaxDist relative to the bounding box.
    if(MeshDistFactor >= 0)
        MeshMaxDist = (Ob.Box.MaxV - Ob.Box.MinV).length() * MeshDistFactor;

    WARN_R(Ob.verts.size() % 3 == 0, "TriObject must be multiple of three vertices.");
    ASSERT_R(Ob.VertexCount == (int)Ob.verts.size());

    if(VertTree == NULL)
        VertTree = new KDBoxTree<KDVertex>;
    ASSERT_RM(VertTree, "memory alloc failed");

    // These tell whether we are extracting attributes from the TriObject.
    bool DoingColor = false, DoingNormals = false, DoingTexcoords = false;

    if(Verts == NULL) {
        // It's a new Mesh. We decide its type.
        // If it's an existing mesh we don't mess with any bits.

        DoingColor = Ob.verts.size() == Ob.dcolors.size();
        DoingNormals = Ob.verts.size() == Ob.normals.size();
        DoingTexcoords = Ob.verts.size() == Ob.texcoords.size();

        VertexType |= DoingColor ? OBJ_COLORS : 0;
        VertexType |= DoingNormals ? OBJ_NORMALS : 0;
        VertexType |= DoingTexcoords ? OBJ_TEXCOORDS : 0;
        VertexType |= (VertexType & OBJ_ALL) ? HAS_ATTRIBS : 0;
    }

    if(VertexType & OBJ_COLORS) cerr << "Mesh has color.\n";
    if(VertexType & OBJ_NORMALS) cerr << "Mesh has normals.\n";
    if(VertexType & OBJ_TEXCOORDS) cerr << "Mesh has texcoords.\n";
    if(VertexType & OBJ_TANGENTS) cerr << "Mesh has tangents.\n";

    if((VertexType & HAS_ATTRIBS) && VF==NULL)
        VF = AVertexFactory;
    if((EdgeType & HAS_ATTRIBS) && EF==NULL)
        EF = AEdgeFactory;
    if((FaceType & HAS_ATTRIBS) && FF==NULL)
        FF = AFaceFactory;

    // cerr << ((int)Ob.verts.size() / 3) << " triangles.\n";
    for(int i=0; i<(int)Ob.verts.size(); i+=3) {
        cerr << Ob.verts[i] << endl;

        bool AddedVert = false;
        Edge *e0 = NULL, *e1 = NULL, *e2 = NULL;

        Vertex *v0 = NULL;
        if(!(v0 = FindVertex(Ob.verts[i])))
        {
            cerr << "Couldn't find it.\n";
            if(VertexType & HAS_ATTRIBS)
            {
                v0 = VF();
                if(DoingColor) ((AVertex *)v0)->Col = Ob.dcolors[i+0];
                if(DoingNormals) ((AVertex *)v0)->Nor = Ob.normals[i+0];
                if(DoingTexcoords) ((AVertex *)v0)->Tex = Ob.texcoords[i+0];
            }
            cerr << 'o';
            /// XXX There's a bug in the KD Tree.
            v0 = AddVertex(Ob.verts[i], v0);
            VertTree->insert(KDVertex(v0));
            AddedVert = true;
        }
        cerr << 'q';

        Vertex *v1 = FindVertexInEdgeList(v0->Edges, Ob.verts[i+1], e0);

        if(v0 == v1) // Degenerate triangle.
            continue;
        cerr << 'a';
        if(v1 == NULL)
        {
            if(!(v1 = FindVertex(Ob.verts[i+1])))
            {
                if(VertexType & HAS_ATTRIBS)
                {
                    v1 = VF();
                    if(DoingColor) ((AVertex *)v1)->Col = Ob.dcolors[i+1];
                    if(DoingNormals) ((AVertex *)v1)->Nor = Ob.normals[i+1];
                    if(DoingTexcoords) ((AVertex *)v1)->Tex = Ob.texcoords[i+1];
                }

                v1 = AddVertex(Ob.verts[i+1], v1);
                VertTree->insert(KDVertex(v1));
                AddedVert = true;
            }

            if(v0 == v1) // Degenerate triangle.
                continue;

            // Need to add e01 to all lists.
            e0 = FindEdge(v0, v1, EF);
        }
        cerr << 'b';

        Vertex *v2 = FindVertexInEdgeList(v0->Edges, Ob.verts[i+2], e1);

        Vertex *vt = FindVertexInEdgeList(v1->Edges, Ob.verts[i+2], e2);
        cerr << 'c';

        if(v2 == NULL)
            v2 = vt;
        if(v2 == NULL)
        {
            if(!(v2 = FindVertex(Ob.verts[i+2])))
            {
                if(VertexType & HAS_ATTRIBS)
                {
                    v2 = VF();

                    if(DoingColor) ((AVertex *)v2)->Col = Ob.dcolors[i+2];
                    if(DoingNormals) ((AVertex *)v2)->Nor = Ob.normals[i+2];
                    if(DoingTexcoords) ((AVertex *)v2)->Tex = Ob.texcoords[i+2];
                }

                v2 = AddVertex(Ob.verts[i+2], v2);
                VertTree->insert(KDVertex(v2));
                AddedVert = true;
            }
        }
        cerr << 'd';

        if(v2 == v1 || v2 == v0) // Degenerate triangle.
            continue;

        // All three vertices now exist. e1 and e2 might not exist yet.
        if(e1 == NULL)
            // Need to add e02 to all lists.
            e1 = FindEdge(v0, v2, EF);

        if(e2 == NULL)
            // Need to add e12 to all lists.
            e2 = FindEdge(v1, v2, EF);

        WARN_R(v0 != v1 && v1 != v2 && v0 != v2, "Degenerate triangle vertices.");
        WARN_R(e0 != e1 && e1 != e2 && e0 != e2, "Degenerate triangle edges.");
        cerr << 'e';

        // Need some check for unique face.
        if(!AddedVert) {
            // Probably a duplicate face. Check it out, boys.
            // Perform the check by looking at the faces of my first vertex.
            // cerr << "Checking for duplicate face.\n";
            int p;
            for(p = 0; p< (int)v0->Faces.size(); p++) {
                Face *F = v0->Faces[p];
                if((F->v0 == v0 || F->v0 == v1 || F->v0 == v2) &&
                    (F->v1 == v0 || F->v1 == v1 || F->v1 == v2) &&
                    (F->v2 == v0 || F->v2 == v1 || F->v2 == v2)) {
                    cerr << "Duplicate face!\n";
                    break;
                }
            }
            if(p < (int)v0->Faces.size())
                continue;
        }
        cerr << 'f';

        Face *newF = FF ? FF() : NULL;
        AddFace(v0, v1, v2, e0, e1, e2, newF);
        cerr << 'g';

#ifdef DMC_MESH_DEBUG
        CheckIntegrity(true);
#endif
    }
    cerr << "Done\n";

#ifdef DMC_DEBUG
    CheckIntegrity(true);
#else
    CheckIntegrity();
#endif
}

// Convert the Mesh to an TriObject and return it.
// We could put unconnected components in separate Objects, but we don't.
// AcceptedAttribs has bits to tell what vertex properties to export,
// assuming they exist. It is a mask of OBJ_ bits from AElements.h.
void Mesh::ExportTriObject(TriObject &Ob, unsigned int AcceptedAttribs)
{
    for(Face *F = Faces; F; F = F->next) {
        Ob.verts.push_back(F->v0->V);
        Ob.Box += F->v0->V;
        Ob.verts.push_back(F->v1->V);
        Ob.Box += F->v1->V;
        Ob.verts.push_back(F->v2->V);
        Ob.Box += F->v2->V;

        // Export color, texture coordinates, and normals.
        if(VertexType & AcceptedAttribs & OBJ_COLORS) {
            Ob.dcolors.push_back(((AVertex *)F->v0)->Col);
            Ob.dcolors.push_back(((AVertex *)F->v1)->Col);
            Ob.dcolors.push_back(((AVertex *)F->v2)->Col);
        }

        if(VertexType & AcceptedAttribs & OBJ_TEXCOORDS) {
            Ob.texcoords.push_back(((AVertex *)F->v0)->Tex);
            Ob.texcoords.push_back(((AVertex *)F->v1)->Tex);
            Ob.texcoords.push_back(((AVertex *)F->v2)->Tex);
        }

        if(VertexType & AcceptedAttribs & OBJ_NORMALS) {
            Ob.normals.push_back(((AVertex *)F->v0)->Nor);
            Ob.normals.push_back(((AVertex *)F->v1)->Nor);
            Ob.normals.push_back(((AVertex *)F->v2)->Nor);
        }
    }
}

// Convert the Mesh to a RenderObject and return it.
// We could put unconnected Mesh components in separate Objects, but we don't.
// AcceptedAttribs has bits to tell what vertex properties to export,
// assuming they exist. It is a mask of OBJ_ bits from AElements.h.
void Mesh::ExportRenderObject(RenderObject &Ob, unsigned int AcceptedAttribs)
{
    // This is required because we will be operating on AVertex, etc. objects.
    ASSERT_R(VertexType & HAS_ATTRIBS);
    ASSERT_R(EdgeType & HAS_ATTRIBS);
    ASSERT_R(FaceType & HAS_ATTRIBS);

    // cerr << "ExportRenderObject\n";
    // Copy the BaseObject values.
    *(BaseObject *)&Ob = *((BaseObject *)this);
    Ob.ObjectType = DMC_RENDER_OBJECT;
    Ob.VertexType = (VertexType & AcceptedAttribs) | HAS_ATTRIBS; // XXX Are parens in right place?
    Ob.EdgeType = HAS_ATTRIBS;
    Ob.FaceType = HAS_ATTRIBS;

    if(Verts == NULL || Faces == NULL)
        return;

    // Create a map of vertex pointers to indices.
    map<AVertex *, int> VIndMapper;

    int ind = 0;
    for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next, ind++) {
        VIndMapper[V] = ind;

        // Make the data arrays.
        Ob.verts.push_back(f3Vector(V->V));

        if(Ob.VertexType & OBJ_COLORS)
            Ob.dcolors.push_back(f3Vector(V->Col));

        if(Ob.VertexType & OBJ_NORMALS) {
            if(CheckVec(V->Nor)) {
                cerr << "Export: BadNorm: " << Ob.Name << " " << V->Nor << endl;
                V->Nor = Vector(0,0,0);
            }
            Ob.normals.push_back(f3Vector(V->Nor));
        }

        if(Ob.VertexType & OBJ_TEXCOORDS)
            Ob.texcoords.push_back(f3Vector(V->Tex));

        if(Ob.VertexType & OBJ_TANGENTS) {
            if(CheckVec(V->Tan)) {
                // cerr << "Export: BadTan: " << Ob.Name << " " << V->Tan << endl;
                V->Tan = Vector(0,0,0);
            }
            Ob.tangents.push_back(f3Vector(V->Tan));
        }
    }

    // Make sure it made the right amount of data.
    ASSERT_R(Ob.VertexCount == (int)Ob.verts.size());
    if(Ob.VertexType & OBJ_COLORS) {
        ASSERT_R(Ob.VertexCount == (int)Ob.dcolors.size());
    }
    if(Ob.VertexType & OBJ_NORMALS) {
        ASSERT_R(Ob.VertexCount == (int)Ob.normals.size());
    }
    if(Ob.VertexType & OBJ_TEXCOORDS) {
        ASSERT_R(Ob.VertexCount == (int)Ob.texcoords.size());
    }
    if(Ob.VertexType & OBJ_TANGENTS) {
        ASSERT_R(Ob.VertexCount == (int)Ob.tangents.size());
    }

    // Make a list of the vertex indices in order.
    // Three indices per face.
    for(Face *F = Faces; F; F = F->next) {
        int i0 = VIndMapper[(AVertex *)F->v0];
        int i1 = VIndMapper[(AVertex *)F->v1];
        int i2 = VIndMapper[(AVertex *)F->v2];
        ASSERT_R(i0 >= 0 && i0 < Ob.VertexCount);
        ASSERT_R(i1 >= 0 && i1 < Ob.VertexCount);
        ASSERT_R(i2 >= 0 && i2 < Ob.VertexCount);
        Ob.indices.push_back(i0);
        Ob.indices.push_back(i1);
        Ob.indices.push_back(i2);
    }

    // cerr << Ob.Name << fcnt << " " << maxi << " " << ind << endl;

    ASSERT_R(Ob.FaceCount == (int)Ob.indices.size() / 3);
    ASSERT_R((int)Ob.indices.size() % 3 == 0);
}

// This is recursive.
int Mesh::FlipMe(Face *F, set<Face *> &Visited,
                 Vertex *v0, Vertex *v1, Vertex *v2)
{
    if(Visited.find(F) == Visited.end())
        return 0;

    Visited.insert(F);
    int flipcnt = 0;

    WARN_R(((F->v0 == v0) + (F->v1 == v0) + (F->v2 == v0) + (F->v0 == v1) + (F->v1 == v1) + (F->v2 == v1) + (F->v0 == v2) + (F->v1 == v2) + (F->v2 == v2)) >= 2, "Faces should share exactly two vertices.");

    if((F->v0 == v0 && (F->v1 != v1 && F->v2 != v2)) ||
        (F->v1 == v0 && (F->v2 != v1 && F->v0 != v2)) ||
        (F->v2 == v0 && (F->v0 != v1 && F->v1 != v2)) ||

        (F->v0 == v1 && (F->v1 != v2 && F->v2 != v0)) ||
        (F->v1 == v1 && (F->v2 != v2 && F->v0 != v0)) ||
        (F->v2 == v1 && (F->v0 != v2 && F->v1 != v0)) ||

        (F->v0 == v2 && (F->v1 != v0 && F->v2 != v1)) ||
        (F->v1 == v2 && (F->v2 != v0 && F->v0 != v1)) ||
        (F->v2 == v2 && (F->v0 != v0 && F->v1 != v1)))
    {
        // Swap the winding.
        // cerr << "Swapping.\n";
        Vertex *T = F->v1;
        F->v1 = F->v2;
        F->v2 = T;
        flipcnt++;
    }

    // Fix my neighbors.
    int i;
    for(i=0; i<(int)F->e0->Faces.size(); i++)
    {
        if(F->e0->Faces[i] != F)
            flipcnt += FlipMe(F->e0->Faces[i], Visited, F->v0, F->v2, F->v1);
    }

    for(i=0; i<(int)F->e1->Faces.size(); i++)
    {
        if(F->e1->Faces[i] != F)
            flipcnt += FlipMe(F->e1->Faces[i], Visited, F->v0, F->v2, F->v1);
    }

    for(i=0; i<(int)F->e2->Faces.size(); i++)
    {
        if(F->e2->Faces[i] != F)
            flipcnt += FlipMe(F->e2->Faces[i], Visited, F->v0, F->v2, F->v1);
    }

    return flipcnt;
}

// Give all faces in each manifold the same winding.
// Should specify a starting face with proper winding.
// Should make it not recursive. Use bits and own stack.
void Mesh::FixFacing()
{
    if(FacesAreFixed)
        return;

    set<Face *> Visited;

    int flipcnt = 0;

    for(Face *F = Faces; F; F = F->next)
        flipcnt += FlipMe(F, Visited, F->v0, F->v1, F->v2);

    cerr << "Flipped " << flipcnt << endl;

    FacesAreFixed = true;
}

// Debug: See if all the vertices are inside the box.
bool Mesh::CheckSize(const BBox<Vector> &Box)
{
    bool bad = false;
    for(Vertex *V = Verts; V; V = V->next) {
        if(!Box.Inside(V->V)) {
            cerr << "Vertex out of box: " << V->V << endl;
            bad = true;
        }
    }

    return bad;
}

void Mesh::CheckIntegrity(const bool Slow)
{
    // cerr << "Checking mesh integrity.\n";

    int vc = 0, fc = 0, ec = 0;
    // Edges
    for(Edge *E = Edges; E; E = E->next, ec++)
    {
        if(E != Edges)
        {WARN_R(E->prev, "No prev pointer");}

        WARN_R((int)E->Faces.size() > 0, "Edge must have at least one face!");
        if((int)E->Faces.size() < 1 || (int)E->Faces.size() > 2)
            fprintf(stderr, "Nonmanifold Edge: %d 0x%08llx\n", (int)E->Faces.size(), (DMCINT64) 0 /* (E) */);

        WARN_R(E->v0 != E->v1, "Edge with the same vertices!");
        WARN_R(E->v0 && E->v1, "Edge vertex pointer is NULL!");
        int i;
        for(i=0; i< (int)E->Faces.size(); i++)
            WARN_R(E->Faces[i], "Edge face pointer is NULL!");

        for(i=0; i< (int)E->v0->Edges.size(); i++) {
            if(E->v0->Edges[i] == E)
                break;
        }
        WARN_R(i < (int) E->v0->Edges.size(), "Vertex0 doesn't point to its Edge!");

        for(i=0; i< (int)E->v1->Edges.size(); i++) {
            if(E->v1->Edges[i] == E)
                break;
        }
        WARN_R(i < (int) E->v1->Edges.size(), "Vertex1 doesn't point to its Edge!");

        for(i=0; i< (int)E->Faces.size(); i++)
        {
            Face *F = E->Faces[i];
            WARN_R(F->e0 == E || F->e1 == E || F->e2 == E, "Face doesn't point to its Edge!");
        }

        for(i=0; i< (int)E->Faces.size(); i++)
        {
            Face *F0 = E->Faces[i];
            WARN_R(F0->v0, "Edge points to dead face");

            // Make sure there is no duplicate face.
            for(int j=i+1; j< (int)E->Faces.size(); j++)
            {
                Face *F1 = E->Faces[j];
                WARN_R(F0 != F1, "Same face exists twice in an edge");

                // Two faces can't have the same three edges.
                WARN_R(F0->e0 != F1->e0 || F0->e1 != F1->e1 || F0->e2 != F1->e2, "EDup face 0");
                WARN_R(F0->e0 != F1->e0 || F0->e1 != F1->e2 || F0->e2 != F1->e1, "EDup face 0");
                WARN_R(F0->e0 != F1->e1 || F0->e1 != F1->e0 || F0->e2 != F1->e2, "EDup face 1");
                WARN_R(F0->e0 != F1->e1 || F0->e1 != F1->e2 || F0->e2 != F1->e0, "EDup face 1");
                WARN_R(F0->e0 != F1->e2 || F0->e1 != F1->e0 || F0->e2 != F1->e1, "EDup face 2");
                WARN_R(F0->e0 != F1->e2 || F0->e1 != F1->e1 || F0->e2 != F1->e0, "EDup face 2");
            }
        }

        // Only check for a duplicate edge every so often.
        if(Slow)
        {
            for(Edge *E1 = E->next; E1; E1 = E1->next)
            {
                // Make sure there's no duplicate edge.
                WARN_R(E1->v0 != E->v0 || E1->v1 != E->v1, "Duplicate edge");
                WARN_R(E1->v0 != E->v1 || E1->v1 != E->v0, "Duplicate edge");
            }
        }

        WARN_R((E->v0->next || E->v0->prev) && (E->v1->next || E->v1->prev), "Edge points to dead vertex");
    }

    // Faces
    for(Face *F = Faces; F; F = F->next, fc++)
    {
        if(F != Faces)
        {WARN_R(F->prev, "No prev pointer");}

        WARN_R(F->v0 != F->v1 && F->v1 != F->v2 && F->v0 != F->v2, "Face with the same vertices!");
        WARN_R(F->e0 != F->e1 && F->e1 != F->e2 && F->e0 != F->e2, "Face with the same edges!");
        WARN_R(F->e0 && F->e1 && F->e2, "Face has NULL edge pointer!");
        WARN_R(F->v0 && F->v1 && F->v2, "Face has NULL vertex pointer!");

        int i;
        for(i=0; i< (int)F->e0->Faces.size(); i++)
        {
            if(F->e0->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->e0->Faces.size(), "Edge0 doesn't point to its face!");

        for(i=0; i< (int)F->e1->Faces.size(); i++)
        {
            if(F->e1->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->e1->Faces.size(), "Edge1 doesn't point to its face!");

        for(i=0; i< (int)F->e2->Faces.size(); i++)
        {
            if(F->e2->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->e2->Faces.size(), "Edge2 doesn't point to its face!");

        for(i=0; i< (int)F->v0->Faces.size(); i++)
        {
            if(F->v0->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->v0->Faces.size(), "Vertex0 doesn't point to its face!");

        for(i=0; i< (int)F->v1->Faces.size(); i++)
        {
            if(F->v1->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->v1->Faces.size(), "Vertex1 doesn't point to its face!");

        for(i=0; i< (int)F->v2->Faces.size(); i++)
        {
            if(F->v2->Faces[i] == F)
                break;
        }
        WARN_R(i < (int) F->v2->Faces.size(), "Vertex2 doesn't point to its face!");

        // Make sure I don't point to any dead vertices or edges.
        WARN_R((F->v0->next || F->v0->prev) && (F->v1->next || F->v1->prev) && (F->v2->next || F->v2->prev), "Face points to dead vertex");

        WARN_R(F->e0->v0 && F->e1->v0 && F->e2->v0, "Face points to dead edge");

        if(Slow)
        {
            for(Face *F1 = F->next; F1; F1 = F1->next)
            {
                WARN_R(F != F1, "Same face exists twice in the list");

                WARN_R(F->v0 != F1->v0 || F->v1 != F1->v1 || F->v2 != F1->v2, "Dup face 0");
                WARN_R(F->v0 != F1->v0 || F->v1 != F1->v2 || F->v2 != F1->v1, "Dup face 0");
                WARN_R(F->v0 != F1->v1 || F->v1 != F1->v0 || F->v2 != F1->v2, "Dup face 1");
                WARN_R(F->v0 != F1->v1 || F->v1 != F1->v2 || F->v2 != F1->v0, "Dup face 1");
                WARN_R(F->v0 != F1->v2 || F->v1 != F1->v0 || F->v2 != F1->v1, "Dup face 2");
                WARN_R(F->v0 != F1->v2 || F->v1 != F1->v1 || F->v2 != F1->v0, "Dup face 2");
            }
        }
    }

    // Vertices
    for(Vertex *V = Verts; V; V = V->next, vc++) {
        // cerr << 'v';
        if(V != Verts) {
            WARN_R(V->prev, "No prev pointer");
            WARN_R(V->prev->next == V, "Broken vertex link");
        }

        WARN_R(V->Edges.size() >= 2, "Vertex must be part of at least two edges!");
        WARN_R(V->Faces.size() >= 1, "Vertex must be part of at least one face!");

        int i;
        for(i=0; i< (int)V->Edges.size(); i++) {
            if(V->Edges[i]->v0 == V || V->Edges[i]->v1 == V)
                break;
        }
        WARN_R(i < (int) V->Edges.size() || V->Edges.size()==0, "Edge doesn't point to its vertex!");

        for(i=0; i< (int)V->Faces.size(); i++) {
            if(V->Faces[i]->v0 == V || V->Faces[i]->v1 == V || V->Faces[i]->v2 == V)
                break;
        }
        WARN_R(i < (int) V->Faces.size() || V->Faces.size()==0, "Face doesn't point to its vertex!");

        // Make sure I don't point to any dead faces or edges.
        for(i=0; i< (int)V->Edges.size(); i++) {
            WARN_R(V->Edges[i]->Faces.size(), "Vertex points to edge with no face.");

            for(int j=i+1; j< (int)V->Edges.size(); j++)
                WARN_R(V->Edges[i] != V->Edges[j], "Vertex points to the same edge twice!");
         }

        // Make sure I don't have any duplicate faces.
        for(i=0; i< (int)V->Faces.size(); i++) {
            // cerr << "f"<<V->Faces.size();
            Face *F0 = V->Faces[i];
            WARN_R(F0->v0, "Vertex points to dead face");

            // Make sure there is no duplicate face.
            int j;
            for(j=i+1; j< (int)V->Faces.size(); j++) {
                Face *F1 = V->Faces[j];
                WARN_R(F0 != F1, "Same face exists twice in a vertex");

                WARN_R(F0->v0 != F1->v0 || F0->v1 != F1->v1 || F0->v2 != F1->v2, "Dup face 00");
                WARN_R(F0->v0 != F1->v0 || F0->v1 != F1->v2 || F0->v2 != F1->v1, "Dup face 01");
                WARN_R(F0->v0 != F1->v1 || F0->v1 != F1->v0 || F0->v2 != F1->v2, "Dup face 10");
                WARN_R(F0->v0 != F1->v1 || F0->v1 != F1->v2 || F0->v2 != F1->v0, "Dup face 11");
                WARN_R(F0->v0 != F1->v2 || F0->v1 != F1->v0 || F0->v2 != F1->v1, "Dup face 20");
                WARN_R(F0->v0 != F1->v2 || F0->v1 != F1->v1 || F0->v2 != F1->v0, "Dup face 21");
            }

            // Make sure V points to two edges of the face.
            int cnt = 0;
            for(j=0; j< (int)V->Edges.size(); j++) {
                if(V->Edges[j] == F0->e0 || V->Edges[j] == F0->e1 || V->Edges[j] == F0->e2)
                    cnt++;
            }

            // cerr << cnt << endl;
            WARN_R(cnt == 2, "Vertex must have 2 edges of each of its faces.");
        }
    }

    Dump();
    WARN_R(VertexCount == vc, "Bad vertex count");
    WARN_R(FaceCount == fc, "Bad face count");
    WARN_R(EdgeCount == ec, "Bad edge count");
}

// Remove vertices that aren't used.
// Doing this gratuitously could mask algorithm problems that CheckIntegrity would expose.
void Mesh::RemoveUnusedVertices()
{
    // cerr << VertexCount;
    int i=0;
    for(Vertex *V = Verts; V; i++) {
        if(V->Edges.size() == 0 && V->Faces.size() == 0) {
            // cerr << i << ' ';
            Vertex *Vp = V->prev;
            DeleteVertex(V);
            V = Vp ? Vp : Verts; // Handles deleting list head.
        } else if(V->Edges.size() == 0 || V->Faces.size() == 0) {
            // cerr << V->Edges.size() << " " << V->Faces.size() << "huh?\n";
            Vertex *Vp = V->prev;
            DeleteVertex(V);
            V = Vp ? Vp : Verts; // Handles deleting list head.
        } else {
            // cerr << '.';
            V = V->next;
        }
    }
    // cerr << " -> " << VertexCount << endl;
}

/////////////////////////////////////////////////////////////////////

void Mesh::GenColorsFromFaceColors()
{
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(FaceType & OBJ_COLORS, "Faces must have colors.");

    // Accumulate the facet Colors into vertex Colors.
    for(Vertex *V = Verts; V; V = V->next) {
        Vector AccNorm(((AFace *)(V->Faces[0]))->Col);
        // Loop on all the faces of this vertex.
        for(int k=1; k<(int)V->Faces.size(); k++) {
            AccNorm += ((AFace *)(V->Faces[k]))->Col;
        }

        // Now store the Color.
        AccNorm /= V->Faces.size(); // XXX I'm pretty sure we don't need to divide by cnt.
        ((AVertex *)V)->Col = AccNorm;
    }

    VertexType = VertexType | OBJ_COLORS;
}

void Mesh::GenFaceNormals()
{
    ASSERT_RM(FaceType & HAS_ATTRIBS, "Mesh must have attributes.");

    // cerr << 'n';
    // Generate the facet normals.
    for(AFace *F = (AFace *)Faces; F; F = (AFace *)F->next) {
        Vector P0 = F->v0->V - F->v1->V;
        Vector P1 = F->v2->V - F->v1->V;

        Vector N = Vector(Cross(P1, P0));
        N.normalize();

        F->Nor = N;
        if(CheckVec(F->Nor)) {
            cerr << "GenFaceNormals: BadNor: " << Name << F->Nor << endl;
        }
    }

    FaceType = FaceType | OBJ_NORMALS;
}

// Auxiliary function used by SplitVertexAtFace.
Edge *Mesh::DoEdge(Edge *E, Vertex *V, Vertex *SplitV)
{
    Edge *E2 = E;

    if(E->v0 == V || E->v1 == V) {
        // This edge has the vertex I'm splitting.

        if(E->Faces.size() == 0) {
            // I was the only user of it. Reuse it.
            if(E->v0 == V) {E->v0 = SplitV; SplitV->Edges.push_back(E);}
            if(E->v1 == V) {E->v1 = SplitV; SplitV->Edges.push_back(E);}

            // The splitting vertex doesn't use it anymore.
            E->ListRemove(V->Edges);
        } else {
            // It's still in use. Make a new one.
            if(E->v0 == V) E2 = AddEdge(SplitV, E->v1, new AEdge);
            if(E->v1 == V) E2 = AddEdge(E->v0, SplitV, new AEdge);
        }
    } else {
        // This edge doesn't have the vertex I'm splitting.
        // Just use it.
    }

    return E2;
}

// SV is the vertex previously split off of this one. Could be NULL.
Vertex * Mesh::SplitVertexAtFace(Face *F, Vertex *V, Vertex *SV)
{
    // cerr << Name << " " << V->V << " Splitting.\n";

    // CheckIntegrity();

    // Make everything not point to this face.
    F->ListRemove(F->v0->Faces);
    F->ListRemove(F->v1->Faces);
    F->ListRemove(F->v2->Faces);
    F->ListRemove(F->e0->Faces);
    F->ListRemove(F->e1->Faces);
    F->ListRemove(F->e2->Faces);

    // Split it.
    Vertex *SplitV;
    if(SV) {
        SplitV = SV;
    } else {
        SplitV = new AVertex;
        *(AVertex *)SplitV = *(AVertex *)V;
        AddVertex(V->V, SplitV);
        SplitV->Faces.clear();
        SplitV->Edges.clear();
    }

    // fprintf(stderr, "SplitV=0x%x V=0x%x\n", SplitV, V);

    // Set up the edges.
    F->e0 = DoEdge(F->e0, V, SplitV);
    F->e1 = DoEdge(F->e1, V, SplitV);
    F->e2 = DoEdge(F->e2, V, SplitV);

    // Point the face to the new vertex.
    if(F->v0 == V) {
        F->v0 = SplitV;
    } else if(F->v1 == V) {
        F->v1 = SplitV;
    } else if(F->v2 == V) {
        F->v2 = SplitV;
    } else {
        ASSERT_R(0);
    }

    // Add the face index to the vertices and edges.
    F->v0->Faces.push_back(F);
    F->v1->Faces.push_back(F);
    F->v2->Faces.push_back(F);
    F->e0->Faces.push_back(F);
    F->e1->Faces.push_back(F);
    F->e2->Faces.push_back(F);

    // CheckIntegrity();

    return SplitV;
}

void Mesh::GenNormalsFromFaceNormals()
{
    // cerr << Name << " GenNormalsFromFaceNormals " << creaseAngle << endl;

    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(FaceType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(FaceType & OBJ_NORMALS, "Mesh must have facet normals.");

    double CosCrease = cos(creaseAngle);

    bool ReDo = false;
    int i=0;

    // Accumulate the facet normals into vertex normals.
    for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next, i++) {
        // cerr << V->Nor << endl;
        // CheckIntegrity();
        ASSERT_R(V->Faces.size());
        ASSERT_R(V->Faces[0]);

        Vertex *SplitV = NULL;

        Vector AccNorm(((AFace *)(V->Faces[0]))->Nor);
        // Loop on all the rest of the faces of this vertex. Accumulate those with
        // an angle less than creaseAngle into a smooth normal.
        for(int k=1; k<(int)V->Faces.size(); ) {
            AFace *F = (AFace *)V->Faces[k];
            ASSERT_R(F);
#if 1
            // This is for if crease angle and vertex splitting works.
            Vector AccNormN = AccNorm;
            AccNormN.normalize();

            double AngDot = Dot(AccNormN, F->Nor);
            if(AngDot > CosCrease) {
                // Vertex is ok.
                AccNorm += F->Nor;
                k++;
            } else {
                SplitV = SplitVertexAtFace(F, V, SplitV);
                ReDo = true;
            }
#else
            AccNorm += F->Nor;
            k++;
#endif
        }

        // Now store the normal.
        AccNorm.normalize();
        V->Nor = AccNorm;
        if(CheckVec(V->Nor)) {
            cerr << "GenNormalsFromFaceNormals: BadNor: " << Name << V->Nor << endl;
        }
    }

    VertexType = VertexType | OBJ_NORMALS;

    if(ReDo)
        GenNormalsFromFaceNormals();
}

// For meshes with attributes, generates vertex normals based on the
// geometry of the mesh.
//
// You need to make sure the facing is consistent before calling this.
//
// XXX Creases don't work because they require duplicating the vertex and
// storing a different normal at each.
// These could be made to work by splitting the mesh.
// The crease angle is in radians and ranges from 0 to pi.
// If normals already exist nothing is changed.
void Mesh::GenNormals()
{
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(creaseAngle >= 0 && creaseAngle <= M_PI, "Bad creaseAngle.");

    if(!(VertexType & OBJ_NORMALS)) {
        // cerr << "GenNormals: " << Name << endl;
        // FixFacing();

        GenFaceNormals();
        GenNormalsFromFaceNormals();
    }

#ifdef HEAVY_DEBUG
    // XXX Debug code.
    // Sanity check the normals.
    for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next) {
        if(CheckVec(V->Nor)) {
            cerr << "GenNormals: BadNor: " << Name << V->Nor << endl;
        }
    }
#endif
}

// It is ok to have texcoords without a texture defined.
void Mesh::GenTexCoords()
{
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");

    // If texcoords already exist nothing is changed.
    if(!(VertexType & OBJ_TEXCOORDS)) {
        // cerr << "GenTexCoords\n";

        // Select which dimensions become the s and t coords.
        RebuildBBox();

        Matrix44 perm;
        perm.IsIdentity() = false;
        perm.InverseValid() = false;
        perm(0, 0) = 0;
        perm(1, 1) = 0;
        Vector Span = Box.MaxV - Box.MinV;
        if(Span.x >= Span.y && Span.x >= Span.z) {
            perm(0, 0) = 1;
            if(Span.y >= Span.z)
                perm(1, 1) = 1;
            else
                perm(1, 2) = 1;
        } else if(Span.y >= Span.z) {
            perm(0, 1) = 1;
            if(Span.x >= Span.z)
                perm(1, 0) = 1;
            else
                perm(1, 2) = 1;
        } else {
            perm(0, 2) = 1;
            if(Span.x >= Span.y)
                perm(1, 0) = 1;
            else
                perm(1, 1) = 1;
        }

        Matrix44 TmpTex = TexTransform;

        TmpTex *= perm;

        TmpTex.Scale(1. / Box.MaxDim());
        TmpTex.Translate(-Box.MinV);

        // Transform all the vertices by the matrix to gen. texcoords.
        for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next) {
            V->Tex = TmpTex * V->V;
        }
    }

    // XXX Debug code.
    // Sanity check the texcoords.
    for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next) {
        if(Abs(V->Tex.x) > 1e5 || Abs(V->Tex.y) > 1e5 || !Finite(V->Tex.x) || !Finite(V->Tex.y))
            cerr << "GenTex: BadTex: " << Name << V->Tex << endl;
    }

    VertexType = VertexType | OBJ_TEXCOORDS;
}

// Sometimes we get a bad tangent from all the vertices having the same U texcoord.
DMC_INLINE Vector ComputeFaceTangent(const Vector &P1, const Vector &P2, const Vector &P3,
                         const Vector &T1, const Vector &T2, const Vector &T3, const Vector &N)
{
    Vector Vec1 = P3 - P2;
    Vector Vec2 = P1 - P2;
    double dU1 = T3.x - T2.x;
    double dU2 = T1.x - T2.x;
    Vector V = Vec2 * dU1 - Vec1 * dU2; // Opposite of the article.
    V.normalize();
    Vector U = Cross(V, N);
    U.normalize();
    return U;
}

void Mesh::GenFaceTangents()
{
    ASSERT_RM(FaceType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");
    ASSERT_RM(VertexType & OBJ_TEXCOORDS, "Vertices must have texcoords to gen tangents.");

    // Need face normals first.
    if(!(FaceType & OBJ_NORMALS)) {
        GenFaceNormals();
    }

    // Generate the facet Tangents.
    for(AFace *F = (AFace *)Faces; F; F = (AFace *)F->next) {
        F->Tan = ComputeFaceTangent(F->v0->V, F->v1->V, F->v2->V,
            ((AVertex *)F->v0)->Tex, ((AVertex *)F->v1)->Tex, ((AVertex *)F->v2)->Tex, F->Nor);
        //if(CheckVec(F->Tan)) cerr << "Gen: BadFTan: " << Name << F->Tan << endl;
    }

    FaceType = FaceType | OBJ_TANGENTS;
}

// Sometimes we get good face tangents but when we add them together to get a vertex tangent
// they average to zero.
void Mesh::GenTangentsFromFaceTangents()
{
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");

    // Accumulate the facet Tangents into vertex Tangents.
    for(AVertex *V = (AVertex *)Verts; V; V = (AVertex *)V->next) {
        // cerr << "v";
        ASSERT_R(V->Faces.size());

        Vector AccTan(((AFace *)(V->Faces[0]))->Tan);
        // Loop on all the faces of this vertex.
        for(int k=1; k<(int)V->Faces.size(); k++) {
            AccTan += ((AFace *)(V->Faces[k]))->Tan;
        }

        // Now store the Tangent.
        AccTan.normalize();
        V->Tan = AccTan;
        if(CheckVec(V->Tan)) {
            // cerr << "Gen: BadVTan: " << Name << V->Tan << endl;
            // V->Tan = Vector(0,0,0);
        }
    }

    VertexType = VertexType | OBJ_TANGENTS;
}

// For meshes with attributes, generates vertex Tangents based on the
// geometry of the mesh.
//
// You need to make sure the facing is consistent before calling this.
// If Tangents already exist nothing is changed.
void Mesh::GenTangents()
{
    ASSERT_RM(VertexType & HAS_ATTRIBS, "Mesh must have attributes.");

    if(VertexType & OBJ_TANGENTS)
        // If Tangents already exist nothing is changed.
        return;

    // cerr << "GenTangents: " << Name << endl;
    // FixFacing();

    GenFaceTangents();
    GenTangentsFromFaceTangents();
}

void Mesh::RebuildBBox()
{
    Box.Reset();

    for(Vertex *V = Verts; V; V = V->next)
        Box += V->V;
}

void Mesh::ApplyTransform(Matrix44 &Mat)
{
    // XXX If this object is part of a model, the model bbox should also be recomputed.
    Box.Reset();

    for(Vertex *V = Verts; V; V = V->next) {
        V->V = Mat * V->V;
        Box += V->V;
    }

    // XXX Need to transform the normals and tangents.
    ASSERT_R(!(VertexType & (OBJ_NORMALS | OBJ_TANGENTS)));
}

void Mesh::ApplyTextureTransform(Matrix44 &Mat)
{
    ASSERT_R(VertexType & OBJ_TEXCOORDS);

    for(Vertex *V = Verts; V; V = V->next) {
        ((AVertex *)V)->Tex = Mat * ((AVertex *)V)->Tex;
    }
}

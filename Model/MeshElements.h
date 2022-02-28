//////////////////////////////////////////////////////////////////////
// MeshElements.h - The geometric elements of a Mesh - Vertex, Edge, Face
//
// Copyright David K. McAllister, Aug. 1999.
//
// This represents arbitrary non-manifold meshes of vertices, edges, and faces.

#pragma once

#include "Math/Vector.h"

#include <vector>

struct Edge;
struct Face;

struct Vertex {
    f3vec V;

    Vertex *next, *prev;

    std::vector<Edge*> Edges;
    std::vector<Face*> Faces;

    DMC_DECL Vertex() { next = prev = NULL; }

    DMC_DECL ~Vertex()
    {
        // WARNING: If this is the list head, you need to fix it yourself!
        if (next) next->prev = prev;
        if (prev) prev->next = next;

#ifdef DMC_MESH_DEBUG
        next = prev = NULL;
        // INFO("~Vertex");
#endif
    }

    // Remove myself from this list of vertices.
    DMC_DECL void ListRemove(std::vector<Vertex*>& Ll)
    {
        for (unsigned int i = 0; i < Ll.size();)
            if (this == Ll[i]) {
                Ll[i] = Ll.back();
                Ll.pop_back();
            } else
                i++;
    }
};

struct Edge {
    Edge *next, *prev; // For the linked list of all edges.

    std::vector<Face*> Faces; // Should be 1 or 2 if manifold.
    Vertex *v0, *v1;

    DMC_DECL ~Edge()
    {
        // WARNING: If this is the list head, you need to fix it yourself!
        if (next) next->prev = prev;
        if (prev) prev->next = next;

#ifdef DMC_MESH_DEBUG
        v0 = NULL;
        next = prev = NULL;
        // INFO("~Edge");
#endif
    }

    // Remove myself from this list of edges.
    DMC_DECL void ListRemove(std::vector<Edge*>& Ll)
    {
        for (int i = 0; i < (int)Ll.size();)
            if (this == Ll[i]) {
                Ll[i] = Ll.back();
                Ll.pop_back();
            } else
                i++;
    }
};

struct Face {
    Face *next, *prev;

    Vertex *v0, *v1, *v2;
    Edge *e0, *e1, *e2;

    DMC_DECL ~Face()
    {
        // WARNING: If this is the list head, you need to fix it yourself!
        if (next) next->prev = prev;
        if (prev) prev->next = next;

#ifdef DMC_MESH_DEBUG
        v0 = NULL;
        next = prev = NULL;
        // INFO("~Face\n");
#endif
    }

    // Remove myself from this list of faces.
    DMC_DECL void ListRemove(std::vector<Face*>& Ll)
    {
        for (int i = 0; i < (int)Ll.size();)
            if (this == Ll[i]) {
                Ll[i] = Ll.back();
                Ll.pop_back();
            } else
                i++;
    }
};

// Factories for making a new one of these.
extern Vertex* VertexFactory();
extern Edge* EdgeFactory();
extern Face* FaceFactory();

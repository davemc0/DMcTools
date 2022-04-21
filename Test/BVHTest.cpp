// Test BVH builders
//
// Copyright 2022 by David K. McAllister

#include "Math/Triangle.h"
#include "Model/LBVH.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "deps/tiny_obj_loader.h"

#include <string>
#include <vector>

void loadModel(std::vector<Triangle>& tris, const std::string& fname)
{
    std::cerr << "Loading " << fname << std::endl;

    tinyobj::ObjReaderConfig cfg;
    cfg.vertex_color = false;
    tinyobj::ObjReader reader;

    reader.ParseFromFile(fname, cfg);
    if (!reader.Valid()) {
        std::cerr << "OBJ load not valid: WARN: " << reader.Warning() << "ERR: " << reader.Error() << '\n';
        return;
    }

    auto& shapes = reader.GetShapes();
    auto& vertices = reader.GetAttrib().vertices;

    for (auto& s : shapes) {
        int i = 0;
        for (auto& m : s.mesh.num_face_vertices) {
            f3vec v0(&vertices[s.mesh.indices[i++].vertex_index * 3]);
            f3vec v1(&vertices[s.mesh.indices[i++].vertex_index * 3]);
            f3vec v2(&vertices[s.mesh.indices[i++].vertex_index * 3]);
            Triangle t(v0, v1, v2);
            tris.emplace_back(t);
        }
    }
}

bool BVHTest(int argc, char** argv)
{
    std::vector<Triangle> tris;

    loadModel(tris, "../../Models/Bunny/bunny.obj");

    std::cerr << tris.size() << " triangles\n";

    LBVH builder(tris);

    builder.build();

    return true;
}

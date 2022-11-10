// Test BVH builders
//
// Copyright 2022 by David K. McAllister

#include "Image/tImage.h"
#include "Math/Triangle.h"
#include "Model/LBVH.h"
#include "Model/Traverser.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "deps/tiny_obj_loader.h"

#include <algorithm>
#include <execution>
#include <ranges>
#include <string>
#include <vector>

#define EXPOL std::execution::par_unseq

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
            const float K = 1.f; // XXX TODO
            Triangle t(v0 * K, v1 * K, v2 * K);
            tris.emplace_back(t);

            // XXX TODO
            // if (tris.size() >=1000000) return;
        }
    }
}

void renderImg(BVH& Bvh, f3vec& eye, f3vec lookdir, f3vec up, int width, int height, float vfovDeg)
{
    up.normalize();
    lookdir.normalize();
    f3vec right = cross(lookdir, up);

    uc3Image img(width, height, uc3Pixel(0));
    float pixLen = 2.f * tan(degToRad(vfovDeg * 0.5f)) / (float)height;
    f3vec rightPix = right * pixLen, downPix = up * -pixLen;
    f3vec upLeftCorner = lookdir - downPix * height * 0.5f - rightPix * width * 0.5f;

    Traverser Trav(Bvh);

    f3vec Org = eye;

    std::ranges::iota_view indices(0, width * height);
    std::for_each(EXPOL, indices.begin(), indices.end(), [&](int i) {
        int x = i % width, y = i / width;
        // Ray trace a pixel here
        f3vec Dir = upLeftCorner + rightPix * x + downPix * y;

        auto [isHit, tHit, primId] = Trav.traceRay<f3vec, float>(Org, Dir, 0.f, DMC_MAXFLOAT);

        img(x, y) = isHit ? uc3Pixel(255, 255, 255) : uc3Pixel(0, 0, 0);
    });

    img.Save("raytraced.png");
}

bool BVHTest(int argc, char** argv)
{
    std::vector<Triangle> tris;

    loadModel(tris, "../../Models/Bunny/bunny.obj");

    std::cerr << tris.size() << " triangles\n";

    BuildParams buildParams;

    LBVH builder(tris, buildParams);

    builder.build();

    f3vec eye(0.f, 0.f, 3.f), lookat(builder.getWorldBox().centroid()), up(0, 1, 0);
    f3vec lookdir = (lookat - eye).normalized();
    f3vec right = cross(lookdir, up);
    up = cross(right, lookdir);

    renderImg(builder, eye, lookdir, up, 2000, 1500, 45.f);

    return true;
}

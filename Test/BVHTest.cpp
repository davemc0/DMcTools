// Test BVH builders
//
// Copyright 2022 by David K. McAllister

#include "Image/tImage.h"
#include "Math/Triangle.h"
#include "Math/VectorUtils.h"
#include "Model/LBVH.h"
#include "Model/Traverser.h"
#include "Util/Counters.h"

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
            Triangle t(v0, v1, v2);
            tris.emplace_back(t);
        }
    }
}

uc3Pixel shadeHitRandomDiffuse(const Triangle& tri, int primId, int pass)
{
    f3vec lightDir(1.f, 1.f, 1.f);
    lightDir.normalize();

    f3vec nrm = tri.normal();
    float Diffuse = clamp(dot(nrm, lightDir), 0.f, 1.f);

    static int colorCycle = 0;
    if (colorCycle == 0) irand();

    SRand(primId);
    int randVal = irand() ^ colorCycle;
    f3Pixel baseColor = *(uc3Pixel*)&randVal;

    return baseColor * Diffuse;
}

uc3Pixel shadeHitRandom(const Triangle& tri, int primId)
{
    static int colorCycle = 0;
    if (colorCycle == 0) irand();

    SRand(primId);
    int randVal = irand() ^ colorCycle;

    return *(uc3Pixel*)&randVal;
}

void renderImg(uc3Image& img, const BVH& Bvh, const std::vector<Triangle>& tris, f3vec& eye, f3vec lookdir, f3vec up, float vfovDeg, int pass = 0)
{
    int width = img.w(), height = img.h();

    up.normalize();
    lookdir.normalize();
    f3vec right = cross(lookdir, up);

    float pixLen = 2.f * tan(degToRad(vfovDeg * 0.5f)) / (float)height;
    f3vec rightPix = right * pixLen, downPix = up * -pixLen;
    f3vec upLeftCorner = lookdir - downPix * height * 0.5f - rightPix * width * 0.5f;
    f3vec Org = eye;

    Traverser Trav(Bvh);

    std::ranges::iota_view indices(0, width * height);
    std::for_each(EXPOL, indices.begin(), indices.end(), [&](int i) {
        int x = i % width, y = i / width;
        // Ray trace a pixel here
        f3vec Dir = upLeftCorner + rightPix * x + downPix * y;

        auto [isHit, tHit, primId] = Trav.traceRay<f3vec, float>(Org, Dir, 0.f, DMC_MAXFLOAT);

        img(x, y) = uc3Pixel(0, 0, 0);
        if (isHit) img(x, y) = shadeHitRandomDiffuse(tris[primId], primId, pass);
    });
}

bool renderModel(int argc, char** argv)
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

    uc3Image img(3200, 2400, uc3Pixel(0));
    renderImg(img, builder, tris, eye, lookdir, up, 45.f);
    img.Save("ray_traced_bunny.png");

    return true;
}

bool BVHTest(int argc, char** argv)
{
    renderModel(argc, argv);

    std::cerr << "Here\n";
    CPRINT();

    return true;
}

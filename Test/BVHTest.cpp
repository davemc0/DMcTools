// Test BVH builders
//
// Copyright 2022 by David K. McAllister

#include "Image/tImage.h"
#include "Math/Triangle.h"
#include "Math/VectorUtils.h"
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

void makeBLP(std::vector<Triangle>& tris2, std::vector<Triangle>& tris2p, std::vector<Triangle>& trisBLP)
{
    f3vec verts[4];
    for (int i = 0; i < 4; i++) { verts[i] = makeRand<f3vec>() * 1.5f; }

    tris2.emplace_back(verts[0], verts[1], verts[2]); // Red
    tris2.emplace_back(verts[1], verts[2], verts[3]); // Red

    tris2p.emplace_back(verts[0], verts[1], verts[3]); // Blue
    tris2p.emplace_back(verts[0], verts[2], verts[3]); // Blue

    // Insert bilinear patch approximation triangles
    const int NSLABS = 128;
    for (int slab = 0; slab < NSLABS; slab++) {
        float ta = slab / (float)NSLABS;
        float tb = (slab + 1) / (float)NSLABS;

        f3vec sa0 = linInterp(verts[0], verts[1], ta);
        f3vec sb0 = linInterp(verts[0], verts[1], tb);
        f3vec sa1 = linInterp(verts[2], verts[3], ta);
        f3vec sb1 = linInterp(verts[2], verts[3], tb);

        // Two triangles for a horizontal slice of a bilinear patch
        trisBLP.emplace_back(sa0, sb0, sa1);
        trisBLP.emplace_back(sa1, sb0, sb1);
    }
}

uc3Pixel shadeHitBLP(const Triangle& tri, int primId, int pass)
{
    f3vec lightDir(1.f, 1.f, 1.f);
    lightDir.normalize();

    f3vec nrm = tri.normal();
    float lamb = abs(dot(nrm, lightDir));

    f3Pixel baseColor;

    if (pass == 0)
        baseColor = {1.f, primId == 0 ? 0.f : 1.f, lamb};
    else if (pass == 1)
        baseColor = {lamb, primId == 0 ? 0.f : 1.f, 1.f};
    else {
        int randVal = (primId << 19) | 0xff00;
        uc3Pixel randCol(*(uc3Pixel*)&randVal);
        baseColor = randCol;
        baseColor.r() = lamb;
    }

    return baseColor;
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

        // If (drawMisses)
        img(x, y) = uc3Pixel(0, 0, 0);
        if (isHit) {
            // img(x, y) = shadeHitRandom(tris[primId], primId);
            img(x, y) = shadeHitBLP(tris[primId], primId, pass);
        }
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

bool renderBLP(int argc, char** argv)
{
    std::vector<Triangle> tris2, tris2p, trisBLP;
    makeBLP(tris2, tris2p, trisBLP);

    BuildParams buildParams;
    LBVH bvh2(tris2, buildParams);
    bvh2.build();
    LBVH bvh2p(tris2p, buildParams);
    bvh2p.build();
    LBVH bvhBLP(trisBLP, buildParams);
    bvhBLP.build();

    f3vec eye(0.f, 0.f, 3.f), lookat(bvhBLP.getWorldBox().centroid()), up(0, 1, 0);
    f3vec lookdir = (lookat - eye).normalized();
    f3vec right = cross(lookdir, up);
    up = cross(right, lookdir);

    const int w = 800, h = 600;
    uc3Image img1(w,h, uc3Pixel(0));
    uc3Image img2(w,h, uc3Pixel(0));
    uc3Image img3(w,h, uc3Pixel(0));
    renderImg(img1, bvh2, tris2, eye, lookdir, up, 45.f, 0);
    renderImg(img2, bvhBLP, trisBLP, eye, lookdir, up, 45.f, 2);
    renderImg(img3, bvh2p, tris2p, eye, lookdir, up, 45.f, 1);

    static int attempt = 0;
    int failPix = 0;

    bool match1 = true;
    for (int i = 0; i < img1.w() * img1.h(); i++) {
        if (match1 && img2[i] != uc3Pixel(0) && img1[i] == uc3Pixel(0)) match1 = false; // First pair of tris failed to cover it.
        if ((!match1) && img2[i] != uc3Pixel(0) && img3[i] == uc3Pixel(0)) {            // Second pair of tris failed to cover it.
            failPix++;
            // std::cerr << "attempt: " << attempt << " " << i % img1.w() << "," << i / img1.w() << img1[i] << img2[i] << img3[i] << '\n';
        }
    }

    if (failPix) {
        std::cerr << "attempt: " << attempt << " " << failPix << '\n';
        auto numstr = std::to_string(attempt);
        img1.Save("ray_tris_" + numstr + "_1.png");
        img2.Save("ray_tris_" + numstr + "_2.png");
        img3.Save("ray_tris_" + numstr + "_3.png");
    }
    failPix = 0;
    attempt++;

    return true;
}

bool BVHTest(int argc, char** argv)
{
    // renderModel(argc, argv);

    SRand();
    while (1) renderBLP(argc, argv);

    return true;
}

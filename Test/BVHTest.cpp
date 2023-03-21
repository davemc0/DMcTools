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

// #define EXPOL std::execution::seq
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

f3vec toLight(0.f, -3.f, 2.f);

// Return 0.0 -> 1.0
f2vec f3vecToLatLong(f3vec vec)
{
    vec.normalize();
    float rad = sqrtf(vec.x * vec.x + vec.y * vec.y);
    float phi = -vec.z * 0.5f + 0.5f;
    float theta = atan2(vec.y, vec.x);

    f2vec out(0.5f + theta / (2.f * M_PI), phi);

    ASSERT_D(out.x >= 0.f || out.x <= 1.f);
    ASSERT_D(out.y >= 0.f || out.y <= 1.f);

    return out;
}

auto baryInterp(const auto& v0, const auto& v1, const auto& v2, const float bary0, const float bary1)
{
    return v0 * (1.f - bary0 - bary1) + v1 * bary0 + v2 * bary1;
}

f3Pixel tof3Pixel(const f3vec& v) { return {v.x, v.y, v.z}; }

uc3Pixel shadeHitGouraudDiffuse(const f3vec& eye, const Triangle& tri, float bary0, float bary1, bool isFrontFacing, int primId)
{
    toLight.normalize();

    f3vec nrm = tri.normal();
    float diffuseFac = clamp(dot(nrm, toLight), 0.f, 1.f);

    static int colorCycle = 0;
    if (colorCycle == 0) irand();

    f3Pixel baseColor(bary0, bary1, 1.f - bary0 - bary1);

    return uc3Pixel(baseColor * diffuseFac);
}

f3Pixel shadeOnePoint(const f3vec& eye, const f3vec& vrt, const uc3Image& map)
{
    f3vec ptn = vrt.normalized();

    f2vec latLong = f3vecToLatLong(ptn) * f2vec(map.w(), map.h());

    f3Pixel baseColor = map(latLong.x, latLong.y);

    f3vec nrm = ptn.normalized();
    toLight.normalize();
    float diffuseFac = clamp(dot(nrm, toLight), 0.2f, 1.f);

    f3vec toEye = (eye - vrt).normalized();
    f3vec refl = reflect(toLight, nrm);
    float specFac = powf(clamp(dot(toEye, refl), 0.f, 1.f), 10.f);

    return f3Pixel(baseColor * diffuseFac + f3Pixel(1.f) * specFac);
}

uc3Pixel shadeHitTexGouraud(const f3vec& eye, const Triangle& tri, float bary0, float bary1, bool isFrontFacing, int primId)
{
    // static uc3Image map("lores.png");
    static uc3Image map("C:/Users/davemc/GoogleDrive/PicturesG/Maps/1_earth_8k.jpg");

    f3vec tov0 = tri.v[0].normalized();
    f3vec tov1 = tri.v[1].normalized();
    f3vec tov2 = tri.v[2].normalized();

    f3vec shadePoint = baryInterp(tov0, tov1, tov2, 0.5f, 0.25f);

    f3Pixel c0 = shadeOnePoint(eye, shadePoint, map);
    f3Pixel c1 = shadeOnePoint(eye, shadePoint, map);
    f3Pixel c2 = shadeOnePoint(eye, shadePoint, map);

    f3Pixel color = baryInterp(c0, c1, c2, bary0, bary1);

    return uc3Pixel(color);
}

uc3Pixel shadeHitTexPhong(const f3vec& eye, const Triangle& tri, float bary0, float bary1, bool isFrontFacing, int primId)
{
    static uc3Image map("C:/Users/davemc/GoogleDrive/PicturesG/Maps/1_earth_8k.jpg");

    toLight.normalize();

    f3vec tov0 = tri.v[0].normalized();
    f3vec tov1 = tri.v[1].normalized();
    f3vec tov2 = tri.v[2].normalized();

    f3vec shadePoint = baryInterp(tov0, tov1, tov2, bary0, bary1);

    f2vec latLong = f3vecToLatLong(shadePoint) * f2vec(map.w(), map.h());

    f3Pixel baseColor = map(latLong.x, latLong.y);

    f3vec nrm = shadePoint.normalized();
    float diffuseFac = clamp(dot(nrm, toLight), 0.f, 1.f);

    f3vec toEye = (eye - shadePoint).normalized();
    f3vec refl = reflect(toLight, nrm);
    float specFac = powf(clamp(dot(toEye, refl), 0.f, 1.f), 30.f);

    return uc3Pixel(baseColor * diffuseFac + f3Pixel(1.f) * specFac);
}

uc3Pixel shadeHitRandomDiffuse(const f3vec& eye, const Triangle& tri, float bary0, float bary1, bool isFrontFacing, int primId)
{
    toLight.normalize();

    f3vec nrm = tri.normal();
    float diffuseFac = clamp(dot(nrm, toLight), 0.f, 1.f);

    static int colorCycle = 0;
    if (colorCycle == 0) irand();

    SRand(primId);
    int randVal = irand() ^ colorCycle;
    f3Pixel baseColor = *(uc3Pixel*)&randVal;

    return baseColor * diffuseFac;
}

uc3Pixel shadeHitRandom(const f3vec& eye, const Triangle& tri, float bary0, float bary1, bool isFrontFacing, int primId)
{
    static int colorCycle = 0;
    if (colorCycle == 0) irand();

    SRand(primId);
    int randVal = irand() ^ colorCycle;

    return *(uc3Pixel*)&randVal;
}

void renderImg(uc3Image& img, const BVH& Bvh, const std::vector<Triangle>& tris, f3vec& eye, f3vec lookdir, f3vec up, float vfovDeg)
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
        auto [isHit, tHit, bary0, bary1, isFrontFacing, primId] = Trav.traceRay<f3vec, float>(Org, Dir, 0.f, DMC_MAXFLOAT);

        img(x, y) = uc3Pixel(0, 0, 0);
        if (isHit) img(x, y) = shadeHitTexPhong(eye, tris[primId], bary0, bary1, isFrontFacing, primId);
    });
}

bool renderGouraudTex(int argc, char** argv)
{
    std::vector<Triangle> tris;

    loadModel(tris, "../../Models/Small/sphere.obj");

    std::cerr << tris.size() << " triangles\n";

    BuildParams buildParams;

    LBVH builder(tris, buildParams);

    builder.build();

    f3vec eye(0.f, -8.f, 0.f), lookat(0, 0, 0), up(0, 0, 1);
    f3vec lookdir = (lookat - eye).normalized();
    f3vec right = cross(lookdir, up);
    up = cross(right, lookdir);

    uc3Image img(3200, 3200, uc3Pixel(0));
    renderImg(img, builder, tris, eye, lookdir, up, 45.f);
    img.Save("phong_tex_sphere.png");

    return true;
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
    renderGouraudTex(argc, argv);
    // renderModel(argc, argv);

    std::cerr << "Here\n";
    CPRINT();

    return true;
}

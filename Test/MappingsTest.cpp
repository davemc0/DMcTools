// Test the DMcTools Mappings class for mapping a sphere to 2D

#include "Math/Mappings.h"

#include "Image/tImage.h"
#include "Math/Random.h"
#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "Util/Timer.h"

#include <cmath>
#include <iostream>

enum { E_PAR = 0, E_OCT, E_PYR, E_CUB, E_HEM, E_LAST };

std::string mappingName(int chosen_mapping)
{
    switch (chosen_mapping) {
    case E_PAR: return "DualParaboloid";
    case E_OCT: return "Octahedron";
    case E_PYR: return "DualPyramid";
    case E_CUB: return "Cube";
    case E_HEM: return "Hemisphere";
    default: return "UNKNOWN!!!";
    };
}

int mappingFaces(int chosen_mapping)
{
    switch (chosen_mapping) {
    case E_PAR: return 2;
    case E_OCT: return 1;
    case E_PYR: return 2;
    case E_CUB: return 6;
    case E_HEM: return 2;
    default: return -1;
    };
}

void SimpleTest()
{
    std::cerr << HemisphereTo2DNN(f3vec(1, 0, 0)) << '\n';
    std::cerr << HemisphereTo2DNN(f3vec(0, 1, 0)) << '\n';
    std::cerr << HemisphereTo2DNN(f3vec(0, 0, 1)) << '\n';
    std::cerr << HemisphereTo2DNN(f3vec(-1, 0, 0)) << '\n';
    std::cerr << HemisphereTo2DNN(f3vec(0, -1, 0)) << '\n';
    std::cerr << HemisphereTo2DNN(f3vec(0, 0, -1)) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(1, 0, 0))) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(0, 1, 0))) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(0, 0, 1))) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(-1, 0, 0))) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(0, -1, 0))) << '\n';
    std::cerr << HemisphereTo3DNN(HemisphereTo2DNN(f3vec(0, 0, -1))) << '\n';
}

void RenderWithMapping(int chosen_mapping)
{
    ASSERT_RM(chosen_mapping >= 0, "Must specify a mapping number on the command line.");
    const int IMHGT = 512;
    const int IMWID = IMHGT * mappingFaces(chosen_mapping);

    uc3Image Im(IMWID, IMHGT, 0);
    for (float phi = 0; phi <= M_PI; phi += 0.04f) {
        int uc = 0;
        for (float rho = 0; rho < 2 * M_PI; rho += 0.01f) {
            f3vec V3(cos(rho) * sin(phi), sin(rho) * sin(phi), cos(phi));
            f3vec V2;
            switch (chosen_mapping) {
            case E_PAR: V2 = ParaboloidTo2D(V3); break;
            case E_OCT: V2 = OctahedronTo2D(V3); break;
            case E_PYR: V2 = PyramidTo2D(V3); break;
            case E_CUB: V2 = CubeTo2D(V3); break;
            case E_HEM: V2 = HemisphereTo2D(V3); break;
            };

            V2.clamp(f3vec(-1.f), f3vec(1.f));
            // if (V2.x < 0) std::cerr << V2 << '\n';
            int x, y;
            UVToXY(x, y, V2, IMHGT);

            if (x < 0 || y < 0 || x >= IMWID || y >= IMHGT) { std::cerr << x << "," << y << V2 << '\n'; }

            uc++;
            Im(x, y) = uc3Pixel(uc % 255, V3.z < 0 ? 255 : 70, uc / 25);
        }
    }

    Im.Save("Out" + mappingName(chosen_mapping) + ".png");
}

void RenderFacesWithMapping(int chosen_mapping)
{
    ASSERT_RM(chosen_mapping >= 0, "Must specify a mapping number on the command line.");
    const int IMHGT = 512;
    const int IMWID = IMHGT * mappingFaces(chosen_mapping);

    uc3Image Im(IMWID, IMHGT, 0);
    for (int y = 0; y < Im.h(); y++)
        for (int x = 0; x < Im.w(); x++) {
            f3vec V2, V3;
            XYToUV(V2, x, y, IMHGT);
            switch (chosen_mapping) {
            case E_PAR: V3 = ParaboloidTo3D(V2); break;
            case E_OCT: V3 = OctahedronTo3D(V2); break;
            case E_PYR: V3 = PyramidTo3D(V2); break;
            case E_CUB: V3 = CubeTo3D(V2); break;
            case E_HEM: V3 = HemisphereTo3D(V2); break;
            };

            int d = abs(V3).maxDim();
            bool n = V3[d] < 0;
            f3Pixel c(0);
            c[d] = n ? 0 : 1;
            c[(d + 1) % 3] = n ? 1 : 0;
            c[(d + 2) % 3] = n ? 1 : 0;
            // Im(x, y) = f3Pixel(V3.x, V3.y, V3.z);
            Im(x, y) = c;
        }

    Im.Save("Face" + mappingName(chosen_mapping) + ".png");
}

bool Map2DTest()
{
    const int IMSIZE = 512;
    while (1) {
        {
            int x = LRand(0, IMSIZE);
            int y = LRand(0, IMSIZE);

            float u, v;

            XYToUV(u, v, x, y, IMSIZE);

            int x2, y2;
            UVToXY(x2, y2, u, v, IMSIZE);
            if (x != x2 || y != y2) { std::cerr << x << "," << y << " != " << x2 << "," << y2 << " " << u << "," << v << '\n'; }

            float u2, v2;
            XYToUV(u2, v2, x2, y2, IMSIZE);
            if (u != u2 || v != v2) { std::cerr << x2 << "," << y2 << " " << u << "," << v << " != " << u2 << "," << u2 << '\n'; }
        }

        {
            int x = LRand(0, IMSIZE * 6);
            int y = LRand(0, IMSIZE);

            // Test them again with the image offset in Z
            f3vec V2;
            XYToUV(V2, x, y, IMSIZE);

            int x2, y2;
            UVToXY(x2, y2, V2, IMSIZE);
            if (x != x2 || y != y2) { std::cerr << x << "," << y << " != " << x2 << "," << y2 << " " << V2 << '\n'; }

            f3vec V22;
            XYToUV(V22, x2, y2, IMSIZE);
            if (V2 != V22) { std::cerr << x2 << "," << y2 << " " << V2 << " != " << V22 << '\n'; }
        }
    }
}

void MappingsPerf(int chosen_mapping)
{
    ASSERT_RM(chosen_mapping >= 0, "Must specify a mapping number on the command line.");
    SRand();

    std::cerr << "Starting timer.\n";
    const int N = 1000000000;
    Timer T;

    f3vec V3 = MakeRandOnSphere<f3vec>();
    V3.z = -0.5f;
    f3vec Acc(0, 0, 0);

    T.Reset();
    for (int i = 0; i < N; i++) {
        f3vec V2;
        switch (chosen_mapping) {
        case E_PAR: V2 = ParaboloidTo2DNN(V3); break;
        case E_OCT: V2 = OctahedronTo2DNN(V3); break;
        case E_PYR: V2 = PyramidTo2DNN(V3); break;
        case E_CUB: V2 = CubeTo2DNN(V3); break;
        case E_HEM: V2 = HemisphereTo2DNN(V3); break;
        }
        Acc += V2;
    }
    std::cerr << T.Read() << '\n';

    T.Reset();
    for (int i = 0; i < N; i++) {
        f3vec V2;
        switch (chosen_mapping) {
        case E_PAR: V2 = ParaboloidTo2D(V3); break;
        case E_OCT: V2 = OctahedronTo2D(V3); break;
        case E_PYR: V2 = PyramidTo2D(V3); break;
        case E_CUB: V2 = CubeTo2D(V3); break;
        case E_HEM: V2 = HemisphereTo2D(V3); break;
        }
        Acc += V2;
    }
    std::cerr << T.Read() << '\n';

    T.Reset();
    for (int i = 0; i < N; i++) {
        f3vec V2;
        switch (chosen_mapping) {
        case E_PAR: V2 = ParaboloidTo3DNN(V3); break;
        case E_OCT: V2 = OctahedronTo3DNN(V3); break;
        case E_PYR: V2 = PyramidTo3DNN(V3); break;
        case E_CUB: V2 = CubeTo3DNN(V3); break;
        case E_HEM: V2 = HemisphereTo3DNN(V3); break;
        }
        Acc += V2;
    }
    std::cerr << T.Read() << '\n';

    T.Reset();
    for (int i = 0; i < N; i++) {
        f3vec V2;
        switch (chosen_mapping) {
        case E_PAR: V2 = ParaboloidTo3D(V3); break;
        case E_OCT: V2 = OctahedronTo3D(V3); break;
        case E_PYR: V2 = PyramidTo3D(V3); break;
        case E_CUB: V2 = CubeTo3D(V3); break;
        case E_HEM: V2 = HemisphereTo3D(V3); break;
        }
        Acc += V2;
    }
    std::cerr << T.Read() << '\n';

    std::cerr << Acc << '\n';
}

void PrintTest(int chosen_mapping)
{
    int tries = 0, fails = 0;
    while (1) {
        f3vec V2, V3p, V3 = MakeRandOnSphere<f3vec>();

        V3 *= DRandf(0.00001f, 10.0f); // For testing the NN functions

        switch (chosen_mapping) {
        case E_PAR:
            V2 = ParaboloidTo2DNN(V3);
            V3p = ParaboloidTo3D(V2);
            break;
        case E_OCT:
            V2 = OctahedronTo2DNN(V3);
            V3p = OctahedronTo3D(V2);
            break;
        case E_PYR:
            V2 = PyramidTo2DNN(V3);
            V3p = PyramidTo3D(V2);
            break;
        case E_CUB:
            V2 = CubeTo2DNN(V3);
            V3p = CubeTo3D(V2);
            break;
        case E_HEM:
            V2 = HemisphereTo2DNN(V3);
            V3p = HemisphereTo3D(V2);
            break;
        };

        V3.normalize();

        const float EPS = 0.00000026f; // Par: 0.00000026f Oct: 0.00000045f Pyr: 0.00000026f

        tries++;
        if ((V3 - V3p).length() > EPS) {
            fails++;
            std::cerr << fails << "/" << tries << " " << mappingName(chosen_mapping) << V3 << V2 << V3p << '\n';
        }
    }
}

bool MappingsTest(int argc, char** argv)
{
    std::cerr << "Starting MappingsTest\n";

    SRand();

    SimpleTest();

    int st = 0, end = E_LAST;
    if (argc > 1) {
        st = atoi(argv[1]);
        end = st + 1;
    }

    for (int chosen_mapping = st; chosen_mapping < end; chosen_mapping++) {
        std::cerr << chosen_mapping << '\n';
        RenderFacesWithMapping(chosen_mapping);
        RenderWithMapping(chosen_mapping);
        // MappingsPerf(chosen_mapping);
        // PrintTest(chosen_mapping);
    }

    // Map2DTest();

    std::cerr << "Ending MappingsTest\n";

    return true;
}

// Test the DMcTools Mappings class for mapping a sphere to 2D

#include "Math/Mappings.h"

#include "Image/tImage.h"
#include "Math/Random.h"
#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "Util/Timer.h"

#include <cmath>
#include <iostream>

void RenderWithMapping(int chosen_mapping)
{
    ASSERT_RM(chosen_mapping >= 0, "Must specify a mapping number on the command line.");
    const int IMSIZE = 512;

    uc3Image Im(IMSIZE, IMSIZE, 0);
    for (float r = -1.0f; r <= 1.01f; r += 0.1f) {
        int uc = 0;
        for (float th = 0; th < 2 * M_PI; th += 0.001f) {
            f3vec V3(r * cos(th), r * sin(th), 0);
            float d = 1 - V3.x * V3.x - V3.y * V3.y;

            if (d > 0) V3.z = _copysign(sqrt(d), r);

            float t = V3.z;
            V3.z = V3.y;
            V3.y = t;

            f3vec V2;
            switch (chosen_mapping) {
            case 0: V2 = ParaboloidTo2D(V3); break;
            case 1: V2 = OctahedronTo2D(V3); break;
            case 2: V2 = PyramidTo2D(V3); break;
            };

            V2.x = dmcm::Clamp<float>(-1, V2.x, 1);
            V2.y = dmcm::Clamp<float>(-1, V2.y, 1);
            V2.z = dmcm::Clamp<float>(-1, V2.z, 1);

            int x, y;
            UVToXY(x, y, V2.x, V2.y, IMSIZE);

            if (y < 0) { V2 = ParaboloidTo2D(V3); }

            uc++;
            Im(x, y) = uc3Pixel(uc % 255, r * 250, uc / 25);
        }
    }

    Im.Save("out.png");

    exit(0);
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
    SRand();

    std::cerr << "Starting timer.\n";
    const int N = 10000000;
    Timer T;

    f3vec V3 = MakeRandOnSphere<f3vec>();
    V3.z = -0.5f;
    f3vec Acc(0, 0, 0);

    switch (chosen_mapping) {
    case 0:
        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = ParaboloidTo2DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = ParaboloidTo2D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = ParaboloidTo3DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = ParaboloidTo3D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';
        break;
    case 1:
        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = OctahedronTo2DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = OctahedronTo2D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = OctahedronTo3DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = OctahedronTo3D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';
        break;
    case 2:
        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = PyramidTo2DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = PyramidTo2D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = PyramidTo3DNN(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';

        T.Reset();
        T.Start();
        for (int i = 0; i < N; i++) {
            f3vec V2 = PyramidTo3D(V3);
            Acc += V2;
        }
        T.Stop();
        std::cerr << T.Read() << '\n';
        break;
    }

    std::cerr << Acc << '\n';
    exit(0);
}

bool MappingsTest(int argc, char** argv)
{
    std::cerr << "Starting MappingsTest\n";

    SRand();

    int chosen_mapping = -1;
    if (argc > 1) { chosen_mapping = atoi(argv[1]); }

    // Render(chosen_mapping);
    MappingsPerf(chosen_mapping);
    // Map2DTest();

    while (1) {
        f3vec V3 = MakeRandOnSphere<f3vec>();

        V3 *= DRandf(0.00001f, 10.0f); // For testing the NN functions

        f3vec V2, V3p;

        int mapping = chosen_mapping >= 0 ? chosen_mapping : (rand() % 3);
        switch (mapping) {
        case 0:
            std::cerr << "Par ";
            V2 = ParaboloidTo2DNN(V3);
            V3p = ParaboloidTo3D(V2);
            break;
        case 1:
            std::cerr << "Oct ";
            V2 = OctahedronTo2DNN(V3);
            V3p = OctahedronTo3D(V2);
            break;
        case 2:
            std::cerr << "Pyr ";
            V2 = PyramidTo2DNN(V3);
            V3p = PyramidTo3D(V2);
            break;
        };

        V3.normalize();

        if ((V3 - V3p).length() > 0.00001f) { std::cerr << V3 << V2 << V3p << '\n'; }
    }

    std::cerr << "Ending MappingsTest\n";

    return true;
}

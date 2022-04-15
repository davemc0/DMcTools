#include "Math/SpaceFillCurve.h"

#include "Math/AABB.h"
#include "Math/Random.h"
#include "Math/VectorUtils.h"
#include "Util/Timer.h"

#include <iostream>
#include <map>
#include <vector>

// For random pairs of a given distance in one space, what is the histogram of distances in the other space?

const int NBITS = 10;
const int DIM = 1 << NBITS;
const int LOOP_COUNT = 10000000;

template <typename intcode_t> void printName(const char* name, SFCurveType crvType)
{
    std::cerr << name << ": " << SFCurveNames[crvType];
    std::cerr << " sizeof(intcode_t) = " << sizeof(intcode_t) << " nbits = " << NBITS << '\n';
}

void testFixedPointifier()
{
    printName<uint32_t>("testFixedPointifier", CURVE_COUNT);

    Aabb world(f3vec(-100), f3vec(100));
    FixedPointifier<f3vec> FP(world, NBITS);
    i3vec i3;
    f3vec v3;

#define MAC(F)                              \
    {                                       \
        i3 = FP.floatToFixed(F);            \
        v3 = FP.fixedToFloat(i3);           \
        std::cerr << F << v3 << i3 << '\n'; \
    }

    MAC(f3vec(0, 0, 0));
    MAC(f3vec(-100));
    MAC(f3vec(100));
    MAC(f3vec(-200, 200, 0));
    MAC(f3vec(23.3));
    MAC(f3vec(23.4));

    // Find largest error for each value of nBits
    for (int nBits = 1; nBits <= 21; nBits++) {
        FixedPointifier<f3vec> F(world, nBits);
        i3vec i3;
        f3vec v3;
        float maxLen = 0;

        for (int i = 0; i < 30000000; i++) {
            f3vec f3 = makeRand(world);
            i3 = F.floatToFixed(f3);
            v3 = F.fixedToFloat(i3);
            float len = (f3 - v3).length();
            if (len > maxLen) {
                maxLen = len;
                std::cerr << nBits << f3 << v3 << i3 << maxLen << '\n';
            }
        }
    }

    std::cerr << "\n\n\n";
}

template <typename intcode_t> void testCodeToCoordToCodeAll(SFCurveType crvType)
{
    printName<intcode_t>("testCodeToCoordToCodeAll", crvType);

    intcode_t maxCode = (intcode_t)1 << (intcode_t)(curveOrder<intcode_t>() * 3);
    for (intcode_t code = 0; code < maxCode; code++) {
        i3vec coords = toSFCurveCoords(code, crvType);
        intcode_t code2 = toSFCurveCode<intcode_t>(coords, crvType);

        if (code != code2) {
            std::cerr << code << coords << code2 << '\n';
            throw DMcError("Mismatch");
        }
    }

    std::cerr << "\n\n\n";
}

template <typename intcode_t> void testCodeToCoordToCodeRand(SFCurveType crvType)
{
    printName<intcode_t>("testCodeToCoordToCodeRand", crvType);

    for (int i = 0; i < LOOP_COUNT; i++) {
        intcode_t code = (intcode_t)irand() | ((intcode_t)irand() << (intcode_t)31); // 62 random bits
        intcode_t maxCode = (intcode_t)1 << (intcode_t)(curveOrder<intcode_t>() * 3);
        code = code & (maxCode - 1);
        i3vec coords = toSFCurveCoords(code, crvType);
        intcode_t code2 = toSFCurveCode<intcode_t>(coords, crvType);

        std::cerr << i << ": " << code << coords << code2 << '\n';
        if (code != code2) {
            std::cerr << i << ": " << code << coords << code2 << '\n';
            throw DMcError("Mismatch");
        }
    }

    std::cerr << "\n\n\n";
}

// For random pairs of a given distance in one space, what is the histogram of distances in the other space?
template <typename intcode_t> void testCodeToCoordDistRand(SFCurveType crvType)
{
    printName<intcode_t>("testCodeToCoordDistRand", crvType);

    std::map<int, int> distanceHist; // How many mappings had each given distance
    SRand(0);                        // Give the same random numbers to each curve type

    float distTotal = 0.f;
    intcode_t numCodes = (intcode_t)1 << (intcode_t)(curveOrder<intcode_t>() * 3);
    for (int i = 0; i < LOOP_COUNT; i++) {
        intcode_t code = (intcode_t)irand() | ((intcode_t)irand() << (intcode_t)31);  // 62 random bits
        intcode_t code2 = (intcode_t)irand() | ((intcode_t)irand() << (intcode_t)31); // 62 random bits
        code = code & (numCodes - 1);
        code2 = code2 & (numCodes - 1);

        i3vec coords = toSFCurveCoords(code, crvType);
        i3vec coords2 = toSFCurveCoords(code2, crvType);

        int dist = (coords - coords2).length();
        float fdist = (f3vec(coords) - f3vec(coords2)).length();

        // std::cerr << code << " " << code2 << '\n';
        // std::cerr << i << ": " << coords << coords2 << dist << "=" << fdist << '\n';

        distanceHist[dist]++;
        distTotal += fdist;
    }
    std::cout << SFCurveNames[crvType] << "," << (distTotal / (float)LOOP_COUNT) << '\n';

    for (const auto& kv : distanceHist) { std::cout << kv.first << "," << kv.second << '\n'; }

    std::cerr << "\n\n\n";
}

// For random pairs of a given distance in one space, what is the histogram of distances in the other space?
template <typename intcode_t> void testCodeToCoordDistAll(SFCurveType crvType)
{
    printName<intcode_t>("testCodeToCoordDistAll", crvType);

    intcode_t numCodes = (intcode_t)1 << (intcode_t)(curveOrder<intcode_t>() * 3);

    for (intcode_t sep = 1; sep < 32; sep++) {
        std::vector<int> distanceHist(100, 0);
        float distTotal = 0.f, distMax = 0;

        for (int i = 0; i < LOOP_COUNT; i++) {
            intcode_t code = (intcode_t)irand() | ((intcode_t)irand() << (intcode_t)31); // 62 random bits
            code = code % (numCodes - sep - 1);
            intcode_t code2 = code + sep;

            i3vec coords = toSFCurveCoords(code, crvType);
            i3vec coords2 = toSFCurveCoords(code2, crvType);

            int dist = (coords - coords2).length();
            float fdist = (f3vec(coords) - f3vec(coords2)).length();

            // if (fdist > 1.f) std::cerr << i << ": " << code << "-" << code2 << coords << coords2 << dist << "=" << fdist << '\n';

            if (dist >= distanceHist.size()) dist = (int)distanceHist.size() - 1;
            distanceHist[dist]++;
            distTotal += fdist;
            distMax = max(distMax, fdist);
        }
        std::cout << SFCurveNames[crvType] << ",codeDiff," << sep << ",maxDist," << distMax << ",avgDist," << (distTotal / (float)LOOP_COUNT) << ",Hist,";

        for (size_t i = 0; i < distanceHist.size(); i++) { std::cout << distanceHist[i] << ","; }
        std::cout << std::endl;
    }

    std::cerr << "\n\n\n";
}

// For random pairs of a given distance in one space, what is the histogram of distances in the other space?
template <typename intcode_t> void testCoordToCodeDistAll(SFCurveType crvType)
{
    printName<intcode_t>("testCoordToCodeDistAll", crvType);

    intcode_t numCodes = (intcode_t)1 << (intcode_t)(curveOrder<intcode_t>() * 3);

    tAABB<i3vec> box(i3vec(0), i3vec(DIM - 1));

    for (int dist = 1; dist < LOOP_COUNT; dist++) {
        std::vector<int> separationHist(128, 0);
        intcode_t sepMax = 0;
        double sepTotal = 0, loopCount = 0;

        float distMin = dist - 0.99f, distMax = dist + 0.01f;
        distMin = 0.1f; // XXX All points up-to and including dist
        float distMinSqr = sqr(distMin), distMaxSqr = sqr(distMax);

        for (int i = 0; i < LOOP_COUNT; i++) {
            i3vec v = makeRand<i3vec>(0, DIM);
            // std::cerr << distMin << " " << distMax << " v " << v << '\n';

            i3vec dv, v2;
            f3vec dvf;
            do {
                dv = makeRand<i3vec>(i3vec(-dist - 1), i3vec(dist + 1));
                dvf = dv;
                v2 = v + dv;
            } while (dvf.lenSqr() < distMinSqr || dvf.lenSqr() > distMaxSqr || !box.contains(v2));
            // std::cerr << "dv" << dv << dvf.length() << '\n';

            intcode_t code = toSFCurveCode<intcode_t>(v, crvType);
            intcode_t code2 = toSFCurveCode<intcode_t>(v2, crvType);

            intcode_t sep = abs((int)code2 - (int)code);
            sepTotal += sep;
            loopCount++;
            sepMax = max(sepMax, sep);
            if (sep >= separationHist.size()) sep = (intcode_t)separationHist.size() - 1;
            separationHist[sep]++;
        }

        std::cout << SFCurveNames[crvType] << ",coordDist," << dist << ",maxCodeDiff," << sepMax << ",avgCodeDiff," << (sepTotal / loopCount) << ",Hist,";

        for (size_t i = 0; i < separationHist.size(); i++) { std::cout << separationHist[i] << ","; }
        std::cout << '\n';
    }

    std::cerr << "\n\n\n";
}

// Just a histogram of how many random points fall in each cell for random points. Not useful.
template <typename intcode_t> void testHistoRand(SFCurveType crvType)
{
    printName<intcode_t>("testHistoRand", crvType);

    std::vector<int> histo(DIM * DIM * DIM, 0);

    for (int i = 0; i < LOOP_COUNT; i++) {
        i3vec v = makeRand<i3vec>(0, DIM);
        std::cerr << v << '\n';
        intcode_t code = toSFCurveCode<intcode_t>(v, crvType);

        if (code >= (int)histo.size()) throw DMcError("Out of bounds");

        histo[code]++;
    }

    std::cerr << "Histogram:\n";
    for (int i = 0; i < (int)histo.size(); i++) { std::cerr << i << " " << histo[i] << '\n'; }
    std::cerr << "\n\n\n";
}

// Just a histogram of how many random points fall in each cell for all points. Not useful.
template <typename intcode_t> void testHistoAll(SFCurveType crvType)
{
    printName<intcode_t>("testHistoAll", crvType);

    std::vector<int> histo(DIM * DIM * DIM, 0);
    i3vec v;
    for (v.z = 0; v.z < DIM; v.z++) {
        for (v.y = 0; v.y < DIM; v.y++) {
            for (v.x = 0; v.x < DIM; v.x++) {
                intcode_t code = toSFCurveCode<intcode_t>(v, crvType);

                if (code >= (int)histo.size()) throw DMcError("Out of bounds");

                histo[code]++;

                // cerr << v << " " << code << '\n';
            }
        }
    }

    // Make sure each code was emitted exactly once
    for (int i = 0; i < (int)histo.size(); i++)
        if (histo[i] != 1) std::cerr << "testHistoAll Error: " << i << " " << histo[i] << '\n';

    std::cerr << "\n\n\n";
}

template <typename intcode_t> void testGrid(SFCurveType crvType)
{
    printName<intcode_t>("testGrid", crvType);

    std::vector<intcode_t> histo(DIM * DIM * DIM, 0);
    i3vec v;

    for (v.z = 0; v.z < DIM; v.z++) {
        for (v.y = 0; v.y < DIM; v.y++) {
            for (v.x = 0; v.x < DIM; v.x++) {
                intcode_t code = toSFCurveCode<intcode_t>(v, crvType);

                // std::cerr << v << " " << code << '\n';

                histo[v.z * DIM * DIM + v.y * DIM + v.z] = code;
            }
        }
    }

    // Make sure each code was emitted exactly once
    for (int i = 0; i < (int)histo.size(); i++)
        if (histo[i] >= DIM * DIM * DIM) std::cerr << "testGrid Error: " << i << " " << histo[i] << '\n';

    std::cerr << "\n\n\n";
}

// Compare the slow computeMortonCode vs. the fast computeMortonCodeF for random points
template <typename intcode_t> void testCompareRand(SFCurveType crvType)
{
    printName<intcode_t>("testCompareRand", crvType);

    for (int i = 0; i < LOOP_COUNT; i++) {
        i3vec v = makeRand<i3vec>(0, DIM);

        intcode_t code = toSFCurveCode<intcode_t>(v, crvType);
        intcode_t codeF = computeMortonCodeF<intcode_t>(v);

        if (code != codeF) std::cerr << x << " " << y << " " << z << " " << code << " " << codeF << '\n';
    }

    std::cerr << "\n\n\n";
}

// Compare the slow computeMortonCode vs. the fast computeMortonCodeF for all points
template <typename intcode_t> void testCompareAll(SFCurveType crvType)
{
    printName<intcode_t>("testCompareAll", crvType);
    i3vec v;

    for (v.z = 0; v.z < DIM; v.z++) {
        for (v.y = 0; v.y < DIM; v.y++) {
            for (v.x = 0; v.x < DIM; v.x++) {
                intcode_t code = toSFCurveCode<intcode_t>(v, crvType);
                intcode_t codeF = computeMortonCodeF<intcode_t>(v);

                if (code != codeF) std::cerr << v << " " << code << " " << codeF << '\n';
            }
        }
    }

    std::cerr << "\n\n\n";
}

// Try all the codes in order and print the coordinates
template <typename intcode_t> void testFollowCurveAll(SFCurveType crvType)
{
    printName<intcode_t>("testFollowCurveAll", crvType);

    intcode_t maxCode = (intcode_t)DIM * (intcode_t)DIM * (intcode_t)DIM;

    for (intcode_t code = 0; code < maxCode; code++) {
        i3vec v = toSFCurveCoords<intcode_t>(code, crvType);
        std::cerr << code << " " << v << '\n';
    }

    std::cerr << "\n\n\n";
}

template <typename intcode_t> void testPerf(SFCurveType crvType)
{
    printName<intcode_t>("testPerf", crvType);

    SRand(0);
    i3vec v = makeRand<i3vec>(0, DIM);
    Timer T;
    intcode_t codeSum = 0;

    for (int i = 0; i < LOOP_COUNT; i++) {
        intcode_t code = toSFCurveCode<intcode_t>(v, crvType);
        // intcode_t code = computeMortonCodeF<intcode_t>(v);
        v = toSFCurveCoords<intcode_t>(code, crvType);

        // if (code != codeF) std::cerr << x << " " << y << " " << z << " " << code << " " << codeF << '\n';
        codeSum += code;
    }

    std::cerr << T.Read() << " " << codeSum << "\n\n\n";
}

template <typename intcode_t> void testAll()
{
    // To CURVE_COUNT
    for (int crvType = 0; crvType < CURVE_COUNT; crvType++) {
        testCoordToCodeDistAll<intcode_t>((SFCurveType)crvType);
        // testPerf<intcode_t>((SFCurveType)crvType);
        // testCodeToCoordDistAll<intcode_t>((SFCurveType)crvType);
        // testCodeToCoordDistRand<intcode_t>((SFCurveType)crvType);
        // testFollowCurveAll<intcode_t>((SFCurveType)crvType);
        // testCodeToCoordToCodeRand<intcode_t>((SFCurveType)crvType);
        // testCodeToCoordToCodeAll<intcode_t>((SFCurveType)crvType);
        // testHistoRand<intcode_t>((SFCurveType)crvType);
        // testHistoAll<intcode_t>((SFCurveType)crvType);
        // testGrid<intcode_t>((SFCurveType)crvType);
        // testCompareRand<intcode_t>((SFCurveType)crvType);
        // testCompareAll<intcode_t>((SFCurveType)crvType);
    }
}

bool SpaceFillCurveTest(int argc, char** argv)
{
    std::cerr << "Starting SpaceFillCurveTest\n";

    testAll<uint32_t>();
    testAll<uint64_t>();

    // testFixedPointifier();

    std::cerr << "Ending SpaceFillCurveTest\n";

    return true;
}

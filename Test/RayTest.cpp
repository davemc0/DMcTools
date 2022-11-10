// RayTest - Test stuff about ray tracing
// Questions to answer:
// When a ray gets a bary of exactly 0 do all possible other triangles that share that edge also get 0? Needed for watertightness.
// Does this depend on the order of the vertices forming the edge?
// Does ray-box intersection always succeed for triangles where ray-triangle intersection succeeds?
// Try tessellated sphere for double hits and escapes
// Can SAH be refined?

#include "Math/AABB.h"
#include "Math/Intersect.h"
#include "Math/Random.h"
#include "Math/Triangle.h"
#include "Math/Vector.h"
#include "Math/VectorUtils.h"

#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

template <class Vec_T> tTriangle<Vec_T> makeNRandTri(const Vec_T ctr) { return {makeNRand<Vec_T>() + ctr, makeNRand<Vec_T>() + ctr, makeNRand<Vec_T>() + ctr}; }

// Replace one of the vertices randomly and see whether it still hits an edge or vertex case
template <class Vec_T> void testSurroundingEdges(const tTriangle<Vec_T>& tri0, const Vec_T& orig, const Vec_T& dir)
{
    for (int v = 0; v < 3; v++) {
        int isEdge_isHit = 0, isEdge_notHit = 0, notEdge_isHit = 0, notEdge_notHit = 0;

        for (int i = 0; i < 10000; i++) {
            tTriangle<Vec_T> tri = tri0;
            tri.v[v] = makeNRand<Vec_T>();

            typename Vec_T::ElType tHit = 0, bary0 = 0, bary1 = 0;
            bool isFrontFacing = false;
            bool isHit = RTI<Vec_T, typename Vec_T::ElType>(orig, dir, 0, DMC_MAXFLOAT, tri, false, false, true, // Intersection query
                                                            tHit, bary0, bary1, isFrontFacing);                  // Results
            bool edgeCase = bary0 || bary1;

            // if (isHit) std::cerr << "Rand: v=" << v << " isHit=" << isHit << tri << orig << ',' << dir << ' ' << edgeCase << '\n';
            if (isHit)
                if (edgeCase)
                    isEdge_isHit++;
                else
                    notEdge_isHit++;
            else if (edgeCase)
                isEdge_notHit++;
            else
                notEdge_notHit++;
        }

        std::cerr << "v=" << v << " isEdge_isHit=" << isEdge_isHit << " notEdge_isHit=" << notEdge_isHit << " isEdge_notHit=" << isEdge_notHit
                  << " notEdge_notHit=" << notEdge_notHit << std::endl;
    }
}

template <class Vec_T> void randTriRandRays()
{
    int hits = 0, edgeCases = 0, tries = 0;
    for (int i = 0; i < 10000; i++) {
        tTriangle<Vec_T> tri = makeNRandTri<Vec_T>(Vec_T());
        Vec_T ctr = tri.centroid();

        for (int r = 0; r < 1000000; r++) {
            Vec_T orig = makeNRand<f3vec>() * (typename Vec_T::ElType)10;
            Vec_T tgt = ctr + makeNRand<f3vec>() * (typename Vec_T::ElType)0.4; // Aim the ray toward tgt. Could randomize it.
            Vec_T dir = (tgt - orig).normalized();
            typename Vec_T::ElType tMin = 0, tMax = DMC_MAXFLOAT;
            typename Vec_T::ElType tHit = 0, bary0 = 0, bary1 = 0;
            bool isFrontFacing = false;

            bool isHit = RTI<Vec_T, typename Vec_T::ElType>(orig, dir, tMin, tMax, tri, false, false, true, // Intersection query
                                                            tHit, bary0, bary1, isFrontFacing);             // Results
            bool edgeCase = bary0 || bary1;

            if (isHit) hits++;
            if (edgeCase) {
                edgeCases++;
                std::cerr << edgeCases << "," << hits << "/" << tries << '\n';
                std::cerr << isHit << tri << orig << ',' << dir << ' ' << bary0 << "_" << bary1 << '\n';
                testSurroundingEdges(tri, orig, dir);
            }
            tries++;
        }
    }
}

template <class Vec_T> void fixedTriRandRays()
{
    tTriangle<Vec_T> tri = {Vec_T(-1, -1, 0), Vec_T(1, -1, 0), Vec_T(0, 1, 0)};
    Vec_T ctr = tri.centroid();

    for (int r = 0; r < 1000000; r++) {
        Vec_T orig = makeNRand<f3vec>() * (typename Vec_T::ElType)10;
        Vec_T tgt = ctr; // Aim the ray toward tgt. Could randomize it.
        Vec_T dir = (tgt - orig).normalized();
        typename Vec_T::ElType tMin = 0, tMax = DMC_MAXFLOAT;
        typename Vec_T::ElType tHit = 0, bary0 = 0, bary1 = 0;
        bool isFrontFacing = false;

        bool isHit = RTI<Vec_T, typename Vec_T::ElType>(orig, dir, tMin, tMax, tri, false, false, true, // Intersection query
                                                        tHit, bary0, bary1, isFrontFacing);             // Results

        if (!isHit) std::cerr << isHit << tri << orig << dir << bary0 << "_" << bary1 << '\n';
    }
}

template <class Vec_T> void fixedTriRayCast()
{
    tTriangle<Vec_T> tri = {Vec_T(-1, -1, 0), Vec_T(1, -1, 0), Vec_T(0, 1, 0)};
    Vec_T orig = {0, 0, 10};
    const float D = 2;

    for (orig.y = -D; orig.y < D; orig.y += 0.05) {
        for (orig.x = -D; orig.x < D; orig.x += 0.05) {
            Vec_T dir = {0, 0, -1};
            typename Vec_T::ElType tMin = 0, tMax = DMC_MAXFLOAT;
            typename Vec_T::ElType tHit = 0, bary0 = 0, bary1 = 0;
            bool isFrontFacing = false;

            bool isHit = RTI<Vec_T, typename Vec_T::ElType>(orig, dir, tMin, tMax, tri, false, true, true, // Intersection query
                                                            tHit, bary0, bary1, isFrontFacing);            // Results

            std::cerr << (isHit ? 'x' : '.');
        }
        std::cerr << std::endl;
    }
}

template <class Vec_T> void fixedTriList()
{
    std::vector<tTriangle<Vec_T>> triList = {
        //
        /*4*/
        {f3vec(-0.912413597106934, 0.991252481937408, -0.49364361166954), f3vec(1.08588290214539, 2.23145842552185, -0.500642001628876),
         f3vec(-0.0480434559285641, 1.26859247684479, -0.499918639659882)},
        /*2*/
        {f3vec(1.08588290214539, 2.23145842552185, -0.500642001628876), f3vec(-0.0480434559285641, 1.26859247684479, -0.499918639659882),
         f3vec(-0.912413597106934, 0.991252481937408, -0.49364361166954)},
        /*1*/
        {f3vec(-0.0480434559285641, 1.26859247684479, -0.499918639659882), f3vec(1.08588290214539, 2.23145842552185, -0.500642001628876),
         f3vec(-0.912413597106934, 0.991252481937408, -0.49364361166954)},
        /*3*/
        {f3vec(-1.13312327861785889, 2.09519386291503906, 0.98815995454788208), f3vec(-0.417993128299713135, 0.625265121459960938, -0.165169432759284973),
         f3vec(1.09847831726074219, -0.792442262172698975, 1.21720278263092041)},
        /*5*/
        {f3vec(-0.694982588291168, 0.3729168176651, 0.337574273347855), f3vec(1.22124695777893, -1.34817087650299, -1.01021504402161),
         f3vec(-0.405294358730316, 0.596838057041168, 1.85505986213684)},
        /*6*/
        {f3vec(1.02560102939606, 1.27942025661469, 0.20395028591156), f3vec(1.5848217010498, 1.88681578636169, 0.290507137775421),
         f3vec(-0.0609888546168804, -0.0243941526859999, -1.73975646495819)},
        /*1*/
        {f3vec(-0.694982588291168, 0.3729168176651, 0.337574273347855), f3vec(1.22124695777893, -1.34817087650299, -1.01021504402161),
         f3vec(-0.405294358730316, 0.596838057041168, 1.85505986213684)},
        {f3vec(-1.13312327861786, 2.09519386291504, 0.988159954547882), f3vec(-0.417993128299713, 0.625265121459961, -0.165169432759285),
         f3vec(1.09847831726074, -0.792442262172699, 1.21720278263092)},
        {f3vec(1.02560102939606, 1.27942025661469, 0.20395028591156), f3vec(1.5848217010498, 1.88681578636169, 0.290507137775421),
         f3vec(-0.0609888546168804, -0.0243941526859999, -1.73975646495819)},
        //
    };

    std::vector<std::pair<Vec_T, Vec_T>> rayList = //
        {
            {f3vec(-13.1288232803345, -7.13277626037598, -19.326286315918), f3vec(0.539997935295105, 0.357084929943085, 0.762163102626801)},
            {f3vec(-13.1288232803345, -7.13277626037598, -19.326286315918), f3vec(0.539997935295105, 0.357084929943085, 0.762163102626801)},
            {f3vec(-13.1288232803345, -7.13277626037598, -19.326286315918), f3vec(0.539997935295105, 0.357084929943085, 0.762163102626801)},
            {f3vec(1.06331217288970947, 0.657791376113891602, 5.37967300415039062), f3vec(0.0351661443710327148, -1.45023369789123535, -4.16247034072875977)},
            {f3vec(-11.0729446411132812, -9.4750518798828125, 8.55517482757568359), f3vec(12.2941913604736328, 8.12688064575195312, -9.56538963317871094)},
            {f3vec(-1.65805768966674805, -0.567242860794067383, 9.68963050842285156), f3vec(2.68365859985351562, 1.84666311740875244, -9.48568058013916016)},
            {f3vec(-11.0729446411133, -9.47505187988281, 8.55517482757568), f3vec(0.682584643363953, 0.553970754146576, -0.476649284362793)},
            {f3vec(1.06331217288971, 0.657791376113892, 5.37967300415039), f3vec(-0.220271050930023, -0.0553024262189865, -0.973869800567627)},
            {f3vec(-1.65805768966675, -0.567242860794067, 9.68963050842285), f3vec(0.239249005913734, 0.155104771256447, -0.958489596843719)},
            //
        };

    for (size_t t = 0; t < triList.size(); t++) {
        tTriangle<Vec_T>& tri = triList[t];
        Vec_T orig = rayList[t].first;
        Vec_T dir = rayList[t].second;

        typename Vec_T::ElType tMin = 0, tMax = DMC_MAXFLOAT;
        typename Vec_T::ElType tHit = 0, bary0 = 0, bary1 = 0;
        bool isFrontFacing = false;

        bool isHit = RTI<Vec_T, typename Vec_T::ElType>(orig, dir, tMin, tMax, tri, false, false, true, // Intersection query
                                                        tHit, bary0, bary1, isFrontFacing);             // Results

        std::cerr << "Orig: " << isHit << tri << orig << ',' << dir << ' ' << bary0 << "_" << bary1 << '\n';

        testSurroundingEdges(tri, orig, dir);
    }
}

bool RayTest(int argc, char** argv)
{
    std::cerr << std::setprecision(15);

    randTriRandRays<f3vec>();
    // fixedTriRandRays<f3vec>();
    // fixedTriRayCast<f3vec>();
    // fixedTriList<f3vec>();

    return true;
}

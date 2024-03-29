#include "Math/MiscMath.h"
#include "Math/Random.h"

#include <cmath>
#include <cstdio>

bool GaussianTest(int argc, char** argv)
{
    std::cerr << "Starting GaussianTest\n";

    double sigma = 0.7;
    double r1s = sigma * sigma;
    double r2s = -0.5 / r1s;
    double r2 = sqrt(2. * M_PI * r1s);

    for (double x = 0; x < 10; x += 0.1) {
        int yes = 0;
        double xs = x * x;
        // double gx = exp(xs * r2s) * r2;

        double gx = Gaussian(x, sigma);

        for (int i = 0; i < 1000000; i++) yes += (drand() < gx);

        printf("R %f %f\n", x, yes / 1000000.0);
        printf("G %f %f\n", x, gx);
    }
    exit(0);
    std::cerr << "Ending GaussianTest\n";

    return true;
}

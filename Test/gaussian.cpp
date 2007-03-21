#include <stdio.h>
#include <math.h>
#include <Math/MiscMath.h>

int main()
{
    double sigma = 0.7;
    double r1s = sigma*sigma;
    double r2s = -0.5/r1s;
    double r2 = sqrt(2. * M_PI * r1s);

    for(double x=0; x<10; x+= 0.1)
    {
        int yes = 0;
        double xs = x*x;
        // double gx = exp(xs * r2s) * r2;

        double gx = Gaussian(x, sigma);
#if 0
        for(int i=0; i<1000000; i++)
        {
            yes += (drand48() < gx);
        }
#endif
        printf("%f %d\n", x, yes);
        printf("%f %f\n", x, gx);
    }

    return 0;
}

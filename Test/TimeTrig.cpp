#include <Math/Vector.h>

#include <iostream>

using namespace std;

int main()
{
#define TSIZE 1024
    Vector Table[TSIZE];
    for(int i=0; i<TSIZE; i++)
    {
        Table[i] = Vector(drand48(), drand48(), drand48());
        cerr << Cross(Table[i],Table[i]) << endl;
        cerr << Abs(Table[i]) << endl;
        Table[i].normalize();
    }

    double Tt = 0, Pp = 0;
    for(int k=0,j=0; k<50000000; k++, j = (j+1) % TSIZE)
    {
        Vector &ray = Table[j];
        double P = asin(ray.y);

        double XZlen = sqrt(1 - Sqr(ray.y)); // 22.6
        // double XZlen = cos(P); // 30.8

        double ix = 1. / XZlen;
        double T = -atan2(ray.z * ix, ray.x * ix); // 30.8
        // double T = acos(ray.x / XZlen); if (ray.z > 0) T = -T; // 22.6

        Pp += P;
        Tt += T;
    }
  
    cerr << Pp << " " << Tt << endl;
}

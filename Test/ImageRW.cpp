#include <Image/ucImage.h>
#include <Util/Assert.h>
#include <Util/Timer.h>

#include <iostream>
using namespace std;

#include <stdlib.h>

int main(int argc, char **argv)
{
#ifdef BENCH
    // int loops = atoi(argv[2]);

    Timer Ti;
#else
    {
        ucImage Im(argv[1]);
        cerr << Im.wid<<"x"<<Im.hgt<<" "<< Im.size <<" "<< Im.chan<<" "<<Im.dsize << endl;
        for(int c=0; c<Im.chan; c++)
          cerr << int(Im.Pix[1000*Im.chan+c]) << " ";
        cerr << endl;

        char *outfname;
        if(argc > 2)
          outfname = argv[2];
        else
          {
            outfname = strdup(argv[1]);
            int l = strlen(outfname);
            outfname[l-3] = 't';
            outfname[l-2] = 'i';
            outfname[l-1] = 'f';
          }
        cerr << "Saving to " << outfname << endl;
        Im.Save(outfname);
        // return 0;
    }
#endif

#ifdef BENCH
    Ti.Reset();
    Ti.Start();

    for(int i=0; i<loops; i++)
    {
        ucImage Im(argv[1]);
        // cerr << Im.size << " " << Im.chan << endl;

        if(i>0 && (i%10 == 0))
        {
            cerr << i << " avg time = " << (Ti.Read() / double(i)) << endl;
        }

        // char fn[100];
        // sprintf(fn, "bob%04d.tif", i);
        // Im.Save(fn);
    }

    cerr << i << " avg time = " << (Ti.Read() / double(i)) << endl;
#endif

    return 0;
}

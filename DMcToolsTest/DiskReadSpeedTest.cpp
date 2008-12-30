#include "Util/Timer.h"
#include "Math/MiscMath.h"

#include <iostream>
#include <fstream>

using namespace std;

bool DiskReadSpeedTest(int argc, char **argv)
{
    const int NUM_CHUNKS_TO_TEST = 512;
    const int CHUNK_SIZE = (1024 * 256);
    const int NUM_CHUNKS_IN_MEM = 256;

    SRand();

    //ifstream bob("C:/DaveMc/WhiteFabric152new_2.8.big", ios::in | ios::binary);
    ifstream bob("G:\\DaveMcSBRDF\\Data\\BigFiles\\WhiteFabric152new_2.8.big", ios::in | ios::binary);
    if(!bob.is_open()) {
        cerr << "Failed to open file \n";
        exit(1);
    }

    cerr << "Reading\n";

    unsigned char *dat = new unsigned char[CHUNK_SIZE * NUM_CHUNKS_IN_MEM];

    int fsize_KB = (4 * 1024 * 1024);

    int nchunks_in_file = fsize_KB / (CHUNK_SIZE/1024);

    Timer T;
    T.Reset();

    T.Start();
    int disk_chunk = LRand() % nchunks_in_file;
    for(int i=0; i<NUM_CHUNKS_TO_TEST; i++) {
        disk_chunk = LRand() % nchunks_in_file;
        //disk_chunk = (disk_chunk+1) % nchunks_in_file;
        int mem_chunk = LRand() % NUM_CHUNKS_IN_MEM;
        //cerr << "Read " << CHUNK_SIZE << "B from " << disk_chunk << " to " << mem_chunk << endl;
        bob.seekg(streamoff(__int64(CHUNK_SIZE) * __int64(disk_chunk)));
        bob.read((char *)&(dat[CHUNK_SIZE * mem_chunk]), CHUNK_SIZE);
    }
    T.Stop();

    float t = T.Read();
    float bytes = float(NUM_CHUNKS_TO_TEST) * float(CHUNK_SIZE);
    cerr << "sec = " << t << " B = " << bytes << " MB/sec = " << ((bytes / float(1024*1024)) / t)
        << " sec/chunk = " << t/float(NUM_CHUNKS_TO_TEST) << " chunks/sec = " << float(NUM_CHUNKS_TO_TEST)/t << endl;
    exit(0);
}

bool DiskWriteTest(int argc, char **argv)
{
    const int CHUNK_SIZE = 1024 * 1024;
    SRand();

    ofstream bob("G:\\file.dat", ios::out | ios::binary);
    if(!bob.is_open()) {
        cerr << "Failed to open file \n";
        exit(1);
    }

    cerr << "Writing\n";

    char *dat = new char[CHUNK_SIZE];

    memset(dat, 0, CHUNK_SIZE);
    Timer T;
    T.Reset();

    T.Start();
    int l = 0;
    while(1) {
        bob.write(dat, CHUNK_SIZE);
        l++;
        float t = T.Read();
        float bytes = float(CHUNK_SIZE) * float(l);
        cerr << "sec = " << t << " B = " << bytes << " MB/sec = " << ((bytes / float(1024*1024)) / t) << endl;
    }

    exit(0);
}

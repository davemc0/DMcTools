#include "Math/MiscMath.h"
#include "Math/Random.h"
#include "Util/Timer.h"

#include <fstream>
#include <iostream>

// Read a bunch of random chunks and place them in random locations in memory
void ChunkReadTest(const int64_t iterations, const int64_t chunk_size, const int64_t mem_array_size, const int64_t nchunks_in_file, unsigned char* dat,
                   std::ifstream& thefile)
{
    const int64_t nchunks_in_mem = mem_array_size / chunk_size;

    Timer T;
    T.Reset();

    T.Start();

    for (int i = 0; i < iterations; i++) {
        int64_t disk_chunk = LRand() % nchunks_in_file;
        // disk_chunk = (disk_chunk+1) % nchunks_in_file;
        int64_t mem_chunk = LRand() % nchunks_in_mem;
        // std::cerr << "Read " << chunk_size << "B from " << disk_chunk << " to " << mem_chunk << std::endl;
        thefile.seekg(static_cast<std::streamoff>(chunk_size * disk_chunk));
        thefile.read((char*)&(dat[chunk_size * mem_chunk]), chunk_size);
    }
    T.Stop();

    float t = T.Read();
    float bytes = float(iterations) * float(chunk_size);
    std::cerr << "chunk_size = " << chunk_size << "\t sec = " << t << "\t KB = " << int64_t(bytes / 1024)
              << "\t MB/sec = " << ((bytes / float(1024 * 1024)) / t) << std::endl;
}

bool DiskReadTest(int argc, char** argv)
{
    std::cerr << "Starting DiskReadTest\n";

    const int64_t iterations_per_chunk_size = 512;
    const int64_t mem_array_size = 256 * 1024 * 1024;

    SRand();

    const char* fname = "testfile.dat";
    if (argc > 1) fname = argv[1];

    std::ifstream thefile(fname, std::ios::in | std::ios::binary);
    if (!thefile.is_open()) {
        std::cerr << "Failed to open file " << fname << " for reading\n";
        exit(1);
    }
    std::cerr << thefile.fail() << std::endl;
    std::cerr << "Reading " << fname << "\n";

    unsigned char* dat = new unsigned char[mem_array_size];

    thefile.seekg(0, std::ios::end);
    std::cerr << thefile.fail() << std::endl;

    std::ios::pos_type fsizep = thefile.tellg();
    std::cerr << thefile.fail() << " " << fsizep << std::endl;

    int64_t fsize = fsizep;

    if (fsize < mem_array_size || fsize == -1) {
        std::cerr << fsize << " File is too small.\n";
        exit(1);
    }

    for (int64_t chunk_size = 1024; chunk_size <= 128 * 1024 * 1024; chunk_size *= 2) {
        int64_t nchunks_in_file = fsize / chunk_size;
        ChunkReadTest(iterations_per_chunk_size, chunk_size, mem_array_size, nchunks_in_file, dat, thefile);
    }

    std::cerr << "Ending DiskReadTest\n";

    return true;
}

bool DiskWriteTest(int argc, char** argv)
{
    std::cerr << "Starting DiskWriteTest\n";

    const int CHUNK_SIZE = 16 * 1024 * 1024;
    SRand();

    const char* fname = "testfile.dat";
    if (argc > 1) fname = argv[1];

    std::ofstream thefile(fname, std::ios::out | std::ios::binary);
    if (!thefile.is_open()) {
        std::cerr << "Failed to open file " << fname << " for writing\n";
        exit(1);
    }

    std::cerr << "Writing " << fname << '\n';

    char* dat = new char[CHUNK_SIZE];

    memset(dat, 0, CHUNK_SIZE);
    Timer T;
    T.Reset();

    T.Start();

    for (int l = 0; l < 1024; l++) {
        thefile.write(dat, CHUNK_SIZE);
        float t = T.Read();
        float bytes = float(CHUNK_SIZE) * float(l);
        std::cerr << l << " sec = " << t << " KB = " << int64_t(bytes / 1024) << " MB/sec = " << ((bytes / float(1024 * 1024)) / t) << std::endl;
    }

    std::cerr << "Ending DiskWriteTest\n";

    return true;
}

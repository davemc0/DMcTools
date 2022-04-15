#include "Util/Assert.h"
#include "Util/Utils.h"

#include <cstdio>
#include <iostream>

bool HashStringTest(int argc, char** argv)
{
    std::cerr << "Starting HashStringTest\n";

    ASSERT_R(argc > 1);

    printf("'%s' = 0x%08x\n", argv[1], HashString(argv[1]));

    std::cerr << "Ending HashStringTest\n";

    return true;
}

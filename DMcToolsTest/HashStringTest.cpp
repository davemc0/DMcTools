#include "Util/Utils.h"

#include <cstdio>

bool HashStringTest(int argc, char **argv)
{
  printf("'%s' = 0x%08x\n", argv[1], HashString(argv[1]));

  return true;
}

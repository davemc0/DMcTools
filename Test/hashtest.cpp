#include <Util/Utils.h>

#include <stdio.h>

int main(int argc, char **argv)
{
  printf("'%s' = 0x%08x\n", argv[1], HashString(argv[1]));

  return 0;
}

#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{  
  struct sysinfo *p=0;
  sysinfo(p);
  exit(0);
}

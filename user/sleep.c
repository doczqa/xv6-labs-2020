#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(2, "Usage: sleep time\n");
        exit(1);
    }
    fprintf(2,"argv[0]:%s\n",argv[0]);
    fprintf(2,"argv[1]:%s\n",argv[1]);
    fprintf(2, "start sleep %d millseconds\n", atoi(argv[1]));
    if(sleep(atoi(argv[1])) < 0){
        fprintf(2, "sleep failed\n");
    }

    exit(0);
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p1[2];
    int p2[2];
    char rbuf[50];
    char wbuf[50];
    if(pipe(p1) == -1) {
        fprintf(2,"create pipe error!\n");
    }
    if(pipe(p2) == -1) {
        fprintf(2,"create pipe error!\n");
    }
    if(fork() == 0) {
        close(p1[1]);
        read(p1[0],rbuf,1);
        // fprintf(2,"son read over\n");
        close(p1[0]);
        if(rbuf[0] == 'a') {
            fprintf(1,"%d: received ping\n",getpid());
        }
        close(p2[0]);
        write(p2[1],"b",1);
        close(p2[1]);
        exit(0);
    } else {
        close(p1[0]);
        write(p1[1],"a",1);
        // fprintf(2,"father write over\n");
        close(p1[1]);
        wait(0);
        close(p2[1]);
        read(p2[0],wbuf,1);
        if(wbuf[0] == 'b') {
            fprintf(1,"%d: received pong\n",getpid());
        }
        close(p2[0]);
    }
    exit(0);
}

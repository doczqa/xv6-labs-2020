#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[35][2];
    int now=0;
    if(pipe(p[0]) < 0) {
        fprintf(2,"create pipe failed!\n");
    }
    int f = fork();
    if(f != 0) {
        close(p[0][0]);
        for(int i=2;i<=35;i++) {
            write(p[0][1], (void *)&i,4);
        }
        close(p[0][1]);
        wait(0);
    } else {
        while(1) {
            close(p[now][1]);
            int recnum;
            if(read(p[now][0],(void *)&recnum,4) == 0) {
                // fprintf(2,"exit-------\n");
                exit(0);
            }
            fprintf(1,"prime %d\n",recnum);
            now++;
            pipe(p[now]);
            if(fork() != 0) {
                int recnum2;
                while(read(p[now-1][0],(void *)&recnum2,4)) {
                    if(recnum2 % recnum != 0) {
                        write(p[now][1],(void *)&recnum2,4);
                    }
                }
                close(p[now-1][0]);
                close(p[now][1]);
                wait(0);
                exit(0);
            }
        }
    }
    exit(0);
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *dir,char *file) {
    // fprintf(2,"dir:%s\n",dir);
    // fprintf(2,"file:%s,file len:%d\n",file,strlen(file));
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(dir, 0)) < 0){
        fprintf(2, "find: cannot open %s,dir len:%d\n", dir,strlen(dir));
        return;
    }
    // fprintf(2, "find: open %s,dir len:%d\n", dir,strlen(dir));
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot open %s,dir len:%d\n", dir,strlen(dir));
        close(fd);
        return;
    }
    char *temp;
    switch(st.type){
        case T_FILE:
            temp = fmtname(dir);
            // printf("temp len:%d\n",strlen(temp));
            if(strcmp(temp,file)==0) {
                printf("%s\n", dir);
            }
            break;
        case T_DIR:
            if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, dir);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                // fprintf(2,"de.name:%s\n",de.name);
                memmove(p, de.name, DIRSIZ);
                // fprintf(2,"p:%s\n",p);
                p[DIRSIZ] = 0;

                // fprintf(2,"buf:%s, len:%d\n",buf,strlen(buf));
                // if(stat(buf, &st) < 0){
                //     printf("ls: cannot stat %s\n", buf);
                //     continue;
                // }
                char *fname = fmtname(buf);
                // fprintf(2,"fname:%s\n",fname);
                if(fname[0]=='.') {
                    continue;
                }
                find(buf,file);
            }
            close(fd);
            break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2,"usage:find dir file");
    exit(0);
  }
  char buf[DIRSIZ+1];
  memmove(buf, argv[2], strlen(argv[2]));
  memset(buf+strlen(argv[2]), ' ', DIRSIZ-strlen(argv[2]));
//   printf("buf size:%d\n",strlen(buf));
  find(argv[1],buf);
  exit(0);
}

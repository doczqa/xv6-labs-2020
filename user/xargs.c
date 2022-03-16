#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char buf[512];
  if(argc <= 1){
    exit(0);
  }
  char *argv2[10];
  int argc2=0;
  static char temp[512];
  argv2[argc2] = temp;
  int l=0;
  for(int i = 1; i < argc; i++){
    for(int j=0;j<strlen(argv[i]);j++) {
        temp[l++] = argv[i][j];
    }
    temp[l++] = '\0';
    argv2[++argc2] = &temp[l];
  }
  while(read(0,buf,sizeof(buf))) {
    // printf("read:%s,buf len:%d\n",buf,strlen(buf));
    // argv[argc] = temp;
    for(int i=0;i<strlen(buf);i++) {
        if(buf[i] == ' ' || buf[i] == '\n') {
            temp[l++] = '\0';
            argv2[++argc2] = &temp[l];
            continue;
        }
        temp[l++] = buf[i];
    }
    // printf("temp:%s,temp len:%d\n",temp,strlen(temp));
  }
//   fprintf(2,"++++bye:%s\n",bye);
//   for(int i=0;i<argc2;i++) {
//       printf("argv2:%s\n",argv2[i]);
//   }
  exec(argv2[0], argv2);
  exit(0);
}

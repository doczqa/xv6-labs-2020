#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

// Fetch the uint64 at addr from the current process.
int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(addr >= p->sz || addr+sizeof(uint64) > p->sz)
    return -1;
  if(copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  int err = copyinstr(p->pagetable, buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  *ip = argraw(n);
  return 0;
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
int
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, buf, max);
}

extern uint64 sys_chdir(void);
extern uint64 sys_close(void);
extern uint64 sys_dup(void);
extern uint64 sys_exec(void);
extern uint64 sys_exit(void);
extern uint64 sys_fork(void);
extern uint64 sys_fstat(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_mknod(void);
extern uint64 sys_open(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_unlink(void);
extern uint64 sys_wait(void);
extern uint64 sys_write(void);
extern uint64 sys_uptime(void);
extern uint64 sys_trace(void);
extern uint64 sys_sysinfo(void);

static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,//1
[SYS_exit]    sys_exit,//2
[SYS_wait]    sys_wait,//3
[SYS_pipe]    sys_pipe,//4
[SYS_read]    sys_read,//5
[SYS_kill]    sys_kill,//6
[SYS_exec]    sys_exec,//7
[SYS_fstat]   sys_fstat,//8
[SYS_chdir]   sys_chdir,//9
[SYS_dup]     sys_dup,//10
[SYS_getpid]  sys_getpid,//11
[SYS_sbrk]    sys_sbrk,//12
[SYS_sleep]   sys_sleep,//13
[SYS_uptime]  sys_uptime,//14
[SYS_open]    sys_open,//15
[SYS_write]   sys_write,//16
[SYS_mknod]   sys_mknod,//17
[SYS_unlink]  sys_unlink,//18
[SYS_link]    sys_link,//19
[SYS_mkdir]   sys_mkdir,//20
[SYS_close]   sys_close,//21
[SYS_trace]   sys_trace,//22
[SYS_sysinfo]   sys_sysinfo,//23
};

static char names[32][10] = {{""},{"fork"},{"exit"},{"wait"},{"pipe"},{"read"},{"kill"},
{"exec"},{"fstat"},{"chdir"},{"dup"},{"getpid"},
{"sbrk"},{"sleep"},{"uptime"},{"open"},{"write"},{"mknod"},{"unlink"},{"link"},{"mkdir"},{"close"},{"trace"},{"sysinfo"}
};

void
syscall(void)
{
  int num;
  struct proc *p = myproc();
  // printf("syscall(void)\n");
  num = p->trapframe->a7;
  // printf("p->trapframe->a7:%d\n",num);
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // printf("p->trapframe->a7:%d\n",num);
    p->trapframe->a0 = syscalls[num](); //syscalls 存储着各个系统调用的函数指针，a0 存储着返回结果？
    if(p->mask[num] == 1) {
      printf("%d: syscall %s -> %d\n",p->pid, names[num], p->trapframe->a0);
    }
    // printf("p->trapframe->a0:%d\n",p->trapframe->a0);
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  printf("sys_getpid/n");
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0) //get p->trapframe->a0，a0 是保存的参数。
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_trace(void)
{
  long int record[32];
  record[0] = 1;
  for(int i=1;i<32;i++) {
    record[i] = record[i-1]*2;
  }
  int n;
  if(argint(0, &n) < 0) //get p->trapframe->a0，a0 是保存的参数。
    return -1;
  // printf("n:%d-------------------\n", n);
  for(int i=31;i>=0;i--) {
    if(record[i] <= n) {
      // printf("record[i]:%d-------------------\n", record[i]);
      myproc()->mask[i] = 1;
      n -= record[i];
    }
  }
  // printf("n:%d-------------------\n", n);
  return 0;
}

uint64
sys_sysinfo(void)
{
  // printf("there are %d blocks left-------------------\n", getFreeMemory());
  // printf("there are %d unused proc-------------------\n", getUnusedProc());
  uint64 blocks = getFreeMemory();
  uint64 unusedProc = getUnusedProc();

  uint64 res;
  // printf("res:%d\n",res);
  struct proc *p = myproc();
  if(argaddr(0, &res) < 0)
    return -1;
  // printf("res:%d\n",res);
  struct sysinfo temp;
  temp.freemem = blocks;
  temp.nproc = unusedProc;

  if(copyout(p->pagetable, res, (char *)&temp, sizeof(temp)) < 0) //p->pagetable 的类型是 uint64 *
      return -1;
  // printf("there are %d blocks left----copy---------------\n", res->freemem);
  return res;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

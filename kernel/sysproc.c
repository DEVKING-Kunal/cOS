#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
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

// Sv39 page table walker for Resident Set Size (RSS) calculation.
// Traverses virtual address space [0, sz) and tallies valid user pages.
static uint64
calc_rss(pagetable_t pagetable, uint64 sz)
{
  uint64 va;
  uint64 rss = 0;

  if (pagetable == 0)
    return 0;

  for (va = 0; va < sz; va += PGSIZE) {
    pte_t *pte = walk(pagetable, va, 0);
    if (pte != 0 && (*pte & PTE_V) && (*pte & PTE_U)) {
      rss += PGSIZE;
    }
  }
  return rss;
}

// System call procinfo (Syscall #23):
// Marshalling: argint(0, &pid), argaddr(1, &uaddr)
// If pid == 0: inspects calling process (myproc()).
// If pid > 0: inspects matching process in proc[] under spinlock.
// Returns 0 on success, -1 on error.
uint64
sys_procinfo(void)
{
  int pid;
  uint64 uaddr;
  struct proc *curr = myproc();
  struct proc *target = 0;
  struct proc_info kinfo;

  argint(0, &pid);
  argaddr(1, &uaddr);

  if (pid < 0 || uaddr == 0)
    return -1;

  memset(&kinfo, 0, sizeof(kinfo));

  if (pid == 0 || pid == curr->pid) {
    target = curr;
    acquire(&target->lock);
  } else {
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if (p->state != UNUSED && p->pid == pid) {
        target = p;
        break;
      }
      release(&p->lock);
    }
  }

  if (target == 0)
    return -1;

  // Copy telemetry data from target proc under target->lock
  kinfo.pid = target->pid;
  kinfo.ppid = target->parent ? target->parent->pid : 0;
  kinfo.state = target->state;
  safestrcpy(kinfo.name, target->name, sizeof(kinfo.name));
  kinfo.sz = target->sz;
  kinfo.cpu_ticks = target->cpu_ticks;
  kinfo.ctx_switches = target->ctx_switches;
  kinfo.page_faults = target->page_faults;
  kinfo.total_syscalls = target->total_syscalls;
  memmove(kinfo.syscall_counts, target->syscall_counts, sizeof(kinfo.syscall_counts));

  // Compute RSS via Sv39 page table walk
  kinfo.rss = calc_rss(target->pagetable, target->sz);

  // CRITICAL: Release spinlock BEFORE invoking copyout!
  release(&target->lock);

  // CRITICAL: 5-argument copyout required by repo's demand-paging virtual memory system
  if (copyout(curr->pagetable, curr->sz, uaddr, (char *)&kinfo, sizeof(kinfo)) < 0)
    return -1;

  return 0;
}

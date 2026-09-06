#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/syscall.h"

#ifndef SYS_procinfo
#define SYS_procinfo 23
#endif

#ifndef _PROC_INFO_
#define _PROC_INFO_
struct proc_info {
  int pid;                      // Process ID
  int ppid;                     // Parent Process ID
  int state;                    // Process state (UNUSED, SLEEPING, RUNNABLE, etc.)
  uint64 sz;                    // Virtual memory size in bytes
  uint64 rss;                   // Resident Set Size (physical bytes currently mapped)
  uint64 cpu_ticks;             // CPU timer ticks accumulated while running
  uint64 ctx_switches;          // Voluntary & involuntary context switches
  uint64 page_faults;           // Demand-paging page faults handled
  uint64 total_syscalls;        // Total system calls executed
  uint32 syscall_counts[32];    // Frequency per system call number
  char name[16];                // Process command name
};
#endif

// Prototype in case user.h does not yet expose it
int procinfo(int, struct proc_info *);

#define TEST_NAME "testsyscall"

static void
assert_failed(const char *msg, const char *file, int line)
{
  printf("FAILURE [%s:%d]: %s\n", file, line, msg);
  exit(1);
}

#define ASSERT(cond, msg) \
  do { \
    if (!(cond)) { \
      assert_failed(msg, __FILE__, __LINE__); \
    } \
  } while (0)

// test Self-query (pid=0 and pid=getpid())
static void
test_basic(void)
{
  struct proc_info info;
  int my_pid = getpid();

  printf("test_basic: starting self-query test...\n");

  // Query with pid = 0 (caller shorthand)
  int ret = procinfo(0, &info);
  ASSERT(ret == 0, "procinfo(0, &info) returned error");
  ASSERT(info.pid == my_pid, "info.pid does not match getpid()");
  ASSERT(strcmp(info.name, TEST_NAME) == 0, "info.name does not match executable name");
  ASSERT(info.sz > 0, "info.sz must be greater than 0");
  ASSERT(info.rss > 0, "info.rss must be greater than 0");
  ASSERT(info.rss <= info.sz, "info.rss cannot exceed virtual size info.sz");
  ASSERT(info.total_syscalls > 0, "info.total_syscalls must be positive");
  ASSERT(info.syscall_counts[SYS_procinfo] >= 1, "SYS_procinfo count must be at least 1");

  // Query with explicit pid
  struct proc_info info_explicit;
  ret = procinfo(my_pid, &info_explicit);
  ASSERT(ret == 0, "procinfo(my_pid, &info) returned error");
  ASSERT(info_explicit.pid == my_pid, "explicit info.pid mismatch");
  ASSERT(info_explicit.sz == info.sz, "explicit query sz mismatch");
  ASSERT(info_explicit.rss == info.rss, "explicit query rss mismatch");

  printf("test_basic: PASS (pid=%d, name=%s, sz=%lu, rss=%lu, syscalls=%lu)\n",
         info.pid, info.name, (unsigned long)info.sz, (unsigned long)info.rss,
         (unsigned long)info.total_syscalls);
}

// test Init process query (pid=1)
static void
test_init_query(void)
{
  struct proc_info info;

  printf("test_init_query: querying init process (pid=1)...\n");
  int ret = procinfo(1, &info);
  ASSERT(ret == 0, "procinfo(1, &info) returned error");
  ASSERT(info.pid == 1, "expected pid 1");
  ASSERT(strcmp(info.name, "init") == 0, "expected name 'init'");
  ASSERT(info.sz > 0, "init sz must be positive");
  ASSERT(info.rss > 0, "init rss must be positive");

  printf("test_init_query: PASS (init pid=1, name=%s, sz=%lu, rss=%lu)\n",
         info.name, (unsigned long)info.sz, (unsigned long)info.rss);
}

// test System call count tracking (uptime() and getpid())
static void
test_syscall_counting(void)
{
  struct proc_info b4, after;

  printf("test_syscall_counting: verifying deterministic syscall tracking...\n");

  ASSERT(procinfo(0, &b4) == 0, "initial procinfo failed");
  uint32 initial_uptime_calls = b4.syscall_counts[SYS_uptime];
  uint32 initial_getpid_calls = b4.syscall_counts[SYS_getpid];
  uint64 initial_total = b4.total_syscalls;

  // Execute exactly 5 uptime syscalls
  for (int i = 0; i < 5; i++) {
    uptime();
  }

  // Execute exactly 3 getpid syscalls
  for (int i = 0; i < 3; i++) {
    getpid();
  }

  ASSERT(procinfo(0, &after) == 0, "subsequent procinfo failed");

  // Verify uptime counter increased by exactly 5
  uint32 delta_uptime = after.syscall_counts[SYS_uptime] - initial_uptime_calls;
  ASSERT(delta_uptime == 5, "uptime count delta was not exactly 5");

  // Verify getpid counter increased by exactly 3
  uint32 delta_getpid = after.syscall_counts[SYS_getpid] - initial_getpid_calls;
  ASSERT(delta_getpid == 3, "getpid count delta was not exactly 3");

  // Verify total syscalls increased by at least 5 uptime + 3 getpid + 1 procinfo
  uint64 delta_total = after.total_syscalls - initial_total;
  ASSERT(delta_total >= 9, "total syscall delta was less than expected (5 uptime + 3 getpid + 1 procinfo)");

  printf("test_syscall_counting: PASS (uptime delta=%u, getpid delta=%u, total delta=%lu)\n",
         delta_uptime, delta_getpid, (unsigned long)delta_total);
}

// test Memory and page fault tracking (sbrklazy, touching memory)
static void
test_lazy_pagefaults(void)
{
  struct proc_info b4, during, after;

  printf("test_lazy_pagefaults: verifying lazy memory allocation and page fault counts...\n");

  ASSERT(procinfo(0, &b4) == 0, "initial procinfo failed");
  uint64 initial_faults = b4.page_faults;
  uint64 initial_rss = b4.rss;
  uint64 initial_sz = b4.sz;

  // Allocate 3 pages lazily (virtual size expands, physical RAM unallocated)
  int num_pages = 3;
  int alloc_size = num_pages * 4096;
  char *ptr = sbrklazy(alloc_size);
  ASSERT(ptr != (char *)-1, "sbrklazy failed");

  ASSERT(procinfo(0, &during) == 0, "intermediate procinfo failed");
  ASSERT(during.sz == initial_sz + alloc_size, "virtual size did not expand properly");
  ASSERT(during.page_faults == initial_faults, "lazy allocation must not trigger page faults yet");

  // Now touch each lazy page, triggering page faults in hardware
  for (int i = 0; i < num_pages; i++) {
    ptr[i * 4096] = (char)(0xAA + i);
  }

  ASSERT(procinfo(0, &after) == 0, "post-touch procinfo failed");
  uint64 fault_delta = after.page_faults - initial_faults;
  ASSERT(fault_delta == (uint64)num_pages, "page fault count delta did not match touched pages");
  ASSERT(after.rss >= initial_rss + alloc_size, "RSS did not increase by allocated page size");

  // Shrink heap back
  sbrk(-alloc_size);

  printf("test_lazy_pagefaults: PASS (faults before=%lu, after=%lu, delta=%lu, rss delta=%lu)\n",
         (unsigned long)initial_faults, (unsigned long)after.page_faults,
         (unsigned long)fault_delta, (unsigned long)(after.rss - initial_rss));
}

// test Boundary conditions and error handling
static void
test_invalid_args(void)
{
  struct proc_info info;

  printf("test_invalid_args: verifying fault tolerance on invalid inputs...\n");

  // 1. Negative PID
  ASSERT(procinfo(-1, &info) == -1, "procinfo(-1) must return -1");
  ASSERT(procinfo(-999, &info) == -1, "procinfo(-999) must return -1");

  // 2. Non-existent PID
  ASSERT(procinfo(30000, &info) == -1, "procinfo(30000) must return -1");

  // 3. NULL pointer for struct
  ASSERT(procinfo(0, (struct proc_info *)0) == -1, "procinfo(0, NULL) must return -1");

  // 4. Kernel memory address (0x80000000) - user must not write to kernel
  ASSERT(procinfo(0, (struct proc_info *)0x80000000LL) == -1, "procinfo with kernel address must return -1");

  // 5. Trampoline / Trapframe virtual address (highest virtual page)
  ASSERT(procinfo(0, (struct proc_info *)0x3fffffe000LL) == -1, "procinfo with trapframe address must return -1");

  // 6. Address beyond MAXVA (Sv39 38-bit virtual address limit: 0x4000000000)
  ASSERT(procinfo(0, (struct proc_info *)0x4000000000LL) == -1, "procinfo above MAXVA must return -1");

  // 7. Max 64-bit address
  ASSERT(procinfo(0, (struct proc_info *)0xffffffffffffffffLL) == -1, "procinfo with 0xFFF... must return -1");

  printf("test_invalid_args: PASS (all error conditions returned -1 cleanly without panicking)\n");
}

// test Child process test (fork(), PID, PPID, telemetry isolation)
static void
test_fork_inheritance(void)
{
  printf("test_fork_inheritance: testing child process hierarchy and isolation...\n");
  int parent_pid = getpid();
  int pid = fork();

  ASSERT(pid >= 0, "fork failed");

  if (pid == 0) {
    // Child process
    struct proc_info cinfo;
    ASSERT(procinfo(0, &cinfo) == 0, "child procinfo failed");
    ASSERT(cinfo.pid == getpid(), "child pid mismatch");
    ASSERT(cinfo.pid != parent_pid, "child pid equals parent pid");
    ASSERT(cinfo.ppid == parent_pid, "child ppid mismatch");
    ASSERT(strcmp(cinfo.name, TEST_NAME) == 0, "child name mismatch");
    ASSERT(cinfo.sz > 0, "child sz must be positive");
    ASSERT(cinfo.rss > 0, "child rss must be positive");
    exit(0);
  }

  // Parent process
  int status = 0;
  wait(&status);
  ASSERT(status == 0, "child exited with non-zero status");

  printf("test_fork_inheritance: PASS (child correctly verified ppid=%d and isolated stats)\n",
         parent_pid);
}

// main
int
main(int argc, char *argv[])
{
  printf("========================================\n");
  printf("   cos tests   \n");
  printf("   Target: procinfo (Syscall #23)       \n");
  printf("========================================\n");

  test_basic();
  test_init_query();
  test_syscall_counting();
  test_lazy_pagefaults();
  test_invalid_args();
  test_fork_inheritance();

  printf("========================================\n");
  printf("all tests passed\n");
  printf("tests completed\n");
  printf("========================================\n");

  exit(0);
}

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if (cpuid() == 0) {
    consoleinit();
    printkinit();
    printk("\n");
    printk("            /$$$$$$   /$$$$$$ \n");
    printk("           /$$__  $$ /$$__  $$\n");
    printk("  /$$$$$$$| $$  \\ $$| $$  \\__/\n");
    printk(" /$$_____/| $$  | $$|  $$$$$$ \n");
    printk("| $$      | $$  | $$ \\____  $$\n");
    printk("| $$      | $$  | $$ /$$  \\ $$\n");
    printk("|  $$$$$$$|  $$$$$$/|  $$$$$$/\n");
    printk(" \\_______/ \\______/  \\______/ \n");
    printk("\n");
    printk("cOS kernel is booting\n");
    printk("\n");
    
    printk("[kinit] Initializing physical memory allocator...\n");
    kinit();            // physical page allocator
    printk("[kvminit] Creating kernel page table at kernel base...\n");
    kvminit();          // create kernel page table
    printk("[kvminithart] Enabling SV39 hardware paging...\n");
    kvminithart();      // turn on paging
    printk("[procinit] Initializing process control blocks...\n");
    procinit();         // process table
    printk("[trapinit] Setting up trap vectors and exception handlers...\n");
    trapinit();         // trap vectors
    printk("[trapinithart] Installing kernel trap vector to stvec...\n");
    trapinithart();     // install kernel trap vector
    printk("[plicinit] Configuring PLIC...\n");
    plicinit();         // set up interrupt controller
    printk("[plicinithart] Routing UART and VIRTIO interrupts to hart 0...\n");
    plicinithart();     // ask PLIC for device interrupts
    printk("[binit] Allocating memory for disk buffer cache...\n");
    binit();            // buffer cache
    printk("[iinit] Initializing inode directory cache...\n");
    iinit();            // inode table
    printk("[fileinit] Setting up global file descriptor table...\n");
    fileinit();         // file table
    printk("[virtio_disk_init] Probing virtio-mmio bus for block devices...\n");
    virtio_disk_init(); // emulated hard disk
    printk("[userinit] Handing off control to first user process (init)...\n");
    userinit();         // first user process

    __atomic_store_n(&started, 1, __ATOMIC_RELEASE);
  } else {
    while (__atomic_load_n(&started, __ATOMIC_ACQUIRE) == 0)
      ;

    printk("hart %d starting\n", cpuid());
    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  scheduler();
}

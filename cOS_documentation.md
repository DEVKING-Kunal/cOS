# cOS (Curiosity Operating System): The Comprehensive Systems Engineering Textbook

**Author**: cOS Core Engineering Team  
**Architecture**: 64-bit RISC-V (`rv64gc`) on QEMU `virt` machine  
**Edition**: First Edition (Release 1.0)

---

## Table of Contents

- [Chapter 1: Introduction to cOS & Architectural Overview](#chapter-1-introduction-to-cos--architectural-overview)
  - [1.1 What is cOS?](#11-what-is-cos)
  - [1.2 Target Hardware: RISC-V 64-bit (rv64gc)](#12-target-hardware-risc-v-64-bit-rv64gc)
  - [1.3 Privilege Rings: Machine, Supervisor, and User Modes](#13-privilege-rings-machine-supervisor-and-user-modes)
  - [1.4 The Subsystem Architecture of cOS](#14-the-subsystem-architecture-of-cos)
  - [1.5 The ASCII Boot Splash Screen](#15-cos-rebranding--the-ascii-boot-splash-screen)
- [Chapter 2: Hardware Startup & The Boot Sequence](#chapter-2-hardware-startup--the-boot-sequence)
  - [2.1 The Bootstrapping Mental Model](#21-the-bootstrapping-mental-model)
  - [2.2 Assembly Bootstrap: kernel/entry.S](#22-assembly-bootstrap-kernelentrys)
  - [2.3 Machine Mode Setup & Privilege Drop: kernel/start.c](#23-machine-mode-setup--privilege-drop-kernelstartc)
  - [2.4 Supervisor Mode Bring-up & SMP: kernel/main.c](#24-supervisor-mode-bring-up--smp-kernelmainc)
- [Chapter 3: Console & Diagnostic I/O](#chapter-3-console--diagnostic-io)
  - [3.1 The Layered Console Mental Model](#31-the-layered-console-mental-model)
  - [3.2 16550A UART Hardware Driver: kernel/uart.c](#32-16550a-uart-hardware-driver-kerneluartc)
  - [3.3 Console Line Discipline & Device Layer: kernel/console.c](#33-console-line-discipline--device-layer-kernelconsolec)
  - [3.4 Formatted Output & Panic Handling: kernel/printk.c](#34-formatted-output--panic-handling-kernelprintkc)
- [Chapter 4: Memory Management & Sv39 Virtual Memory](#chapter-4-memory-management--sv39-virtual-memory)
  - [4.1 Physical Memory vs. Virtual Memory: The Illusion of Solitude](#41-physical-memory-vs-virtual-memory-the-illusion-of-solitude)
  - [4.2 RISC-V Sv39 Three-Level Page Table Architecture](#42-risc-v-sv39-three-level-page-table-architecture)
  - [4.3 Memory Layout Landmarks: kernel/memlayout.h and kernel/riscv.h](#43-memory-layout-landmarks-kernelmemlayouth-and-kernelriscvh)
  - [4.4 Physical Page Allocator: kernel/kalloc.c](#44-physical-page-allocator-kernelkallocc)
  - [4.5 Virtual Memory Subsystem: kernel/vm.c](#45-virtual-memory-subsystem-kernelvmc)
- [Chapter 5: Process Management, Lifecycle, & Context Switching](#chapter-5-process-management-lifecycle--context-switching)
  - [5.1 The Process Abstraction: Process Control Block (struct proc)](#51-the-process-abstraction-process-control-block-struct-proc)
  - [5.2 Process State Machine](#52-process-state-machine)
  - [5.3 Process Lifecycle Operations: kernel/proc.c](#53-process-lifecycle-operations-kernelprocc)
  - [5.4 Low-Level Context Switch: kernel/swtch.S](#54-low-level-context-switch-kernelswtchs)
  - [5.5 Two-Phase Sleep and Wakeup Synchronization](#55-two-phase-sleep-and-wakeup-synchronization)
- [Chapter 6: Traps, Interrupts, & Exception Handling](#chapter-6-traps-interrupts--exception-handling)
  - [6.1 The Trap Mental Model](#61-the-trap-mental-model)
  - [6.2 The Dual-Mapped Trampoline Page: kernel/trampoline.S](#62-the-dual-mapped-trampoline-page-kerneltrampolines)
  - [6.3 Trap Dispatching: kernel/trap.c](#63-trap-dispatching-kerneltrapc)
  - [6.4 Kernel-Mode Traps: kernel/kernelvec.S](#64-kernel-mode-traps-kernelkernelvecs)
- [Chapter 7: The System Call Subsystem](#chapter-7-the-system-call-subsystem)
  - [7.1 The Boundary Between User and Kernel](#71-the-boundary-between-user-and-kernel)
  - [7.2 System Call Numbering: kernel/syscall.h](#72-system-call-numbering-kernelsyscallh)
  - [7.3 Dispatching and Argument Extraction: kernel/syscall.c](#73-dispatching-and-argument-extraction-kernelsyscallc)
  - [7.4 File and Process System Calls: kernel/sysfile.c and kernel/sysproc.c](#74-file-and-process-system-calls-kernelsysfilec-and-kernelsysprocc)
- [Chapter 8: Custom Feature Deep-Dive: sys_procinfo (Syscall #23)](#chapter-8-custom-feature-deep-dive-sys_procinfo-syscall-23)
  - [8.1 Motivation in Quantitative Systems Engineering](#81-motivation-in-quantitative-systems-engineering)
  - [8.2 Data Structure: struct proc_info (All 11 Fields Explained)](#82-data-structure-struct-proc_info-all-11-fields-explained)
  - [8.3 Kernel Telemetry Instrumentation Points](#83-kernel-telemetry-instrumentation-points)
  - [8.4 Sv39 Resident Set Size (RSS) Calculation Algorithm: calc_rss](#84-sv39-resident-set-size-rss-calculation-algorithm-calc_rss)
  - [8.5 Concurrency Discipline & Spinlock Order Safety](#85-concurrency-discipline--spinlock-order-safety)
  - [8.6 User-Space Interface & Test Suite: user/testsyscall.c](#86-user-space-interface--test-suite-usertestsyscallc)
- [Chapter 9: Building, Running, and Debugging cOS](#chapter-9-building-running-and-debugging-cos)
  - [9.1 Toolchain Prerequisites](#91-toolchain-prerequisites)
  - [9.2 Building and Booting in QEMU](#92-building-and-booting-in-qemu)
  - [9.3 Running testsyscall and Interpreting Diagnostics](#93-running-testsyscall-and-interpreting-diagnostics)
  - [9.4 Kernel Debugging Techniques (GDB, procdump)](#94-kernel-debugging-techniques-gdb-procdump)

---

# Chapter 1: Introduction to cOS & Architectural Overview

### 1.1 What is cOS?
**cOS** (Curiosity Operating System) is an educational, production-modeled operating system built upon the foundation of MIT's famous xv6-riscv operating system. This project extends the core xv6 kernel with custom observability and telemetry subsystems. While preserving the minimalist, pedagogical elegance that makes cOS the gold standard for operating systems education, cOS modernizes the kernel into an observability-focused platform suited for modern systems engineering and quantitative computing environments.

In quantitative finance and distributed computing, understanding exact execution latencies, context switch penalties, memory residency, and cache invalidation is non-negotiable. cOS augments classic Unix abstractions with fine-grained kernel telemetry, zero-overhead page table inspection, and resilient demand-paging tracking.

### 1.2 Target Hardware: RISC-V 64-bit (`rv64gc`)
cOS targets the open standard **RISC-V** instruction set architecture (ISA), specifically the 64-bit variant (`RV64GC`):
- **Base ISA**: `RV64I` (64-bit integer registers $x0$ through $x31$).
- **Standard Extensions**:
  - `M`: Hardware integer multiplication and division.
  - `A`: Atomic memory instructions (load-reserved/store-conditional, atomic swap/add).
  - `F` and `D`: Single- and double-precision IEEE 754 floating-point units.
  - `C`: Compressed 16-bit instructions for dense code packaging.

When running under QEMU (`qemu-system-riscv64 -machine virt`), the virtual hardware provides:
- Multiple physical CPU cores called **Harts** (Hardware Threads).
- Physical RAM starting at address `0x80000000` (`KERNBASE`) up to `0x88000000` (`PHYSTOP`, 128 Megabytes).
- Standard memory-mapped I/O (MMIO) devices: UART 16550A serial console at `0x10000000`, VirtIO block device at `0x10001000`, Platform-Level Interrupt Controller (PLIC) at `0x0C000000`, and Core-Local Interruptor (CLINT) at `0x02000000`.

### 1.3 Privilege Rings: Machine, Supervisor, and User Modes
RISC-V defines three distinct hardware execution privilege levels:

```
+-------------------------------------------------------------------+
|               User Mode (U-Mode / Privilege 0)                    |
|   Unprivileged user programs, shell, compilers, testsyscall       |
+---------------------------------+---------------------------------+
                                  | System Calls (ecall) / Traps
                                  v
+-------------------------------------------------------------------+
|            Supervisor Mode (S-Mode / Privilege 1)                 |
|   cOS Operating System Kernel (kalloc, vm, proc, trap, devsw)     |
|   MMU active (Sv39 Virtual Memory Translation enforced)           |
+---------------------------------+---------------------------------+
                                  | Early Boot / Hardware Setup
                                  v
+-------------------------------------------------------------------+
|              Machine Mode (M-Mode / Privilege 3)                  |
|   Highest physical privilege; direct hardware control             |
|   entry.S, start.c, timerinit(), PMP configuration               |
+-------------------------------------------------------------------+
```

1. **Machine Mode (M-mode)**: The hardware boots here. The CPU has direct, unconstrained access to physical memory, hardware CSRs, and device pins. No virtual memory translation exists in M-mode.
2. **Supervisor Mode (S-mode)**: The execution environment of the cOS kernel. The hardware Memory Management Unit (MMU) is active, translating virtual addresses via the `satp` register. S-mode cannot directly modify M-mode configuration registers.
3. **User Mode (U-mode)**: Where unprivileged user binaries (such as `/init`, `sh`, and `testsyscall`) execute. Any attempt by U-mode to execute privileged instructions or access memory without the `PTE_U` permission flag triggers a hardware exception.

### 1.4 The Subsystem Architecture of cOS

```
+-------------------------------------------------------------------------+
|                               USER SPACE                                |
|        /init          sh          cat          ls         testsyscall   |
|                                                                         |
|        User Virtual Address Space: [0x0 -----------> MAXVA - 2*PGSIZE]   |
+------------------------------------+------------------------------------+
                                     | System Call Interface (ecall)
                                     v
+-------------------------------------------------------------------------+
|                   TRAMPOLINE (Shared: MAXVA - PGSIZE)                   |
|       uservec: Save registers, switch SATP to kernel_pagetable          |
|       userret: Restore registers, switch SATP to user_pagetable         |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                           cOS KERNEL SPACE                              |
|                                                                         |
|   +-----------------------------------------------------------------+   |
|   |                     Process & Scheduling Subsystem              |   |
|   |  proc.c, swtch.S, proc.h: struct proc, scheduler(), yield()     |   |
|   +-----------------------------------------------------------------+   |
|                                    |                                    |
|   +--------------------------------v--------------------------------+   |
|   |                     System Call & Telemetry Engine              |   |
|   |  syscall.c, sysproc.c: sys_procinfo (#23), calc_rss()           |   |
|   +-----------------------------------------------------------------+   |
|                                    |                                    |
|   +--------------------------------v--------------------------------+   |
|   |                     Virtual Memory Subsystem (Sv39)             |   |
|   |  vm.c: walk(), mappages(), copyout(), copyin(), vmfault()       |   |
|   +-----------------------------------------------------------------+   |
|                                    |                                    |
|   +--------------------------------v--------------------------------+   |
|   |                     Physical Memory Allocator                   |   |
|   |  kalloc.c: kmem.freelist, kalloc(), kfree(), freerange()        |   |
|   +-----------------------------------------------------------------+   |
|                                    |                                    |
|   +--------------------------------v--------------------------------+   |
|   |                     Device & File Subsystems                    |   |
|   |  uart.c, console.c, printk.c, bio.c, fs.c, virtio_disk.c        |   |
|   +-----------------------------------------------------------------+   |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                         RISC-V HARDWARE (QEMU)                          |
|   Harts 0..2 | Sv39 MMU | DRAM (128 MB) | UART 16550A | VirtIO Block   |
+-------------------------------------------------------------------------+
```

### 1.5 The ASCII Boot Splash Screen
When cOS initializes, it proudly announces itself through a distinctive 8-line ASCII art banner ecustomted over the serial console. This custom banner is the first thing that greets the user upon boot:

```
            /$$$$$$   /$$$$$$ 
           /$$__  $$ /$$__  $$
  /$$$$$$$| $$  \ $$| $$  \__/
 /$$_____/| $$  | $$|  $$$$$$ 
| $$      | $$  | $$ \____  $$
| $$      | $$  | $$ /$$  \ $$
|  $$$$$$$|  $$$$$$/|  $$$$$$/
 \_______/ \______/  \______/ 
```

The boot sequence logs each subsystem as it comes alive, providing diagnostic transparency during system bring-up.

---

# Chapter 2: Hardware Startup & The Boot Sequence

### 2.1 The Bootstrapping Mental Model
*Analogy*: Imagine launching a nuclear submarine. When the power switches on, the submarine cannot immediately begin sonar scans or sub-surface navigation. First, emergency power must be supplied to the engine room (assembly bootstrap). Next, safety inspectors calibrate reactor gauges and transfer command authority to the control room (Machine Mode setup). Finally, the ship's captain activates radar, communication radios, life support, and orders the crew to stations (Supervisor Mode initialization and process scheduling).

In cOS, the hardware follows this exact chain of custody:
1. `kernel/entry.S` (Machine Mode): Allocates execution stacks for all cores.
2. `kernel/start.c` (Machine Mode): Delegates hardware interrupts, configures timers, sets memory boundaries, and drops to Supervisor Mode.
3. `kernel/main.c` (Supervisor Mode): Brings up physical memory, virtual paging, interrupts, device drivers, the file system, and launches the first user process (`/init`).

### 2.2 Assembly Bootstrap: `kernel/entry.S`

#### Responsibility
`entry.S` is the very first software executed by the CPU. At reset, the C runtime does not exist: there is no stack, no heap, and no global variables initialized. C functions cannot execute without a stack pointer register (`sp`), because local variables and return addresses must be pushed onto RAM. `entry.S` provides each CPU core with its own private 4096-byte stack.

#### Code Listing & Instruction Breakdown
```assembly
.section .text
.global _entry
_entry:
        la sp, stack0
        li a0, 1024*4
        csrr a1, mhartid
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
        call start
spin:
        j spin
```

#### Step-by-Step Logic:
1. `la sp, stack0`: Loads the 64-bit base address of `stack0`, a contiguous memory array defined in `start.c` as `char stack0[4096 * NCPU]`.
2. `li a0, 1024*4`: Loads immediate value `4096` ($4\text{ KB}$, the size of a memory page).
3. `csrr a1, mhartid`: Reads the Control and Status Register `mhartid` (Machine Hardware Thread ID). In a 3-core QEMU system, the cores receive IDs `0`, `1`, and `2`.
4. `addi a1, a1, 1`: Computes $(\text{mhartid} + 1)$ (i.e. 1, 2, or 3).
5. `mul a0, a0, a1`: Calculates $4096 \times (\text{mhartid} + 1)$.
6. `add sp, sp, a0`: Sets $\text{sp} = \text{stack0} + ((\text{mhartid} + 1) \times 4096)$.
   *Why add 4096 instead of 0?* In the RISC-V ABI, the stack **grows downward** (from high memory toward low memory). Pushing data decrements `sp`. Therefore, `sp` must point to the *ceiling* (top) of the core's 4KB stack slice!
7. `call start`: Jumps into the C function `start()` in `kernel/start.c`.
8. `spin: j spin`: Fallback guard. If `start()` ever returns (which it must never do), the core enters an infinite loop, preventing execution of garbage memory.

### 2.3 Machine Mode Setup & Privilege Drop: `kernel/start.c`

#### Responsibility
`start.c` executes entirely in RISC-V Machine Mode. Its primary mission is to configure hardware traps, memory permissions, and clock timers, and safely transition (drop privilege) into Supervisor Mode to run `main()`.

#### Key Global Data
- `__attribute__((aligned(16))) char stack0[4096 * NCPU];`  
  Allocates the 16-byte aligned boot stacks used by `entry.S`.

#### Core Functions

#### 1. `void start()`
- **Privilege Mode**: Machine Mode (M-Mode)
- **Signature**: `void start(void)`
- **Parameters**: None.
- **Return Value**: Does not return (`mret` branches to `main()`).
- **Internal Logic Walkthrough**:
  1. **Privilege Mode Drop Preparation (`mstatus`)**:
     ```c
     unsigned long x = r_mstatus();
     x &= ~MSTATUS_MPP_MASK;
     x |= MSTATUS_MPP_S;
     w_mstatus(x);
     ```
     The `mstatus` register contains the `MPP` (Machine Previous Privilege) field. When the CPU executes an `mret` (Machine Return) instruction, it restores the privilege mode stored in `MPP`. By setting `MPP` to `MSTATUS_MPP_S`, `mret` instructs the hardware to drop from M-mode into Supervisor Mode.
  2. **Setting Return Target (`mepc`)**:
     ```c
     w_mepc((uint64)main);
     ```
     `mret` jumps to the address contained in `mepc` (Machine Exception Program Counter). Loading `main` ensures the CPU branches straight into `main.c`.
  3. **Disabling MMU Translation (`satp`)**:
     ```c
     w_satp(0);
     ```
     Writing 0 to `satp` ensures virtual address translation is disabled; early boot operates directly on physical memory addresses.
  4. **Delegating Traps to Supervisor Mode (`medeleg`, `mideleg`)**:
     ```c
     w_medeleg(0xffff);
     w_mideleg(0xffff);
     w_sie(r_sie() | SIE_SEIE | SIE_STIE);
     ```
     By default, all exceptions and interrupts trap directly to M-mode. `medeleg` (Machine Exception Delegation) and `mideleg` (Machine Interrupt Delegation) instruct the CPU hardware to route exceptions, system calls, and interrupts directly into Supervisor Mode handlers, bypassing M-mode overhead.
  5. **Physical Memory Protection (`pmpaddr0`, `pmpcfg0`)**:
     ```c
     w_pmpaddr0(0x3fffffffffffffull);
     w_pmpcfg0(0xf);
     ```
     Without PMP permissions, M-mode hardware forbids S-mode from accessing RAM. Setting `pmpaddr0` to cover all 56 physical address bits and `pmpcfg0` to `0xf` (Read, Write, Execute, NAPOT) grants S-mode full access to system memory.
  6. **Hardware Page Table Bits Configuration (`menvcfg`)**:
     ```c
     w_menvcfg(r_menvcfg() | MENVCFG_ADUE);
     ```
     Enables the hardware automatic updating of Access (`A`) and Dirty (`D`) bits in page table entries (Svadu extension).
  7. **Timer Configuration**:
     Calls `timerinit()` to configure the per-hart clock interrupt.
  8. **Preserving Hart ID in Thread Pointer (`tp`)**:
     ```c
     int id = r_mhartid();
     w_tp(id);
     ```
     The `mhartid` register is accessible *only* in M-mode. Once in S-mode, reading `mhartid` triggers a illegal instruction fault. Therefore, `start()` reads `mhartid` while still in M-mode and writes it into the `tp` (Thread Pointer) register. Throughout cOS, `cpuid()` simply reads `tp`.
  9. **Executing `mret`**:
     ```c
     asm volatile("mret");
     ```
     The processor changes privilege to Supervisor Mode, sets `pc = main`, and begins running `main.c`!

#### 2. `void timerinit()`
- **Signature**: `void timerinit(void)`
- **Parameters**: None.
- **Internal Logic**:
  1. Enables the RISC-V `sstc` extension by setting `MENVCFG_STCE` in `menvcfg`.
  2. Enables S-mode access to the hardware real-time cycle counter by setting bit 1 in `mcounteren`.
  3. Sets the first supervisor timer interrupt threshold:
     ```c
     w_stimecmp(r_time() + 1000000);
     ```
     When the hardware counter `time` reaches `stimecmp` (approximately 0.1 seconds at 10 MHz), the hardware fires a timer interrupt into S-mode.

### 2.4 Supervisor Mode Bring-up & SMP: `kernel/main.c`

#### Responsibility
`main.c` coordinates symmetric multiprocessing (SMP) bring-up. It separates the Bootstrap Processor (Hart 0) from secondary application processors (Harts 1..N), initialises every kernel subsystem sequentially, displays the boot splash screen, and starts the scheduler.

#### Global Synchronization Flag
```c
volatile static int started = 0;
```

#### Bootstrap Processor (Hart 0) Execution Flow
When `cpuid() == 0`, Hart 0 executes the following 17 initialization steps:

| Step | Function Call | Purpose & Internal Actions |
|---|---|---|
| 1 | `consoleinit()` | Initializes console spinlock and 16550A UART hardware. |
| 2 | `printkinit()` | Initializes `pr.lock` protecting formatted console output. |
| 3 | `printk(...)` | Displays the 8-line cOS ASCII splash screen and welcome banner. |
| 4 | `kinit()` | Populates physical memory free list from `end` to `PHYSTOP`. |
| 5 | `kvminit()` | Builds the master kernel Sv39 page table with identity and high-memory mappings. |
| 6 | `kvminithart()` | Writes kernel root page table to `satp` and flushes the TLB via `sfence_vma()`. |
| 7 | `procinit()` | Initializes `proc[NPROC]` state locks and allocates per-process kernel stacks. |
| 8 | `trapinit()` | Initializes trap spinlocks for clock tick synchronization. |
| 9 | `trapinithart()`| Installs kernel trap handler `kernelvec` into the `stvec` CSR. |
| 10 | `plicinit()` | Sets interrupt priorities for UART (IRQ 10) and VirtIO (IRQ 1) in the PLIC. |
| 11 | `plicinithart()`| Enables UART and VirtIO interrupts on Hart 0. |
| 12 | `binit()` | Allocates and links disk block buffer cache (`bcache`). |
| 13 | `iinit()` | Initializes the disk inode cache table. |
| 14 | `fileinit()` | Initializes the global open file descriptor table. |
| 15 | `virtio_disk_init()` | Handshakes with the VirtIO MMIO disk controller and configures queues. |
| 16 | `userinit()` | Creates PID 1 (`initproc`), which will load and execute `/init`. |
| 17 | `__atomic_store_n(&started, 1, __ATOMIC_RELEASE)` | Executes an atomic memory barrier, setting `started = 1` and releasing secondary harts. |

#### Secondary Cores (Harts > 0) Execution Flow
Secondary cores execute the `else` branch of `main()`:
```c
while (__atomic_load_n(&started, __ATOMIC_ACQUIRE) == 0)
  ;

printk("hart %d starting\n", cpuid());
kvminithart();  // turn on paging
trapinithart(); // install kernel trap vector
plicinithart(); // ask PLIC for device interrupts
```
- Cores spin-wait on `started` using an `ACQUIRE` barrier to guarantee they see all memory written by Hart 0.
- Each secondary core activates Sv39 paging using the page table created by Hart 0.
- Each installs `kernelvec` into its local `stvec` register.
- All cores call `scheduler()`, entering the infinite process scheduling loop.

---

# Chapter 3: Console & Diagnostic I/O

### 3.1 The Layered Console Mental Model
The console subsystem forms a three-tier pipeline connecting physical UART hardware pins to user-space standard input/output (`stdin`/`stdout`):

```
+-------------------------------------------------------------------------+
|     User Application / Shell (read(0, buf, n), write(1, buf, n))        |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|               Console Device Layer (kernel/console.c)                  |
|   - devsw[CONSOLE].read  -> consoleread(): Line-buffering, blocking     |
|   - devsw[CONSOLE].write -> consolewrite(): Chunked user buffer copying |
|   - consoleintr(): Processes backspace, Ctrl-U, Ctrl-P, Ctrl-D          |
+------------------------------------+------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                  16550A UART Driver (kernel/uart.c)                     |
|   - uartwrite(): Asynchronous, interrupt-driven writes with sleeplock   |
|   - uartputc_sync(): Polled, busy-wait writes for printk() and panic()  |
|   - uartintr(): Hardware interrupt handler; drains RX FIFO              |
+------------------------------------+------------------------------------+
                                     | Memory-Mapped I/O (MMIO)
                                     v
+-------------------------------------------------------------------------+
|            Physical 16550A Serial Controller (Address: 0x10000000)      |
+-------------------------------------------------------------------------+
```

### 3.2 16550A UART Hardware Driver: `kernel/uart.c`

#### Responsibility
`uart.c` drives the National Semiconductor 16550A UART serial controller mapped at physical memory address `0x10000000` (`UART0`). It translates kernel bytes into serial pulses and handles byte transmission queues.

#### Memory-Mapped Register Access
```c
#define Reg(reg) ((volatile unsigned char *)(UART0 + (reg)))
#define ReadReg(reg)     (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))
```
The `volatile` qualifier prevents the compiler from optimizing out repeated reads or reordering hardware writes.

#### Core Functions

#### 1. `void uartinit(void)`
- **Signature**: `void uartinit(void)`
- **Logic**:
  1. `WriteReg(IER, 0x00)`: Disables interrupts during configuration.
  2. `WriteReg(LCR, LCR_BAUD_LATCH)`: Sets Divisor Latch Access Bit (DLAB).
  3. `WriteReg(0, 0x03); WriteReg(1, 0x00)`: Sets baud divisor to 3 ($38,400\text{ baud}$).
  4. `WriteReg(LCR, LCR_EIGHT_BITS)`: Sets 8 data bits, no parity, 1 stop bit (8N1).
  5. `WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR)`: Enables and resets hardware 16-byte FIFOs.
  6. `WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE)`: Enables transcustom and receive interrupts.
  7. `initsleeplock(&tx_lock, "uart")`: Initializes sleep-lock serializing user writers.

#### 2. `void uartwrite(char buf[], int n)`
- **Signature**: `void uartwrite(char buf[], int n)`
- **Parameters**: `buf[]` (array of bytes to transcustom), `n` (byte count).
- **Logic**: Acquires `tx_lock`. For each character:
  - Registers to sleep on `&tx_chan` via `sleep_prepare(&tx_chan)`.
  - Checks if transcustom FIFO is ready (`ReadReg(LSR) & LSR_TX_IDLE`).
  - If ready, writes `buf[i]` into `THR` (Transcustom Holding Register).
  - If busy, calls `sleep()` to yield the CPU until a transcustom interrupt fires.
  - Releases `tx_lock` upon completion.

#### 3. `void uartputc_sync(int c)`
- **Signature**: `void uartputc_sync(int c)`
- **Parameters**: `c` (ASCII character byte).
- **Purpose**: Synchronous, polled character output. Does not sleep or rely on interrupts. Used exclusively by `printk()`, console echo, and `panic()`.
- **Logic**: Disables local interrupts via `push_off()`. Busy-waits polling `ReadReg(LSR) & LSR_TX_IDLE`. Once idle, writes `c` to `THR`. Restores interrupts via `pop_off()`.

#### 4. `void uartintr(void)`
- **Signature**: `void uartintr(void)`
- **Logic**: Top-half interrupt handler invoked when PLIC raises IRQ 10.
  - Reads `ISR` to acknowledge the interrupt to the UART chip.
  - If transcustom buffer is idle, awakens sleeping writers: `wakeup(&tx_chan)`.
  - While receiver FIFO has data (`ReadReg(LSR) & LSR_RX_READY`), reads byte from `RHR` and forwards to `consoleintr(c)`.

### 3.3 Console Line Discipline & Device Layer: `kernel/console.c`

#### Responsibility
`console.c` implements the terminal **line discipline**, buffering interactive keystrokes into lines so user programs read whole lines when Enter is pressed, and handling editing keystrokes (`^U`, backspace).

#### Circular Buffer State
```c
struct {
  struct spinlock lock;
#define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r; // Read index: where user processes read from
  uint w; // Write index: where completed lines end
  uint e; // Edit index: where interactive typing currently happens
} cons;
```
**Buffer Invariant**: $r \le w \le e$. Characters between $w$ and $e$ are currently being typed and edited; they are invisible to `consoleread()` until Enter (`\n`) or EOF (`^D`) comcustoms them.

#### Core Functions

#### 1. `void consoleintr(int c)`
- **Signature**: `void consoleintr(int c)`
- **Parameters**: `c` (incoming raw ASCII byte).
- **Logic**: Acquires `cons.lock`. Handles special characters:
  - `C('P')` (Ctrl-P): Calls `procdump()` to dump all process states and stack traces to the screen for diagnostic inspection.
  - `C('U')` (Ctrl-U): Erases the entire current line. Rewinds `cons.e` back to `cons.w`, ecustomting backspaces to erase characters visually.
  - `C('H')` (Ctrl-H / Backspace / Delete): If `cons.e != cons.w`, decrements `cons.e--` and ecustoms `\b \b`.
  - Regular characters: Echoes character to terminal via `consputc(c)`, stores in `cons.buf[cons.e++ % INPUT_BUF_SIZE]`. If character is `\n` or `^D`, sets `cons.w = cons.e` and calls `wakeup(&cons.r)` to wake up waiting readers.

#### 2. `int consoleread(int user_dst, uint64 dst, int n)`
- **Signature**: `int consoleread(int user_dst, uint64 dst, int n)`
- **Parameters**: `user_dst` (1 if user address, 0 if kernel), `dst` (destination address), `n` (byte count).
- **Return Value**: Number of bytes read.
- **Logic**: Acquires `cons.lock`. While `cons.r == cons.w` (no completed lines), sleeps on `&cons.r`. Once awakened, copies characters from `cons.buf[cons.r++ % INPUT_BUF_SIZE]` to `dst` using `either_copyout()` until a newline or `^D` is transferred.

#### 3. `int consolewrite(int user_src, uint64 src, int n)`
- **Signature**: `int consolewrite(int user_src, uint64 src, int n)`
- **Logic**: Batches user buffer into 32-byte chunks using `either_copyin()`, forwarding each chunk to `uartwrite()`.

### 3.4 Formatted Output & Panic Handling: `kernel/printk.c`

#### Responsibility
`printk.c` (rebranded from `printf.c`) provides formatted printing (`printk`) and kernel halts (`panic`). It is completely independent of user memory, heap allocators (`kalloc`), and sleeping locks.

#### Core Functions

#### 1. `int printk(char *fmt, ...)`
- **Signature**: `int printk(char *fmt, ...)`
- **Supported Format Specifiers**: `%d` (signed int), `%u` (unsigned int), `%x` (hex), `%p` (pointer), `%s` (string), `%c` (char), `%ld`/`%lld` (64-bit signed int), `%lu`/`%llu` (64-bit unsigned int), `%lx`/`%llx` (64-bit hex).
- **Concurrency**: Protected by spinlock `pr.lock`. If `panicking == 0`, acquires `pr.lock` to prevent garbled interleaving from multiple cores. Parses the format string and prints numbers via `printint()` and `printptr()`.

#### 2. `void panic(char *s)`
- **Signature**: `void panic(char *s)`
- **Logic**:
  1. Sets `panicking = 1`. This instructs `printk()` to bypass acquiring `pr.lock`, guaranteeing that panic output will never deadlock even if a core crashed inside `printk`.
  2. Prints `"panic: "` followed by the string `s`.
  3. Sets `panicked = 1`, commanding all other cores to halt output.
  4. Enters an infinite spin loop `for(;;);`.

---

# Chapter 4: Memory Management & Sv39 Virtual Memory

### 4.1 Physical Memory vs. Virtual Memory: The Illusion of Solitude
In physical memory, DRAM is a linear sequence of bytes from address `0x80000000` to `0x88000000`. Without virtual memory, if Program A writes to address `0x80100000`, it could silently overwrite Program B's stack or the kernel's process table.

cOS enforces **Virtual Memory Isolation**:
- Every user process runs in its own private virtual address space from `0x0` to `MAXVA` (256 Gigabytes).
- Process A and Process B can both execute instructions at virtual address `0x1000`, but their virtual addresses map to completely distinct physical memory pages in DRAM.
- The kernel is protected: user code cannot read or modify kernel pages because the hardware MMU checks the `PTE_U` (User) flag. If `PTE_U` is 0, user access triggers an immediate hardware page fault.

### 4.2 RISC-V Sv39 Three-Level Page Table Architecture
cOS uses the standard RISC-V **Sv39** paging scheme:
- A virtual address is 64 bits wide, but only the lower **39 bits** participate in address translation (giving $2^{39} = 512\text{ GB}$ addressable space; cOS licustoms `MAXVA` to $2^{38} = 256\text{ GB}$ to ensure bit 38 is 0, avoiding sign extension).

```
 38         30 29         21 20         12 11                  0
+-------------+-------------+-------------+--------------------+
|   VPN[2]    |   VPN[1]    |   VPN[0]    |    Page Offset     |
|   (9 bits)  |   (9 bits)  |   (9 bits)  |     (12 bits)      |
+-------------+-------------+-------------+--------------------+
       |             |             |                  |
       v             v             v                  v
 [Root Level 2] [Dir Level 1] [Leaf Level 0]   [Byte inside 4KB Page]
```

#### Page Table Entry (PTE) Structure
Each page table page is exactly $4096\text{ bytes}$ and contains $512$ entries of $8\text{ bytes}$ each ($512 \times 8 = 4096$).
```
 63      54 53                               10 9   8 7 6 5 4 3 2 1 0
+----------+-----------------------------------+-----+---------------+
| Reserved |    PPN (Physical Page Number)     | RSW | D A G U X W R V
| (10 bits)|             (44 bits)             |(2 b)|   PTE Flags   |
+----------+-----------------------------------+-----+---------------+
```
- **PPN (Physical Page Number)**: Shifting PPN left by 12 bits (`PPN << 12`) yields the physical address of the target page.
- **PTE Flags**:
  - `PTE_V` (Bit 0): **Valid**. If 0, the entry is unmapped; accessing it triggers a page fault.
  - `PTE_R` (Bit 1): **Readable**.
  - `PTE_W` (Bit 2): **Writable**.
  - `PTE_X` (Bit 3): **Executable**.
  - `PTE_U` (Bit 4): **User accessible**. If 0, accessible only by Supervisor mode.
- **Directory vs. Leaf**:
  - If `(PTE_R | PTE_W | PTE_X) == 0`: The entry is a **Directory Pointer** pointing to the physical address of a lower-level page table.
  - If any of `PTE_R`, `PTE_W`, `PTE_X` is set: The entry is a **Leaf PTE** mapping directly to physical data RAM.

### 4.3 Memory Layout Landmarks: `kernel/memlayout.h` and `kernel/riscv.h`

```
Virtual Address Space
+-------------------------------------------------------------+ MAXVA (0x4000000000)
| TRAMPOLINE (Trap vector code shared by kernel and all procs)| MAXVA - PGSIZE
+-------------------------------------------------------------+
| TRAPFRAME (Saved user registers; user page table only)      | TRAMPOLINE - PGSIZE
+-------------------------------------------------------------+
| KSTACK 63 (Kernel stack for process 63)                     |
| Guard page (Unmapped)                                       |
| ...                                                         |
| KSTACK 0 (Kernel stack for process 0)                       |
| Guard page (Unmapped)                                       |
+-------------------------------------------------------------+
| [Direct Mapped Kernel Memory: VA == PA]                     |
| Free Physical RAM: [etext -> PHYSTOP] (Read/Write)          | 0x88000000 (PHYSTOP)
| Kernel Text & Read-Only Data: [KERNBASE -> etext] (RX)      | 0x80000000 (KERNBASE)
| PLIC (Interrupt Controller): 0x0C000000                     |
| VIRTIO0 (Disk MMIO): 0x10001000                             |
| UART0 (Serial Console MMIO): 0x10000000                     |
+-------------------------------------------------------------+ 0x00000000
```

### 4.4 Physical Page Allocator: `kernel/kalloc.c`

#### Responsibility
`kalloc.c` manages raw physical memory in fixed 4096-byte slices. It maintains an **intrusive singly linked list** of free pages. Because an unused page is empty RAM, the kernel stores the pointer to the next free page directly inside the first 8 bytes of the free page itself (`struct run`), achieving zero metadata memory overhead.

#### Core Functions

#### 1. `void kinit(void)`
- **Signature**: `void kinit(void)`
- **Logic**: Initializes `kmem.lock` and calls `freerange(end, (void*)PHYSTOP)`.

#### 2. `void freerange(void *pa_start, void *pa_end)`
- **Signature**: `void freerange(void *pa_start, void *pa_end)`
- **Logic**: Rounds `pa_start` up to the next 4KB boundary, iterates page-by-page to `pa_end`, and calls `kfree(p)` on every page.

#### 3. `void kfree(void *pa)`
- **Signature**: `void kfree(void *pa)`
- **Parameters**: `pa` (physical address of 4KB page).
- **Logic**:
  1. Panics if `pa` is not 4KB aligned, is less than `end`, or exceeds `PHYSTOP`.
  2. **Defensive Poisoning**: Fills the entire page with `1` (`memset(pa, 1, PGSIZE)`). Any buggy code attempting to read memory after freeing will read garbage and fault immediately.
  3. Acquires `kmem.lock`, pushes the page onto the head of `kmem.freelist`, and releases `kmem.lock`.

#### 4. `void *kalloc(void)`
- **Signature**: `void *kalloc(void)`
- **Return Value**: Pointer to 4KB physical page, or `0` if memory is exhausted.
- **Logic**: Acquires `kmem.lock`, pops the head page from `kmem.freelist`, and releases `kmem.lock`. If valid, fills page with junk byte `5` (`memset(r, 5, PGSIZE)`) to catch uninitialized reads, and returns `(void *)r`.

### 4.5 Virtual Memory Subsystem: `kernel/vm.c`

#### Responsibility
`vm.c` implements the software Sv39 page table walker, virtual-to-physical address mappings, process address space destruction, safe user-kernel memory transfers (`copyout`, `copyin`), and lazy demand-paging resolution (`vmfault`).

#### Core Functions Walkthrough

#### 1. `pte_t *walk(pagetable_t pagetable, uint64 va, int alloc)`
- **Purpose**: Simulates the hardware MMU 3-level page table walk in software.
- **Parameters**:
  - `pagetable`: Root Level-2 page table pointer.
  - `va`: Virtual address to look up.
  - `alloc`: If `1`, allocates missing intermediate page directories via `kalloc()`. If `0`, inspects only existing structures.
- **Return Value**: Pointer to Level-0 PTE, or `0` if unmapped/allocation failure.
- **Step-by-Step Logic**:
  1. Validates `va < MAXVA`.
  2. Loops from `level = 2` down to `level = 1`:
     - Extracts 9-bit index: `PX(level, va) = (va >> (12 + 9*level)) & 0x1FF`.
     - Inspects `pte = &pagetable[index]`.
     - If entry is valid (`*pte & PTE_V`), follows pointer: `pagetable = (pagetable_t)PTE2PA(*pte)`.
     - If invalid:
       - If `alloc == 0`, returns `0`.
       - If `alloc == 1`, calls `kalloc()`, zeroes the new page directory page, and writes `*pte = PA2PTE(pagetable) | PTE_V`.
  3. Returns `&pagetable[PX(0, va)]` (the Level-0 leaf PTE).

#### 2. `int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)`
- **Purpose**: Installs PTE mappings in `pagetable` for virtual range `[va, va+size)` pointing to physical range `[pa, pa+size)`.
- **Logic**: For each 4KB page in the range:
  - Calls `pte = walk(pagetable, a, 1)`.
  - Panics if `*pte & PTE_V` (re-mapping an already valid virtual page without unmapping).
  - Installs entry: `*pte = PA2PTE(pa) | perm | PTE_V`.
  - Advances `a += PGSIZE` and `pa += PGSIZE`.

#### 3. `void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)`
- **Purpose**: Unmaps `npages` starting at `va`. If `do_free == 1`, calls `kfree()` on the underlying physical RAM.
- **Logic**: Iterates through pages. Calls `walk(pagetable, a, 0)`. If leaf PTE exists and is valid, frees the physical page if requested, and clears `*pte = 0`. Skips unmapped lazy pages gracefully without panicking.

#### 4. `int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)`
- **Purpose**: Duplicates parent process's memory space into a new child process during `kfork()`.
- **Logic**: Iterates from `0` to `sz`. Looks up PTE via `walk(old, i, 0)`. If valid, allocates a fresh physical page via `kalloc()`, copies data via `memmove()`, and maps it into `new` with identical permission flags.

#### 5. `int copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, char *src, uint64 len)`
- **Purpose**: Safely copies `len` bytes from kernel buffer `src` into user virtual address `dstva`.
- **cOS Repository-Specific Feature**: Features a 5-argument signature accepting `psz` (`p->sz`). If `dstva` maps to a lazily allocated page, `copyout` automatically invokes `vmfault(pagetable, psz, va0, 0)` to map physical RAM on demand! Validates write permissions (`*pte & PTE_W`) and aborts if writing to read-only code.

#### 6. `uint64 vmfault(pagetable_t pagetable, uint64 psz, uint64 va, int read)`
- **Purpose**: Demand-paging fault resolution handler.
- **Logic**: If `va < psz`, allocates a physical page via `kalloc()`, zeroes it, and maps it at `PGROUNDDOWN(va)` with `PTE_W | PTE_U | PTE_R`. Returns physical address, or 0 on failure.

---

# Chapter 5: Process Management, Lifecycle, & Context Switching

### 5.1 The Process Abstraction: Process Control Block (`struct proc`)
In cOS, a process is an isolated executing program. The kernel tracks processes via an array of 64 Process Control Blocks (`proc[NPROC]` in `kernel/proc.h`):

```c
struct proc {
  struct spinlock lock;         // Protects process state transitions
  enum procstate state;         // UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
  void *chan;                   // If non-zero, sleeping on this channel
  int killed;                   // If non-zero, has been killed
  int xstate;                   // Exit status returned to parent wait()
  int pid;                      // Process ID
  struct proc *parent;          // Parent process pointer
  uint64 kstack;                // Virtual address of kernel stack
  uint64 sz;                    // Virtual memory size in bytes
  pagetable_t pagetable;        // User Sv39 page table
  struct trapframe *trapframe;  // Saved user registers during traps
  struct context context;       // swtch() callee-saved registers
  struct file *ofile[NOFILE];   // Open file descriptors
  struct inode *cwd;            // Current working directory
  char name[16];                // Process command name (for diagnostics)

  // cOS Custom Telemetry Counters
  uint64 cpu_ticks;             // CPU timer ticks accumulated
  uint64 ctx_switches;          // Total context switches in sched()
  uint64 page_faults;           // Demand-paging page faults handled
  uint64 total_syscalls;        // Total system calls executed
  uint32 syscall_counts[32];    // Frequency histogram per system call number
};
```

### 5.2 Process State Machine

```
               allocproc()
  +--------+ ------------> +------+ 
  | UNUSED |               | USED | 
  +--------+ <------------ +------+ 
      ^       freeproc()       |
      |                        | userinit() / kfork()
      |                        v
      |                  +----------+
      |    yield() /     |          |
      |   timer trap     | RUNNABLE | <------------------+
      |  +-------------- |          |                    |
      |  |               +----------+                    |
      |  |                     |                         |
      |  v                     | scheduler()             |
      | +---------+            v                         |
      | | RUNNING | <----+ swtch()                       |
      | +---------+      |                               |
      |   |       |      +-------------------------------+
      |   |       |                 wakeup()
      |   | sleep()
      |   v       |
      | +----------+
      | | SLEEPING |
      | +----------+
      |
      | kexit()
      v
  +--------+
  | ZOMBIE | (Waiting for parent kwait() to call freeproc())
  +--------+
```

### 5.3 Process Lifecycle Operations: `kernel/proc.c`

#### 1. `void procinit(void)`
- Initializes `pid_lock`, `wait_lock`, and the 64 process locks `p->lock`. Sets each `p->kstack = KSTACK(i)`.

#### 2. `struct proc *allocproc(void)`
- Scans `proc[]` for `UNUSED`. Allocates new PID under `pid_lock`.
- Allocates a physical trapframe page.
- Allocates an empty user page table (`proc_pagetable`).
- Initializes context: `p->context.ra = (uint64)forkret;`, `p->context.sp = p->kstack + PGSIZE;`.
- **Wipes cOS Telemetry**: Sets `cpu_ticks = 0`, `ctx_switches = 0`, `page_faults = 0`, `total_syscalls = 0`, and clears `syscall_counts[32]`.

#### 3. `void freeproc(struct proc *p)`
- Releases trapframe page, calls `proc_freepagetable` to destroy user virtual memory, clears PID/descriptors, resets telemetry counters, and transitions state to `UNUSED`.

#### 4. `int kfork(void)`
- Clones caller into child: allocates PCB via `allocproc()`, copies address space via `uvmcopy()`, duplicates user trapframe registers (`np->trapframe->a0 = 0` so fork returns 0 in child), duplicates open file references (`filedup`), reparents child, and sets state to `RUNNABLE`. Returns child PID to parent.

#### 5. `void kexit(int status)`
- Closes open files, relinquishes cwd reference, reparents active children to `initproc`, awakens waiting parent (`wakeup(p->parent)`), stores exit status `p->xstate = status`, sets state to `ZOMBIE`, and calls `sched()` to surrender the CPU permanently.

#### 6. `int kwait(uint64 addr)`
- Scans `proc[]` for children. If an exited child is in `ZOMBIE` state, copies exit status to user address `addr`, calls `freeproc()`, and returns child PID. If children are still active, sleeps on `p` until a child exits.

### 5.4 Low-Level Context Switch: `kernel/swtch.S`

#### Responsibility
`swtch.S` switches execution between two kernel threads (e.g. between a process kernel thread and the per-core `scheduler()` loop).

```assembly
.globl swtch
swtch:
        sd ra, 0(a0)
        sd sp, 8(a0)
        sd s0, 16(a0)
        sd s1, 24(a0)
        sd s2, 32(a0)
        sd s3, 40(a0)
        sd s4, 48(a0)
        sd s5, 56(a0)
        sd s6, 64(a0)
        sd s7, 72(a0)
        sd s8, 80(a0)
        sd s9, 88(a0)
        sd s10, 96(a0)
        sd s11, 104(a0)

        ld ra, 0(a1)
        ld sp, 8(a1)
        ld s0, 16(a1)
        ld s1, 24(a1)
        ld s2, 32(a1)
        ld s3, 40(a1)
        ld s4, 48(a1)
        ld s5, 56(a1)
        ld s6, 64(a1)
        ld s7, 72(a1)
        ld s8, 80(a1)
        ld s9, 88(a1)
        ld s10, 96(a1)
        ld s11, 104(a1)
        
        ret
```

#### Why Only 14 Registers?
Notice that caller-saved registers (`a0..a7`, `t0..t6`) are **not** saved here! Because `swtch` is called as a standard C function (`swtch(&c->context, &p->context)`), the C compiler automatically spills any caller-saved registers it cares about onto the C call stack before the call. Only callee-saved registers (`s0..s11`, `sp`, `ra`) must be manually preserved. When `ret` executes, it jumps to the restored `ra`, seamlessly continuing execution inside the new thread!

### 5.5 Two-Phase Sleep and Wakeup Synchronization
To prevent race conditions where a `wakeup()` occurs between a process deciding to sleep and actually setting its state to `SLEEPING` (the classic lost-wakeup problem), cOS uses a two-phase sleep protocol:
1. `sleep_prepare(chan)`: Sets `p->chan = chan` while holding `p->lock`.
2. The subsystem releases its external resource lock.
3. `sleep()`: Verifies `p->chan != 0`. If another core woke the channel during step 2, `p->chan` would already be cleared, and `sleep()` avoids going to sleep! Otherwise, sets state to `SLEEPING` and calls `sched()`.

---

# Chapter 6: Traps, Interrupts, & Exception Handling

### 6.1 The Trap Mental Model
*Analogy*: Imagine you are writing an essay in a library. Suddenly, an alarm rings (interrupt), or you realize you need a restricted book from the rare archives (system call), or your pen runs out of ink (page fault). You cannot simply continue writing. You bookmark your exact page and line, pack your notes into a locker, walk over to the librarian's desk, let the librarian handle your request, return to your desk, unpack your notes, and resume writing as if nothing happened.

A **trap** in cOS is that exact sequence:
1. User application state (all 32 registers) is saved into `TRAPFRAME`.
2. CPU switches to Supervisor mode and loads the kernel page table.
3. Kernel identifies the trap cause (`scause`) and executes the handler.
4. User registers are restored from `TRAPFRAME`.
5. CPU executes `sret`, returning to User mode at `sepc`.

### 6.2 The Dual-Mapped Trampoline Page: `kernel/trampoline.S`
When a trap occurs from user space, hardware switches privilege to S-mode, but **it does not switch the page table (`satp`)**. The processor must execute instructions in S-mode while still using the user's page table.

**The Trampoline Solution**:
`TRAMPOLINE` is a single physical page containing `uservec` and `userret`. It is mapped at the identical virtual address (`MAXVA - PGSIZE`) in **both** every user page table and the kernel page table. When `satp` is swapped, the instruction pointer (`pc`) does not fault because the executing code is mapped at the exact same virtual address in both tables!

```assembly
uservec:
    csrw sscratch, a0             # Stash user a0 into sscratch CSR
    li a0, TRAPFRAME              # a0 = 0x3fffffe000
    sd ra, 40(a0)                 # Save user registers ra, sp, gp, tp, etc.
    ...
    csrr t0, sscratch             # Retrieve user a0
    sd t0, 112(a0)                # Save user a0 to trapframe->a0
    ld sp, 8(a0)                  # Load kernel stack pointer from trapframe
    ld tp, 32(a0)                 # Load hartid into tp
    ld t0, 16(a0)                 # Load usertrap address
    ld t1, 0(a0)                  # Load kernel page table (satp)
    sfence.vma zero, zero
    csrw satp, t1                 # Switch to kernel page table!
    sfence.vma zero, zero
    jalr t0                       # Branch into usertrap() in trap.c
```

### 6.3 Trap Dispatching: `kernel/trap.c`

#### Core Functions

#### 1. `uint64 usertrap(void)`
- Invoked by `uservec` in `kernel/trampoline.S` when a trap occurs from user mode.
- Sets `stvec` to `kernelvec` (so traps inside the kernel are handled by `kerneltrap`).
- Saves user PC: `p->trapframe->epc = r_sepc()`.
- Inspects `scause`:
  - **System Call (`scause == 8`)**:
    - Advances user PC past the 4-byte `ecall` instruction: `p->trapframe->epc += 4`.
    - Enables interrupts via `intr_on()`.
    - Calls `syscall()`.
  - **Device Interrupt (`which_dev = devintr()`)**:
    - If timer interrupt (`which_dev == 2`):
      - Increments process tick counter: `p->cpu_ticks++`.
      - Preempts current process: `yield()`.
  - **Page Fault (`scause == 13` [Load] or `15` [Store])**:
    - Handles lazy demand-paging:
      ```c
      if (vmfault(p->pagetable, p->sz, r_stval(), (r_scause() == 13) ? 1 : 0) != 0) {
        p->page_faults++;
      } else {
        setkilled(p);
      }
      ```
      If `vmfault` maps a page successfully, `p->page_faults++` records the telemetry event!
- **Return Mechanism & Trampoline Transition**:
  - Calls helper `prepare_return()` (`kernel/trap.c:104`), which turns off interrupts (`intr_off()`), sets `stvec` back to `trampoline_uservec`, configures kernel trapframe parameters (`kernel_satp`, `kernel_sp`, `kernel_trap`, `kernel_hartid`), sets S Previous Privilege mode in `sstatus` to User mode with interrupts enabled, and writes `p->trapframe->epc` into `sepc`.
  - Computes the user page table token `uint64 satp = MAKE_SATP(p->pagetable)` and returns `satp` (`return satp;`). Under the RISC-V calling convention (ABI), this 64-bit return value is passed back in register `a0`.
  - Because `uservec` in `kernel/trampoline.S` invoked `usertrap()` via `jalr t0` immediately before the `.globl userret` label, returning from `usertrap()` branches directly into `userret:`.
  - In `userret:`, register `a0` holds the user `satp`, which is immediately installed into the MMU via `csrw satp, a0` to restore the user virtual address space. Trampoline assembly then restores all general-purpose registers from `TRAPFRAME` and executes `sret` to resume user execution.

### 6.4 Kernel-Mode Traps: `kernel/kernelvec.S`
When an interrupt (such as a timer tick) occurs while the CPU is *already* executing inside the kernel in S-mode, hardware traps to `kernelvec`. `kernelvec.S` pushes all registers onto the current kernel stack, calls `kerneltrap()`, restores registers, and executes `sret`.

---

# Chapter 7: The System Call Subsystem

### 7.1 The Boundary Between User and Kernel
User programs cannot access kernel functions directly. Instead, they place the system call number in register `a7`, parameters in `a0..a5`, and execute the `ecall` instruction.

```
User Program                  Hardware / Trampoline               cOS Kernel
+-------------+               +-------------------+              +----------------+
| procinfo(0) | --(ecall)--> | uservec: Save Regs | -----------> | usertrap()     |
| a7 = 23     |               | Switch SATP       |              | syscall()      |
| a0 = 0      |               +-------------------+              | sys_procinfo() |
+-------------+                                                  +-------+--------+
      ^                                                                  |
      |                       +-------------------+                      |
      +-------(sret)--------- | userret: Rest.Regs| <--------------------+
       Return value in a0     | Switch SATP       |   Result in p->trapframe->a0
                              +-------------------+
```

### 7.2 System Call Numbering: `kernel/syscall.h`
cOS assigns unique integer identifiers to every system call:
```c
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_pause  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_sync   22
#define SYS_procinfo 23   // Custom Telemetry System Call
```

> **Important Distinction: `SYS_sync` (Syscall 22) vs. `sbrklazy`**:
> - **Syscall 22 (`SYS_sync`)**: As defined in `kernel/syscall.h:23`, system call number 22 is `#define SYS_sync 22`, which dispatches to `sys_sync()` in the kernel (`kernel/syscall.c:133`) to flush dirty buffer cache blocks to disk.
> - **`sbrklazy(n)` User-Space Implementation**: Lazy heap memory allocation is **not** an independent system call number. Instead, `sbrklazy(int n)` is implemented as a user-space library helper function in `user/ulib.c:159`:
>   ```c
>   char *
>   sbrklazy(int n)
>   {
>     return sys_sbrk(n, SBRK_LAZY);
>   }
>   ```
>   Here, `sbrklazy()` invokes the standard `sbrk` system call (`SYS_sbrk`), passing the allocation mode flag `SBRK_LAZY` (in contrast to eager `sbrk(n)`, which passes `SBRK_EAGER`). Inside `kernel/sysproc.c:40`, `sys_sbrk()` extracts this mode argument via `argint(1, &t)`. When `t == SBRK_LAZY`, the kernel merely expands the process virtual size `myproc()->sz += n` without eagerly allocating physical RAM pages, deferring allocation to demand-paging page faults handled by `usertrap()`.

### 7.3 Dispatching and Argument Extraction: `kernel/syscall.c`

#### Argument Extraction Helpers
User arguments are retrieved from the saved `trapframe`:
- `argraw(n)`: Retrieves register `a0` through `a5` from `p->trapframe`.
- `argint(n, &val)`: Casts `argraw(n)` to a 32-bit signed integer.
- `argaddr(n, &ptr)`: Casts `argraw(n)` to a 64-bit user virtual memory address.
- `argstr(n, buf, max)`: Reads pointer with `argaddr()` and uses `copyinstr()` to safely read a null-terminated string from user space into kernel buffer `buf`.

#### The `syscall()` Dispatcher
```c
void syscall(void) {
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->total_syscalls++;
    if (num < 32)
      p->syscall_counts[num]++;
    p->trapframe->a0 = syscalls[num]();
  } else {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
```
**Telemetry Tracking**: On every valid invocation, `p->total_syscalls++` and `p->syscall_counts[num]++` record the exact frequency of system call usage!

### 7.4 File and Process System Calls: `kernel/sysfile.c` and `kernel/sysproc.c`
Standard system calls are divided between:
- `kernel/sysfile.c`: File system operations (`sys_open`, `sys_read`, `sys_write`, `sys_close`, `sys_fstat`, `sys_pipe`).
- `kernel/sysproc.c`: Process control and accounting (`sys_fork`, `sys_exit`, `sys_wait`, `sys_sbrk`, `sys_uptime`, and `sys_procinfo`).

---

# Chapter 8: Custom Feature Deep-Dive: `sys_procinfo` (Syscall #23)

### 8.1 Motivation in Quantitative Systems Engineering
In high-frequency trading (HFT) and quantitative research, software interacts with bare-metal hardware under microsecond SLAs. Engineers must know:
- Did this trading thread suffer unexpected context switches?
- Did lazy memory allocation trigger unexpected TLB shootdowns or hardware page faults?
- What is the process's true physical memory footprint (Resident Set Size), rather than merely requested virtual size?

`sys_procinfo` provides an atomic, zero-overhead telemetry query answering these questions directly from the cOS kernel.

### 8.2 Data Structure: `struct proc_info` (All 11 Fields Explained)
Defined in `kernel/proc.h` and exposed to user applications via `user/user.h`:

```c
struct proc_info {
  int pid;                      // Process ID
  int ppid;                     // Parent Process ID
  int state;                    // Current lifecycle state (UNUSED, SLEEPING, etc.)
  uint64 sz;                    // Virtual memory size in bytes (p->sz)
  uint64 rss;                   // Resident Set Size (physical bytes currently mapped)
  uint64 cpu_ticks;             // CPU timer ticks accumulated while running
  uint64 ctx_switches;          // Voluntary & involuntary context switches
  uint64 page_faults;           // Demand-paging page faults handled
  uint64 total_syscalls;        // Cumulative system calls executed by process
  uint32 syscall_counts[32];    // Frequency histogram per system call number
  char name[16];                // Process command name (e.g. "testsyscall")
};
```

| Field | Type | Description & Quantitative Systems Value |
|---|---|---|
| `pid` | `int` | Process ID. If caller passes `pid == 0`, `sys_procinfo` queries the caller itself (`myproc()`). |
| `ppid` | `int` | Parent Process ID. Verifies process tree hierarchy and process isolation. |
| `state` | `int` | Current execution state (`enum procstate`: 3 = `RUNNABLE`, 4 = `RUNNING`). |
| `sz` | `uint64` | Total virtual memory size requested via ELF loading or `sbrk()`. |
| `rss` | `uint64` | True **Resident Set Size**: physical DRAM bytes actually mapped in Sv39 page table. |
| `cpu_ticks` | `uint64` | Number of 100ms hardware timer ticks the process spent running on a CPU core. |
| `ctx_switches` | `uint64` | Total context switches executed by `sched()`. Measures thread preemption. |
| `page_faults` | `uint64` | Number of lazy allocation page faults resolved by `vmfault()`. |
| `total_syscalls`| `uint64` | Cumulative total of system calls invoked over process lifetime. |
| `syscall_counts`| `uint32[32]` | Per-syscall execution histogram. Allows tracking exact API consumption. |
| `name` | `char[16]` | Name of the process executable, copied cleanly via `safestrcpy()`. |

### 8.3 Kernel Telemetry Instrumentation Points
To populate `struct proc_info`, telemetry hooks were embedded into core kernel paths:
1. **Zeroing at Lifecycle Boundaries** (`kernel/proc.c` in `allocproc` and `freeproc`):
   Guarantees clean counters for every new process and prevents data leaks upon PID reuse.
2. **Context Switch Accounting** (`kernel/proc.c` in `sched()` line 506):
   `p->ctx_switches++;` increments on every switch out of a process (voluntary yields and involuntary preemptions).
3. **Timer Tick Accounting** (`kernel/trap.c` in `usertrap()` line 87):
   When a hardware timer interrupt occurs (`which_dev == 2`) while the process is executing, `p->cpu_ticks++;` records execution time.
4. **Demand-Paging Faults** (`kernel/trap.c` in `usertrap()` line 75):
   When a load/store fault occurs and `vmfault()` successfully maps a physical page, `p->page_faults++;` records the fault.
5. **System Call Histogram** (`kernel/syscall.c` in `syscall()` line 147):
   `p->total_syscalls++;` and `p->syscall_counts[num]++;` increment on every system call dispatch.

### 8.4 Sv39 Resident Set Size (RSS) Calculation Algorithm: `calc_rss`
In an OS with lazy allocation (`sbrklazy`), virtual size `sz` does not represent actual physical RAM consumption. `calc_rss` walks the Sv39 page directory in software:

```c
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
```

#### Step-by-Step Logic:
1. Iterates through the virtual address space from `va = 0` to `sz` in steps of `PGSIZE` ($4096\text{ bytes}$).
2. Calls `walk(pagetable, va, 0)`:
   - Passing `alloc = 0` is crucial: it traverses the existing 3-level tree without allocating missing directory pages.
3. Filters entries:
   - `pte != 0`: A Level-0 entry exists.
   - `*pte & PTE_V`: The entry is Valid (points to physical DRAM).
   - `*pte & PTE_U`: The entry is User accessible (excludes kernel trampolines and guard pages).
4. For every qualifying page, adds `PGSIZE` ($4096$) to `rss`.

### 8.5 Concurrency Discipline & Spinlock Order Safety

#### The Implementation in `kernel/sysproc.c`:
```c
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

  // Snapshot telemetry safely under lock
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

  // Calculate RSS via Sv39 walk
  kinfo.rss = calc_rss(target->pagetable, target->sz);

  // CRITICAL: Release spinlock BEFORE copyout!
  release(&target->lock);

  // 5-argument copyout for demand-paging compliance
  if (copyout(curr->pagetable, curr->sz, uaddr, (char *)&kinfo, sizeof(kinfo)) < 0)
    return -1;

  return 0;
}
```

#### Why Release `target->lock` Before `copyout`?
This is one of the most critical concurrency invariants in kernel engineering:
- `copyout()` writes data into user memory.
- If the user buffer is located on a lazily-allocated page, `copyout()` triggers `vmfault()`.
- `vmfault()` calls `kalloc()` to allocate a physical DRAM frame.
- In a busy multi-core kernel, `kalloc()` might need to acquire memory locks or wait for memory.
- **Holding a spinlock while performing an operation that could sleep or acquire other locks causes lock inversion and deadlocks!**
- By snapshotting the telemetry into local stack struct `kinfo` and releasing `target->lock` before `copyout()`, cOS guarantees complete deadlock immunity.

### 8.6 User-Space Interface & Test Suite: `user/testsyscall.c`

#### User Stub
In `user/user.h`:
```c
int procinfo(int pid, struct proc_info *info);
```
Generated via `user/usys.pl`, which ecustoms assembly:
```assembly
.global procinfo
procinfo:
 li a7, SYS_procinfo
 ecall
 ret
```

#### Verification Scenarios in `user/testsyscall.c`
The `testsyscall` test program validates six comprehensive scenarios:
1. **`test_basic` (Self-Query)**:
   - Queries `procinfo(0, &info)` and `procinfo(getpid(), &explicit)`.
   - Confirms `info.pid == getpid()`, `info.name == "testsyscall"`, $0 < \text{rss} \le \text{sz}$, and `syscall_counts[SYS_procinfo] >= 1`.
2. **`test_init_query` (Init Process Inspection)**:
   - Queries `procinfo(1, &info)`.
   - Verifies PID 1 exists, has name `"init"`, and positive virtual and physical memory.
3. **`test_syscall_counting` (Deterministic Delta Counting)**:
   - Takes baseline telemetry snapshot.
   - Executes exactly 5 `uptime()` calls and 3 `getpid()` calls.
   - Re-queries telemetry: confirms `delta(uptime) == 5`, `delta(getpid) == 3`, and `delta(total) >= 9`.
4. **`test_lazy_pagefaults` (Demand Paging & RSS Growth)**:
   - Allocates 3 pages via `sbrklazy(3 * 4096)`.
   - Verifies virtual size expands by 12,288 bytes, but `page_faults` does **not** increase before memory access.
   - Writes to each page, triggering hardware faults.
   - Confirms `delta(page_faults) == 3` and RSS grows by at least 12,288 bytes.
5. **`test_invalid_args` (Boundary & Fault Tolerance)**:
   - Supplies negative PIDs (`-1`, `-999`), non-existent PIDs (`30000`), NULL pointer (`0`), kernel addresses (`0x80000000`), trapframe addresses (`0x3fffffe000`), and addresses beyond `MAXVA` (`0x4000000000`).
   - Confirms every invalid call returns `-1` cleanly without triggering kernel panics.
6. **`test_fork_inheritance` (Hierarchy & Stat Isolation)**:
   - Parent forks a child. Child queries `procinfo(0, &cinfo)`.
   - Confirms `cinfo.ppid == parent_pid`, `cinfo.pid == getpid()`, and child telemetry counters are independent of parent counters.

---

# Chapter 9: Building, Running, and Debugging cOS

### 9.1 Toolchain Prerequisites
To compile and execute cOS, your environment requires:
- **RISC-V GNU Compiler Toolchain**: `riscv64-unknown-elf-gcc` (or `riscv64-linux-gnu-gcc`), `binutils`, and `gdb`.
- **QEMU Emulator**: `qemu-system-riscv64` (version 5.0 or newer).
- **Build Tools**: GNU `make`.

### 9.2 Building and Booting in QEMU
In the root directory of the cOS source tree (`cOS`):

```bash
# Clean previous build artifacts
make clean

# Compile the kernel and build user filesystem image (fs.img)
make

# Boot cOS in QEMU with 3 virtual CPU cores and 128MB RAM
make qemu
```

Upon launching, QEMU ecustoms the custom cOS ASCII art splash screen, initializes all 17 subsystems on Hart 0, brings up Harts 1 and 2, and displays the interactive shell prompt:

```
            /$$$$$$   /$$$$$$ 
           /$$__  $$ /$$__  $$
  /$$$$$$$| $$  \ $$| $$  \__/
 /$$_____/| $$  | $$|  $$$$$$ 
| $$      | $$  | $$ \____  $$
| $$      | $$  | $$ /$$  \ $$
|  $$$$$$$|  $$$$$$/|  $$$$$$/
 \_______/ \______/  \______/ 

cOS kernel is booting

[kinit] Initializing physical memory allocator...
[kvminit] Creating kernel page table at kernel base...
[kvminithart] Enabling SV39 hardware paging...
[procinit] Initializing process control blocks...
[trapinit] Setting up trap vectors and exception handlers...
[trapinithart] Installing kernel trap vector to stvec...
[plicinit] Configuring PLIC...
[plicinithart] Routing UART and VIRTIO interrupts to hart 0...
[binit] Allocating memory for disk buffer cache...
[iinit] Initializing inode directory cache...
[fileinit] Setting up global file descriptor table...
[virtio_disk_init] Probing virtio-mmio bus for block devices...
[userinit] Handing off control to first user process (init)...
hart 1 starting
hart 2 starting
init: starting sh
$ 
```

### 9.3 Running `testsyscall` and Interpreting Diagnostics
At the shell prompt `$ `, execute the test suite:
```bash
$ testsyscall
```

Expected diagnostic output:
```
========================================
   cos tests   
   Target: procinfo (Syscall #23)       
========================================
test_basic: starting self-query test...
test_basic: PASS (pid=3, name=testsyscall, sz=16384, rss=16384, syscalls=3)
test_init_query: querying init process (pid=1)...
test_init_query: PASS (init pid=1, name=init, sz=12288, rss=12288)
test_syscall_counting: verifying deterministic syscall tracking...
test_syscall_counting: PASS (uptime delta=5, getpid delta=3, total delta=9)
test_lazy_pagefaults: verifying lazy memory allocation and page fault counts...
test_lazy_pagefaults: PASS (faults before=0, after=3, delta=3, rss delta=12288)
test_invalid_args: verifying fault tolerance on invalid inputs...
test_invalid_args: PASS (all error conditions returned -1 cleanly without panicking)
test_fork_inheritance: testing child process hierarchy and isolation...
test_fork_inheritance: PASS (child correctly verified ppid=3 and isolated stats)
========================================
all tests passed
tests completed
========================================
```

### 9.4 Kernel Debugging Techniques (GDB, procdump)
1. **Interactive Process Dump (`Ctrl-P`)**:
   Pressing `Ctrl-P` in the terminal triggers `consoleintr()`, which invokes `procdump()`. This dumps the PID, state (`RUNNING`, `SLEEPING`), command name, and call stack program counter of every active process to the console.
2. **GDB Remote Debugging**:
   Run QEMU in debug mode:
   ```bash
   make qemu-gdb
   ```
   In a separate terminal, attach the RISC-V debugger:
   ```bash
   riscv64-unknown-elf-gdb kernel/kernel
   (gdb) target remote localhost:26000
   (gdb) break sys_procinfo
   (gdb) continue
   ```
3. **Exiting QEMU**:
   Press `Ctrl-A` followed by `X` to terminate QEMU cleanly.

---

## Summary & Conclusion
cOS bridges academic operating system design and production systems engineering. By combining the elegance of a highly optimized RISC-V kernel with industrial telemetry instrumentation (`sys_procinfo`), software page table inspection (`calc_rss`), and demand-paging tracking, cOS demonstrates the exact low-level mechanics governing modern operating systems.

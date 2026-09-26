
## A Custom 32-Bit x86 Operating System Built From Scratch

ValaOS is a custom 32-bit x86 operating system developed from the ground up using **C and x86 Assembly**.

The project focuses on understanding how operating systems work internally by implementing core components such as memory management, paging, interrupts, scheduling, user-mode execution, system calls, persistent storage, filesystems, and program execution.

> **ValaOS is not built to replace modern operating systems. It is built to understand them.**

🌐 **Website:**  
https://nehalvala0005-cloud.github.io/ValaOS/

💿 **Download ValaOS 2.0:**  
https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-2.0.0.iso

---

# What is ValaOS?

ValaOS is a small but functional operating system environment designed to explore low-level computer architecture and operating system development.

Instead of using an existing kernel, ValaOS implements its own kernel-level components and connects them together into a bootable system.

At a high level, the system follows this flow:

```text
GRUB
  ↓
Bootloader
  ↓
Protected Mode
  ↓
Kernel
  ↓
Memory Management
  ↓
Paging & Virtual Memory
  ↓
Interrupts & Timer
  ↓
Task Scheduler
  ↓
User Mode
  ↓
System Calls
  ↓
Virtual Filesystem
  ↓
Program Loader
  ↓
User Programs
Why did I build ValaOS?

The main reason behind ValaOS is simple:

I wanted to understand what happens underneath the software that we normally take for granted.

Instead of only learning operating system concepts theoretically, ValaOS turns those concepts into actual implementations.

Through this project, concepts such as:

CPU privilege levels
Memory management
Virtual memory
Paging
Interrupts
Scheduling
Context switching
System calls
Filesystems
Disk I/O
User-mode execution

are implemented and connected inside one system.

The project therefore acts as a practical exploration of computer architecture and operating system internals.

What can ValaOS actually do?

ValaOS currently provides a command-line operating environment with several core operating system capabilities.

It can:

Boot through GRUB
Run a custom kernel
Operate in 32-bit protected mode
Manage physical memory
Enable paging
Manage virtual memory
Handle page faults
Configure hardware interrupts
Generate hardware timer ticks
Run a basic task scheduler
Maintain task/process IDs
Execute code in Ring 3
Handle system calls
Create and manage files
Create directories
Navigate through directories
Persist filesystem data on disk
Load programs from the filesystem
Execute user programs
Display processes/tasks
Monitor system uptime
Run subsystem tests
Perform an integrated system self-test
What makes ValaOS different from a simple OS demo?

ValaOS is not just a kernel that prints text on the screen.

The project connects multiple operating-system subsystems together.

For example:

Hardware Timer
      ↓
Interrupt
      ↓
Scheduler
      ↓
Task Context
      ↓
Next Task

And user programs follow another complete path:

Filesystem
     ↓
Program Loader
     ↓
User Memory
     ↓
Ring 3
     ↓
System Call
     ↓
Kernel
     ↓
Operation

The objective is to make the components work together rather than keeping them as isolated experiments.

What is the current version of ValaOS?

The current release is:

ValaOS 2.0.0

ValaOS 2.0 represents a major expansion from the initial prototype.

Major additions include:

Paging
Virtual memory
Page fault handling
Hardware timer
Timer-driven scheduling
Task execution contexts
Private task stacks
Ring 3 user-mode execution
System calls
User task termination
PID-based task management
Persistent VFS
Hierarchical directories
Disk-backed storage
Filesystem-backed program loading
Generic program execution
Program listing
Process listing
System self-test
How does ValaOS boot?

The boot process begins with GRUB loading the ValaOS kernel.

The simplified boot sequence is:

GRUB
 ↓
Load Kernel
 ↓
CPU Setup
 ↓
GDT Initialization
 ↓
Protected Mode
 ↓
Kernel Entry
 ↓
Memory Initialization
 ↓
Paging
 ↓
Interrupt Initialization
 ↓
Timer Initialization
 ↓
Scheduler
 ↓
Filesystem
 ↓
Shell

The bootloader and low-level CPU initialization are implemented using x86 Assembly.

What architecture does ValaOS use?

ValaOS currently targets:

Architecture: 32-bit x86
Execution Environment: QEMU
Kernel: Custom
Bootloader: GRUB

The current implementation is intentionally 32-bit.

A future version can move toward x86_64 after the underlying kernel architecture is sufficiently mature.

How does ValaOS manage memory?

ValaOS contains its own basic memory management subsystem.

The memory manager keeps track of memory pages and provides information such as:

Total Pages
Used Pages
Page Size

The current page size is:

4096 bytes

Memory-related functionality can also be tested through commands available inside the ValaOS shell.

What is paging in ValaOS?

ValaOS implements x86 paging to introduce virtual memory.

Paging allows the operating system to control how virtual addresses are mapped to physical memory.

The paging subsystem contains:

Page directories
Page tables
Page mappings
Page flags
User-accessible pages
Supervisor-only pages

The project also contains dedicated paging tests to verify the subsystem.

How does ValaOS provide virtual memory?

Virtual memory allows software to work with virtual addresses instead of directly depending on physical memory addresses.

ValaOS uses separate virtual memory regions for user programs.

The current user program layout includes:

User Code
0x00400000

User Stack
0x00800000

The paging system uses permission flags to separate user-accessible memory from kernel-only memory.

This provides the foundation for protected user-mode execution.

What happens when a page fault occurs?

ValaOS contains page fault handling for invalid memory accesses.

When the processor detects a page fault:

Invalid Memory Access
        ↓
CPU raises Page Fault
        ↓
Interrupt Handler
        ↓
Page Fault Handler
        ↓
Fault Information

The system can obtain the address that caused the fault through the CPU's page-fault information register.

Page fault handling is an important part of the virtual memory subsystem.

How does ValaOS handle interrupts?

ValaOS uses the x86 Interrupt Descriptor Table (IDT) to register interrupt handlers.

The interrupt subsystem currently supports important mechanisms including:

Hardware timer interrupts
System call interrupts

The Programmable Interrupt Controller (PIC) is configured during initialization.

Why does ValaOS need a hardware timer?

The hardware timer provides periodic interrupts to the kernel.

These timer ticks are used by the scheduler.

The simplified flow is:

Hardware Timer
      ↓
Timer Interrupt
      ↓
Timer Handler
      ↓
Scheduler
      ↓
Task Selection
      ↓
Context Switch

ValaOS also maintains a timer tick counter that can be inspected from the shell.

How does the ValaOS scheduler work?

ValaOS contains a basic timer-driven scheduler.

Each task maintains information such as:

PID
Name
State
Stack
Execution Context

Supported states include:

READY
RUNNING
BLOCKED
TERMINATED

The scheduler uses timer interrupts to determine when task execution should change.

Each task can have its own execution context and private stack.

What tasks currently exist in ValaOS?

The current task system contains basic system tasks such as:

kernel
shell
idle

The scheduler maintains their execution state and context.

The shell also provides commands for inspecting task information.

Does ValaOS support user-mode execution?

Yes.

One of the important milestones of ValaOS 2.0 is the introduction of Ring 3 user-mode execution.

The architecture separates:

Ring 0 → Kernel
Ring 3 → User Programs

The kernel operates with higher privileges while user programs execute with restricted privileges.

This establishes the foundation for process isolation and protected user applications.

Why is Ring 3 important?

Running everything inside the kernel would mean that a program could potentially operate with kernel-level privileges.

ValaOS instead separates user programs from the kernel:

        KERNEL
        Ring 0
           ↑
      System Calls
           ↑
      USER PROGRAM
        Ring 3

This separation is a fundamental operating system concept.

How do user programs communicate with the kernel?

User programs communicate with the kernel through system calls.

Currently ValaOS provides basic system-call functionality including:

SYS_WRITE
SYS_EXIT

The general flow is:

User Program
     ↓
System Call
     ↓
Kernel Handler
     ↓
Requested Operation
     ↓
Return to User Program
What does SYS_WRITE do?

SYS_WRITE allows a user program to request output through the kernel.

Instead of directly controlling kernel functionality, the user program asks the kernel to perform the operation.

This demonstrates the basic idea behind the system-call interface.

What does SYS_EXIT do?

SYS_EXIT allows a user program to terminate itself.

The flow is:

User Program
     ↓
SYS_EXIT
     ↓
Kernel
     ↓
Task Termination

ValaOS updates the user task state and returns control appropriately.

Does ValaOS have a filesystem?

Yes.

ValaOS contains a custom Virtual Filesystem (VFS).

The VFS supports:

Files
Directories
Parent-child relationships
File data
Directory navigation
File creation
File reading
File writing
File deletion
Is the VFS persistent?

Yes.

This is one of the important features of ValaOS 2.0.

The filesystem is backed by a virtual disk image rather than existing only in RAM.

The disk image is:

build/ValaOS.img

Filesystem information is stored in dedicated disk sectors.

This allows filesystem data to remain available across system restarts as long as the disk image is preserved.

Can ValaOS create directories?

Yes.

The shell provides:

mkdir

Directories can then be accessed using:

cd

and inspected using:

ls

The current working directory can be displayed using:

pwd
What filesystem commands are available?

ValaOS currently provides commands including:

pwd
cd
ls
mkdir
cat
touch
write
rm

These commands provide basic filesystem interaction directly from the ValaOS shell.

Can ValaOS execute programs stored on its filesystem?

Yes.

ValaOS contains a filesystem-backed user program loader.

A program can be stored as a binary file such as:

hello.bin
test.bin

The loader can:

Find Program
    ↓
Read Program From VFS
    ↓
Load Into User Memory
    ↓
Enter Ring 3
    ↓
Execute Program
How do I run a program in ValaOS?

ValaOS provides a generic command:

run <program>

For example:

run hello

or:

run test

The shell resolves the corresponding binary:

hello → hello.bin
test  → test.bin

This makes the program execution system reusable instead of hardcoding every individual program.

How can I see available programs?

Use:

programs

The command searches the filesystem for .bin program files and displays the available programs.

Example:

PROGRAMS
hello
test
Does ValaOS have a shell?

Yes.

ValaOS includes an interactive command-line shell.

The shell acts as the main interface between the user and the operating system.

It can be used to:

Inspect the system
Test memory
Test paging
Navigate the filesystem
Manage files
Inspect tasks
Run programs
Monitor uptime
Run diagnostics
Shut down or reboot the system
What commands are available?
System Information
info
about
version
uptime
Memory and Virtual Memory
memtest
memstress
meminfo
paging
ptest
dptest
vmtest
vmaptest
Filesystem
pwd
cd
ls
mkdir
cat
touch
write
rm
Process and Scheduler
ps
tasks
ticks
schedule
Programs
run <program>
programs
Hardware and System
disktest
reboot
shutdown
Diagnostics
selftest
How does ValaOS test its own subsystems?

ValaOS contains individual subsystem tests as well as an integrated self-test.

Individual tests cover areas such as:

Memory
Paging
Page Mapping
Virtual Memory
Disk
Scheduler

The integrated command is:

selftest
What does the ValaOS self-test check?

The self-test currently checks seven major components:

Memory
Paging
Disk
Virtual Memory
Scheduler
Program Loader
VFS

The current result is:

========== ValaOS 2.0 SELF TEST ==========

[ OK ] Memory
[ OK ] Paging
[ OK ] Disk
[ OK ] Virtual Memory
[ OK ] Scheduler
[ OK ] Program Loader
[ OK ] VFS

===========================================
Passed: 7
Failed: 0
SYSTEM STATUS: OK
===========================================

This provides a quick health check for the major implemented subsystems.

What is the architecture of ValaOS?

The current high-level architecture looks like this:

                    +------------------+
                    |      GRUB        |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |   Boot / Kernel  |
                    +--------+---------+
                             |
              +--------------+--------------+
              |              |              |
              v              v              v
         +---------+    +---------+    +---------+
         | Memory  |    | Paging  |    |Interrupt|
         +---------+    +---------+    +---------+
              |              |              |
              +--------------+--------------+
                             |
                             v
                    +------------------+
                    |    Scheduler     |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |   User Mode      |
                    |     Ring 3       |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |  System Calls    |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |       VFS        |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    | Persistent Disk  |
                    +------------------+
What technologies are used?
Area	Technology
Architecture	32-bit x86
Kernel	C
Low-level CPU code	x86 Assembly
Bootloader	GRUB
Assembly Compiler	NASM
Compiler	GCC
Linker	GNU ld
Build System	Make
Virtual Machine	QEMU
Version Control	Git
Repository	GitHub
Why use both C and Assembly?

C provides a practical language for implementing larger kernel components such as:

Memory management
Scheduler
VFS
Disk management
System calls

Assembly is used where direct CPU interaction is required, including:

Boot code
CPU mode transitions
Interrupt entry
Context switching support
User-mode transitions
System-call entry

This combination provides both high-level kernel development and low-level processor control.

What are the advantages of ValaOS?
1. Practical Operating System Experience

ValaOS converts theoretical operating system concepts into working implementations.

2. Low-Level Programming

The project works directly with concepts such as:

CPU Registers
GDT
IDT
PIC
PIT
Paging
Page Tables
Interrupts
Privilege Levels
3. C and Assembly Integration

The project demonstrates how C and Assembly can work together inside a kernel.

4. Modular Development

Major subsystems are separated into individual modules.

For example:

memory.c       → Memory management
paging.c       → Paging
interrupts.c   → Interrupt configuration
task.c         → Tasks and scheduler
vfs.c          → Virtual filesystem
disk.c         → Disk access
syscall.c      → System calls
keyboard.c     → Shell and keyboard input
5. Real Debugging Experience

Operating system development requires dealing with issues at a much lower level than normal application development.

ValaOS provides practical experience with:

Kernel crashes
Memory faults
Invalid mappings
Interrupt problems
Context switching
Boot problems
Filesystem issues
Hardware-level debugging
6. Extensible Architecture

The current architecture provides a foundation for future features such as:

More System Calls
       ↓
Better Process Management
       ↓
Improved Memory Management
       ↓
Networking
       ↓
Drivers
       ↓
64-bit Architecture
       ↓
Graphical Interface
What are the current limitations?

ValaOS is still a development and learning operating system.

Current limitations include:

32-bit x86 architecture
Basic scheduler
Limited filesystem capacity
Limited executable format support
Small system-call interface
Basic command-line shell
Limited hardware drivers
No networking stack
No graphical interface
No USB subsystem
No SMP/multi-core support
Limited process isolation

These limitations are also potential areas for future development.

What are the future goals of ValaOS?

The long-term goal is to gradually evolve ValaOS from a small educational operating system into a much more capable experimental OS.

Planned areas include:

Better Memory Management
Dynamic kernel heap
Improved physical memory allocator
Better virtual memory management
Per-process address spaces
Improved memory protection
Copy-on-write support
Better Process Management
Improved process creation
Better process termination
Process isolation
Process priorities
More advanced scheduling
Multiple independent user processes
More System Calls

Future system calls could provide functionality for:

File Operations
Process Management
Memory Management
Input/Output
Time
Inter-Process Communication
Improved Filesystem

Future VFS improvements may include:

Larger storage
Better metadata
File permissions
File timestamps
Improved allocation
More efficient directory management
Networking

A future version could introduce:

Ethernet
   ↓
Network Driver
   ↓
Network Stack
   ↓
IP
   ↓
TCP/UDP
   ↓
Applications
64-Bit Architecture

One of the major long-term goals is moving from:

32-bit x86

to:

x86_64

This would require major changes to the boot process, memory management, paging, interrupt system, CPU context, and kernel architecture.

Graphical Interface

A much later goal could be a basic graphical environment with:

Framebuffer
Mouse support
Window management
Keyboard input
Basic GUI applications

The priority is to strengthen the kernel before introducing a graphical interface.

What have I learned by building ValaOS?

ValaOS provides hands-on experience with:

Operating system architecture
Kernel development
C programming
x86 Assembly
Computer architecture
Memory management
Paging
Virtual memory
Interrupt handling
Hardware timers
Task scheduling
Context switching
CPU privilege levels
Ring 3 execution
System calls
Filesystem design
Disk I/O
Debugging
Build systems
Virtual machines
Git and GitHub
What is the current project status?
Project       : ValaOS
Version       : 2.0.0
Architecture  : 32-bit x86
Kernel        : Custom
Bootloader    : GRUB
Language      : C + x86 Assembly
Filesystem    : Persistent VFS
User Mode     : Ring 3
Scheduler     : Timer-driven
Program Loader: Filesystem-backed
Testing       : 7/7 Passed
Environment   : QEMU
Status        : Stable Development Build
How can ValaOS be built?

The project uses Make for compilation.

Clone the repository:

git clone https://github.com/nikhilvala0005-cloud/ValaOS.git

Enter the project:

cd ValaOS

Build:

make

Create the bootable ISO:

make iso

Run the operating system:

make run
How can the build be cleaned?

Use:

make clean

Generated build files can then be recreated using:

make

The persistent disk image should be preserved if filesystem data needs to remain available.

What does the project structure look like?
ValaOS/
│
├── boot/
│   └── grub/
│       └── grub.cfg
│
├── kernel/
│   ├── boot.asm
│   ├── kernel.asm
│   ├── kernel.c
│   ├── keyboard.c
│   ├── memory.c
│   ├── paging.c
│   ├── pagefault.c
│   ├── pagefault.asm
│   ├── interrupts.c
│   ├── interrupts.asm
│   ├── timer.asm
│   ├── task.c
│   ├── tss.c
│   ├── tss.asm
│   ├── user_mode.asm
│   ├── syscall.c
│   ├── syscall.asm
│   ├── vfs.c
│   └── disk.c
│
├── build/
├── docs/
├── backup/
│
├── linker.ld
├── Makefile
├── README.md
└── .gitignore
Why is ValaOS a valuable engineering project?

ValaOS demonstrates more than the ability to write application-level code.

It requires understanding how software interacts with:

CPU
 ↓
Memory
 ↓
Interrupts
 ↓
Hardware Timer
 ↓
Kernel
 ↓
Processes
 ↓
Filesystem
 ↓
User Programs

The project therefore combines concepts from:

Operating Systems
Computer Architecture
Systems Programming
Memory Management
Software Engineering

into one continuous project.

What is the long-term vision for ValaOS?

The long-term vision is to continue building ValaOS incrementally.

The intended progression is:

Small Kernel
     ↓
Working OS Environment
     ↓
Better Process Management
     ↓
Better Memory Management
     ↓
More System Calls
     ↓
Better Filesystem
     ↓
Device Drivers
     ↓
Networking
     ↓
64-bit Architecture
     ↓
Graphical Environment

The goal is not to build everything at once.

The goal is to understand, implement, test, and improve each subsystem one step at a time.

Where can the project be found?

GitHub Repository:

https://github.com/nehalvala0005-cloud/ValaOS

Who built ValaOS?

Nehal Vala

Computer Engineering Student

Areas of interest:

Operating Systems
Artificial Intelligence
Machine Learning
Systems Programming
Software Development
Computer Architecture

ValaOS is a personal engineering and learning project created to explore operating system development from the lowest practical level.

Final Thoughts

ValaOS started with a simple question:

"What actually happens inside a computer after the power is turned on?"

Instead of answering that question only through theory, this project attempts to answer it through implementation.

From booting a kernel to managing memory, handling interrupts, scheduling tasks, entering user mode, handling system calls, storing files, and executing programs, ValaOS brings multiple operating-system concepts together into one project.

The current version is only one stage of the project.

The architecture is intentionally designed to provide a foundation for future development.

Understand the concept.
        ↓
Implement the subsystem.
        ↓
Test it.
        ↓
Connect it with the kernel.
        ↓
Build the next subsystem.

ValaOS — Built from scratch, one subsystem at a time.


## Copyright

Copyright © 2026 Nehal Vala. All rights reserved.

ValaOS is an original operating-system development project created by
Nehal Vala.

The source code and documentation are publicly available for educational
and review purposes. Reproduction, redistribution, or use of substantial
portions of this project in another project is not permitted without
prior written permission from the copyright holder.

### Author

**Nehal Vala**

GitHub:  
https://github.com/nehalvala0005-cloud

Repository:  
https://github.com/nehalvala0005-cloud/ValaOS
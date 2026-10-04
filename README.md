ValaOS
A Custom 32-Bit x86 Operating System Built From Scratch
ValaOS is a custom 32-bit x86 operating system developed from the ground up using C and x86 Assembly.
The project focuses on understanding operating-system internals by implementing and connecting kernel, memory, paging, interrupts, scheduling, user-mode execution, system calls, persistent storage, filesystems, program loading, and process management.
ValaOS is not built to replace modern operating systems. It is built to understand them.

🔗 Important Links
Resource	Link
🌐 Website	https://nehalvala0005-cloud.github.io/ValaOS/
💻 GitHub Repository	https://github.com/nehalvala0005-cloud/ValaOS
📦 GitHub Releases	https://github.com/nehalvala0005-cloud/ValaOS/releases
🚀 ValaOS 3.0 ISO	https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-3.0.iso
📦 ValaOS 2.0 ISO	https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-2.0.0.iso


📚 Table of Contents
- Current Release — ValaOS 3.0.0
- Previous Release — ValaOS 2.0.0
- Architecture
- Platform & Technologies
- Features
- How ValaOS Boots
- Memory Management
- Paging & Virtual Memory
- Interrupts & Timer
- Userspace
- System Calls
- Filesystem & VFS
- Process Management
- Shell
- Running ValaOS
- Validation & Testing
- Project Structure
- Version History
- Roadmap
- Why ValaOS?
- About
- License
🚀 Current Release — ValaOS 3.0.0
Userspace + Process Management
ValaOS 3.0 represents the next major expansion of the kernel into a more complete userspace environment.
Major 3.0 Additions
- Interactive shell with command parsing
- Multiple command arguments
- Quoted command arguments
- Filesystem commands and directory navigation
- Persistent filesystem operations
- VFS-backed user-program loading
- Ring 3 user-program execution
- Program arguments passed through the user stack
- User-process table and PID allocation
- Process states and termination tracking
- ps process inspection
- kill PID validation and protection
- Multiple user programs
- Integrated ValaOS 3.0 self-test
- Bootable ValaOS 3.0 ISO
ValaOS 3.0 Validation
ValaOS 3.0 SELF TEST

[ OK ] Memory
[ OK ] Paging
[ OK ] Disk
[ OK ] Virtual Memory
[ OK ] Scheduler
[ OK ] Program Loader
[ OK ] VFS
[ OK ] Process Management

Passed: 8
Failed: 0
SYSTEM STATUS: OK
Release Status
Item	Status
Version	3.0.0
Focus	Userspace + Process Management
Architecture	32-bit x86
User Mode	Ring 3
Filesystem	Persistent VFS
Program Loading	Filesystem-backed
Process Management	PID-based
Validation	8/8 PASS
ISO	Bootable


📦 Previous Release — ValaOS 2.0.0
Kernel Core + Filesystem Foundation
ValaOS 2.0 established the core kernel architecture and filesystem foundation required for later userspace and process features.
Major 2.0 Features
- 32-bit x86 protected mode
- GRUB bootloader
- Global Descriptor Table
- Interrupt Descriptor Table
- Interrupt handling
- Programmable Interval Timer
- Keyboard driver
- Paging
- Virtual memory foundation
- Page fault handling
- Task and scheduler framework
- Virtual Filesystem
- Persistent filesystem
- Interactive shell
- File operations
- Directory operations
- Kernel diagnostics
- System self-test
ValaOS 2.0 Validation
Passed: 7
Failed: 0
SYSTEM STATUS: OK
🏗️ Architecture
ValaOS currently targets a 32-bit x86 environment and uses GRUB to load its custom kernel.
High-Level Architecture
+--------------------------------------+
|          User Programs              |
|              Ring 3                  |
+--------------------------------------+
                  |
                  | System Calls
                  v
+--------------------------------------+
|              Shell                  |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|        Process Management            |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|       Virtual Filesystem             |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|     Memory / Paging / Virtual       |
|              Memory                 |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|     Interrupts / Timer / Drivers   |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|             x86 CPU                 |
+--------------------------------------+
Core Execution Flow
Hardware Timer
      ↓
Interrupt
      ↓
Scheduler
      ↓
Task Context
      ↓
Next Task
User Program Flow
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
The objective is to make multiple operating-system subsystems work together rather than keeping them as isolated experiments.
⚙️ Platform & Technologies
Component	Technology
Architecture	32-bit x86
Kernel	Custom
Bootloader	GRUB
Kernel Language	C
Low-Level Language	x86 Assembly
Assembler	NASM
Compiler	GCC
Build System	GNU Make
Emulator	QEMU
Version Control	Git
Repository	GitHub
Development Environment	WSL
Userspace	Ring 3
Kernel Mode	Ring 0
Filesystem	Custom Persistent Filesystem
Virtual Filesystem	VFS


Verified Toolchain
Tool	Version
GCC	15.2.0
NASM	3.01
QEMU	10.2.1
GRUB	2.14
Git	2.53.0


✨ Features
🧠 Kernel
- 32-bit x86 protected mode
- GDT
- IDT
- Interrupt handling
- Hardware timer
- Keyboard input
- Paging
- Virtual memory
- Page fault handling
- Task management
- Scheduler framework
💾 Storage
- Persistent disk image
- Virtual Filesystem
- Files
- Directories
- File creation
- File reading
- File writing
- File deletion
- Directory navigation
👤 Userspace
- Ring 3 execution
- User program loader
- User program arguments
- User stack
- System calls
- User program termination
- User-process tracking
⚙️ Process Management
- Process IDs
- User process table
- Process creation
- Process states
- Process listing
- Process termination
- PID validation
- Protected kernel and shell processes
🥾 How ValaOS Boots
The boot process begins with GRUB loading the ValaOS kernel.
Simplified Boot Sequence
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
🧠 Memory Management
ValaOS contains its own basic memory management subsystem.
The memory manager keeps track of memory pages and provides information such as:
- Total pages
- Used pages
- Page size
Current Page Size
4096 bytes
Memory-related functionality can be tested through commands available inside the ValaOS shell.
🗺️ Paging & Virtual Memory
ValaOS implements x86 paging to introduce virtual memory.
Paging allows the operating system to control how virtual addresses are mapped to physical memory.
Paging Components
- Page directories
- Page tables
- Page mappings
- Page flags
- User-accessible pages
- Supervisor-only pages
User Memory Layout
Region	Address
User Code	0x00400000
User Stack	0x00800000


The paging system uses permission flags to separate user-accessible memory from kernel-only memory.
This provides the foundation for protected user-mode execution.
⚡ Interrupts & Timer
Interrupts allow the processor to notify the kernel when important events occur.
ValaOS contains interrupt handling for:
- Hardware timer
- Keyboard
- Page faults
- System calls
Timer Flow
Hardware Timer
      ↓
IRQ0
      ↓
Timer ISR
      ↓
Timer Handler
      ↓
Scheduler
      ↓
Task Context
The timer provides the foundation for timer-driven scheduling.
👤 Userspace
ValaOS 3.0 introduces a basic userspace execution environment.
User programs execute outside the kernel's normal privilege level.
Privilege Levels
Mode	Ring	Purpose
Kernel	Ring 0	Kernel and hardware operations
Userspace	Ring 3	User programs


User Program Execution
Kernel
  │
  │ Load Program
  ▼
User Program
  │
  │ int 0x80
  ▼
System Call Handler
  │
  ▼
Kernel
  │
  ▼
Operation
User Program Example
ValaOS> run hello

Starting user program: hello
[ PROCESS ] PID 4 started

[ USER ] sys_write: U
[ USER ] sys_exit called
[ OK ] User task terminated
📞 System Calls
ValaOS uses software interrupts for system calls.
The current syscall interface uses:
int 0x80
Available System Calls
System Call	Number	Purpose
SYS_WRITE	1	Write a character through the kernel
SYS_EXIT	2	Terminate the user program


System Call Flow
User Program
     ↓
int 0x80
     ↓
Syscall ISR
     ↓
Kernel Syscall Handler
     ↓
Requested Operation
📁 Filesystem & VFS
ValaOS includes a persistent filesystem integrated through a Virtual Filesystem layer.
Filesystem Capabilities
- Create files
- Read files
- Write files
- Remove files
- Create directories
- Navigate directories
- List directory contents
- Persist filesystem data
- Load programs from the filesystem
Example
ValaOS> mkdir projects
ValaOS> cd projects
ValaOS> touch test.txt
ValaOS> write test.txt
ValaOS> cat test.txt
Program Loading
User programs can be loaded directly from the ValaOS filesystem.
Filesystem
     ↓
Program Loader
     ↓
User Memory
     ↓
Ring 3 Execution
⚙️ Process Management
ValaOS 3.0 introduces a userspace process table.
Each launched user program receives a unique PID.
Process Information
Property	Description
PID	Unique process identifier
Name	User program name
State	Current process state
Used	Process table allocation status


Example
ValaOS> ps

PID   NAME      STATE
1     kernel    READY
2     shell     RUNNING
3     idle      READY
4     hello     TERMINATED
5     args      TERMINATED
6     args      TERMINATED
Process Lifecycle
run program
     ↓
Process Created
     ↓
PID Assigned
     ↓
User Program Starts
     ↓
Program Executes
     ↓
Program Exits
     ↓
Process Marked TERMINATED
Process Protection
Core kernel processes are protected from termination.
ValaOS> kill 1

[ ERROR ] Protected task
Invalid process IDs are handled safely:
ValaOS> kill 99

[ ERROR ] PID not found
Already terminated processes are also handled:
ValaOS> kill 4

[ ERROR ] Process already terminated
🖥️ Shell
ValaOS provides an interactive command-line shell for system diagnostics, filesystem operations, and user programs.
Available Commands
Command	Purpose
help	Display available commands
clear	Clear the terminal
ls	List directory contents
pwd	Show current directory
cd	Change directory
mkdir	Create directory
touch	Create file
cat	Read file
write	Write data to file
rm	Remove file
run	Execute user program
ps	Display processes
kill	Terminate a process


Command Arguments
ValaOS supports multiple command arguments and quoted command arguments.
Example:
ValaOS> run args abc
User-program arguments are prepared in the user stack before execution.
🧪 Validation & Testing
ValaOS includes an integrated self-test that validates major operating-system subsystems.
ValaOS 3.0 Self-Test
[ OK ] Memory
[ OK ] Paging
[ OK ] Disk
[ OK ] Virtual Memory
[ OK ] Scheduler
[ OK ] Program Loader
[ OK ] VFS
[ OK ] Process Management

Passed: 8
Failed: 0
SYSTEM STATUS: OK
Test Coverage
Subsystem	Result
Memory	✅ PASS
Paging	✅ PASS
Disk	✅ PASS
Virtual Memory	✅ PASS
Scheduler	✅ PASS
Program Loader	✅ PASS
VFS	✅ PASS
Process Management	✅ PASS


Result
ValaOS 3.0: 8/8 subsystem tests passed.

🚀 Running ValaOS
Requirements
Install:
- GCC
- NASM
- GRUB tools
- QEMU
- GNU Make
- Git
- WSL or a compatible Linux environment
Clone the Repository
git clone https://github.com/nehalvala0005-cloud/ValaOS.git
cd ValaOS
Build
make
Create ISO
make iso
The generated ISO will be available at:
build/ValaOS.iso
Run
make run
Clean Build Files
make clean
📂 Project Structure
ValaOS/
│
├── build/
│   ├── ValaOS.iso
│   └── ValaOS.img
│
├── kernel/
│   ├── kernel.c
│   ├── shell.c
│   ├── task.c
│   ├── syscall.c
│   ├── interrupts.c
│   ├── pagefault.c
│   └── ...
│
├── boot/
│   └── ...
│
├── drivers/
│   └── ...
│
├── filesystem/
│   └── ...
│
├── user/
│   └── ...
│
├── Makefile
├── README.md
└── ...
The project structure may evolve as ValaOS development continues.

📜 Version History
Version	Focus	Status
3.0.0	Userspace + Filesystem + Process Management	✅ Current
2.0.0	Kernel Core + Paging + VFS	✅ Previous


Future releases will be added here as ValaOS evolves.
🛣️ Roadmap
ValaOS is being developed incrementally through multiple stages.
Phase	Focus	Status
1.0	Foundation	✅ Complete
2.0	Kernel Core	✅ Complete
3.0	Userspace + Filesystem	✅ Complete
4.0	Drivers + Process System	🔄 Next
5.0	Networking	⏳ Planned
6.0	Graphics + GUI/Desktop	⏳ Planned
7.0	x86-64 Architecture	⏳ Planned
8.0	SMP + Advanced Hardware	⏳ Planned
9.0	Security + System Services	⏳ Planned
10.0	Mature OS Platform	🔮 Future


Phase 4 — Drivers + Process System
Planned improvements include:
- More hardware drivers
- Improved process scheduling
- Better process isolation
- Expanded process lifecycle
- Improved kernel/user interaction
Phase 5 — Networking
Planned:
- Network driver support
- Ethernet
- TCP/IP foundation
- Network utilities
- Socket-style interfaces
Phase 6 — Graphics + GUI
Planned:
- Graphics mode
- Framebuffer
- Window management
- Desktop environment
- Mouse support
- Graphical applications
Phase 7 — x86-64 Architecture
Planned:
- Long mode
- 64-bit kernel
- 64-bit userspace
- Extended memory support
Phase 8 — SMP + Advanced Hardware
Planned:
- Multi-core CPU support
- SMP
- Advanced interrupt controllers
- Improved hardware abstraction
Phase 9 — Security + System Services
Planned:
- Permission system
- User accounts
- Security boundaries
- Background services
- System service management
Phase 10 — Mature OS Platform
Long-term goals:
- Stable userspace
- Complete driver ecosystem
- Networking
- GUI
- Security
- Applications
- Package management
- System utilities
💡 Why ValaOS?
The main reason behind ValaOS is simple:
To understand what happens underneath the software that we normally take for granted.

Instead of only learning operating-system concepts theoretically, ValaOS turns those concepts into actual implementations.
Through this project, concepts such as:
- CPU privilege levels
- Memory management
- Virtual memory
- Paging
- Interrupts
- Scheduling
- Context switching
- System calls
- Filesystems
- Disk I/O
- User-mode execution
are implemented and connected inside one system.
The project therefore acts as a practical exploration of computer architecture and operating-system internals.
🔍 What Makes ValaOS Different?
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
User programs follow another complete path:
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
🎯 Project Goals
The main goals of ValaOS are:
1. Understand operating-system internals.
2. Learn low-level x86 architecture.
3. Build a kernel from scratch.
4. Implement memory management.
5. Implement interrupt handling.
6. Build a filesystem.
7. Create a userspace environment.
8. Implement system calls.
9. Implement process management.
10. Gradually evolve ValaOS into a more complete operating system.
👨‍💻 About
Nehal Vala
ValaOS is developed by Nehal Vala as a personal operating-system development project.
The project combines:
- C programming
- x86 Assembly
- Computer architecture
- Operating systems
- Memory management
- Filesystems
- Process management
- Systems programming
🌐 Project Website
The official ValaOS website provides:
- Project overview
- Architecture information
- Features
- Release information
- ISO downloads
- Version history
- Project documentation
Website
https://nehalvala0005-cloud.github.io/ValaOS/
📦 Downloads
🚀 ValaOS 3.0.0 — Latest
ISO
https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-3.0.iso
GitHub Release
https://github.com/nehalvala0005-cloud/ValaOS/releases/tag/v3.0.0
📦 ValaOS 2.0.0 — Previous
ISO
https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-2.0.0.iso
GitHub Releases
https://github.com/nehalvala0005-cloud/ValaOS/releases
📊 Current Project Status
ValaOS
│
├── Foundation              ✅ Complete
├── Kernel Core             ✅ Complete
├── Filesystem              ✅ Complete
├── Userspace               ✅ Complete
├── Process Management      ✅ Complete
├── Networking              ⏳ Planned
├── Graphics / GUI          ⏳ Planned
├── x86-64                  ⏳ Planned
├── SMP                     ⏳ Planned
└── Mature OS               🔮 Future
Current Release
ValaOS 3.0.0 — Userspace + Process Management

Next Major Focus
ValaOS 4.0 — Drivers + Process System

📄 License
ValaOS is an educational and personal operating-system development project.
See the repository for the current licensing information and project terms.
⭐ ValaOS Development
ValaOS will continue to evolve through incremental releases.
Each release builds on the previous architecture while introducing new operating-system capabilities.
Current Version: ValaOS 3.0.0
Next Major Focus: Drivers + Process System
🔗 Project Links
Platform	Link
🌐 Website	https://nehalvala0005-cloud.github.io/ValaOS/
💻 GitHub	https://github.com/nehalvala0005-cloud/ValaOS
📦 Releases	https://github.com/nehalvala0005-cloud/ValaOS/releases
🚀 Latest ISO	https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-3.0.iso


Built from scratch. One subsystem at a time.
ValaOS — Learn the machine by building the machine.
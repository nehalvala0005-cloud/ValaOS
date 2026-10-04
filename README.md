# ValaOS

## A Custom 32-Bit x86 Operating System Built From Scratch

ValaOS is an educational operating-system project written in C and x86 Assembly. It brings kernel fundamentals, a persistent virtual filesystem, Ring 3 programs, system calls, and basic process management together in a bootable system.

> Built to understand how operating systems work, one subsystem at a time.

![Release](https://img.shields.io/badge/release-3.0.0-green)
![Architecture](https://img.shields.io/badge/architecture-32--bit%20x86-blue)

# Important Links

| Project | Link |
|---|---|
| Website | [ValaOS project site](https://nehalvala0005-cloud.github.io/ValaOS/) |
| GitHub | [Source repository](https://github.com/nehalvala0005-cloud/ValaOS) |
| Releases | [All releases](https://github.com/nehalvala0005-cloud/ValaOS/releases) |
| ValaOS 3.0 ISO | [Download](https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-3.0.iso) |
| ValaOS 2.0 ISO | [Download](https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-2.0.0.iso) |

---

# ValaOS 3.0.0

## Userspace + Process Management

ValaOS 3.0 builds on the kernel and filesystem foundation of 2.0 with a focus on running user programs and tracking their processes.

### What's New

| Area | Capability |
|---|---|
| User programs | Load program binaries from the VFS and enter Ring 3 |
| Arguments | Pass command-line arguments to a user program |
| Process management | Track user processes with PIDs and states; list them with `ps` and terminate them with `kill <PID>` |
| Protected tasks | The kernel and shell tasks cannot be terminated with `kill` |

### ValaOS 3.0 Self-Test

```text
========== ValaOS 3.0 SELF TEST ==========

[ OK ] Memory
[ OK ] Paging
[ OK ] Disk
[ OK ] Virtual Memory
[ OK ] Scheduler
[ OK ] Program Loader
[ OK ] VFS
[ OK ] Process Management

===========================================
Passed: 8
Failed: 0
SYSTEM STATUS: OK
===========================================
```

### Download

- [ValaOS 3.0 ISO](https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-3.0.iso)
- [ValaOS 3.0.0 GitHub release](https://github.com/nehalvala0005-cloud/ValaOS/releases/tag/v3.0.0)

---

# ValaOS 2.0.0

## Kernel Core + Filesystem Foundation

ValaOS 2.0 established the kernel subsystems that ValaOS 3.0 extends.

### Major Features

Paging and virtual memory, interrupt and timer handling, task scheduling, Ring 3 execution, system calls, and a persistent VFS with directories and filesystem-backed program loading.

### Download

[ValaOS 2.0.0 ISO](https://nehalvala0005-cloud.github.io/ValaOS/downloads/ValaOS-2.0.0.iso)

---

# Table of Contents

- [Important Links](#important-links)
- [ValaOS 3.0.0](#valaos-300)
- [ValaOS 2.0.0](#valaos-200)
- [Architecture](#architecture)
- [Platform and Technologies](#platform-and-technologies)
- [Features](#features)
- [Boot Process](#boot-process)
- [Userspace](#userspace)
- [System Calls](#system-calls)
- [Filesystem](#filesystem)
- [Process Management](#process-management)
- [Shell](#shell)
- [Validation](#validation)
- [Running ValaOS](#running-valaos)
- [Project Structure](#project-structure)
- [Version History](#version-history)
- [Roadmap](#roadmap)
- [Why ValaOS?](#why-valaos)
- [About](#about)
- [Project Links](#project-links)
- [License](#license)

---

# Architecture

## High-Level Architecture

```text
GRUB
  ↓
Boot and kernel initialization
  ↓
Memory, paging, interrupts, and timer
  ↓
Scheduler and kernel tasks
  ↓
Ring 3 userspace and system calls
  ↓
VFS and persistent disk
```

## User Program Flow

```text
Program in VFS → Loader → User memory → Ring 3 → System call → Kernel
```

## Scheduler Flow

```text
Timer interrupt → Scheduler → Task selection → Context switch
```

---

# Platform and Technologies

## Development Stack

| Area | Technology |
|---|---|
| Architecture | 32-bit x86 |
| Languages | C and x86 Assembly |
| Bootloader | GRUB |
| Assembler | NASM |
| Compiler | GCC |
| Build system | Make |
| Emulator | QEMU |
| Development environment | WSL |

---

# Features

## Kernel Features

### Core

- Physical memory management, paging, virtual memory, and page-fault handling
- Interrupts, a hardware timer, task scheduling, and context switching support
- Ring 3 execution and system-call handling

## Filesystem Features

### Storage

- Virtual filesystem with files, directories, navigation, and basic file operations
- Disk-backed storage intended to persist across restarts when the disk image is retained
- Filesystem-backed loading of user program binaries

## Userspace Features

### User Programs

- Load and execute user programs in Ring 3
- Pass command-line arguments to programs
- Available system calls: `SYS_WRITE` and `SYS_EXIT`

## Process Features

### Process Management

- Process IDs and state tracking for user processes
- Process listing and termination from the shell
- Protected kernel and shell tasks

---

# Boot Process

## Simplified Boot Sequence

### From GRUB to Shell

```text
GRUB → Kernel entry → CPU and GDT setup → Memory and paging
     → Interrupts and timer → Scheduler → Filesystem → Shell
```

---

# Userspace

## Ring 0 and Ring 3

### Privilege Levels

| Mode | Ring | Purpose |
|---|---:|---|
| Kernel | Ring 0 | Kernel operations |
| Userspace | Ring 3 | User programs |

## User Program Execution

### Execution Flow

The shell resolves a program name to a `.bin` file, loads it from the VFS into user memory, prepares its arguments, and enters Ring 3.

## Example

### Running a User Program

```text
run hello
```

The shell looks for `hello.bin`. Arguments may also be supplied after the program name.

---

# System Calls

## System Call Interface

### Interrupt Interface

User programs invoke the system-call interrupt with `int 0x80`. The syscall number is passed in `EAX`, and the syscall value is passed in `EBX`.

## Available System Calls

### Current ABI

| Call | Number | Purpose |
|---|---:|---|
| `SYS_WRITE` | 1 | Request kernel output for a character value |
| `SYS_EXIT` | 2 | Terminate the current user task |

---

# Filesystem

## Virtual Filesystem

### Supported Commands

| Command | Purpose |
|---|---|
| `pwd` | Show the current directory |
| `cd <dir>` | Change directory; `cd` returns to the root |
| `ls [directory]` | List entries |
| `mkdir <dir>` | Create a directory |
| `touch <file>` | Create a file |
| `cat <file>` | Display a file |
| `write <file> <text>` | Write text to a file |
| `rm <file>` | Remove a file |

## Example

### Basic File Operations

```text
mkdir notes
cd notes
touch todo.txt
write todo.txt Review paging
cat todo.txt
```

## Persistent Storage

The VFS reads and writes disk sectors. The disk image identified by the existing project documentation is `build/ValaOS.img`; preserve it to retain filesystem data across restarts.

---

# Process Management

## Process Table

### Process Information

| Field | Description |
|---|---|
| PID | Process or task identifier |
| Name | Task or user-program name |
| State | Execution state, such as `READY`, `RUNNING`, or `TERMINATED` |

The user-process table has eight entries. User-process PIDs begin at 4; the initial kernel tasks use PIDs 1 through 3.

## Process Example

### `ps` Output

The initial kernel-task entries are defined as follows; their states can change as scheduling proceeds.

```text
PID   NAME      STATE
1     kernel      RUNNING
2     shell      READY
3     idle      READY
```

## Process Lifecycle

### User Program Lifecycle

```text
run <program> → Allocate process entry → Load and enter user program
             → SYS_EXIT or kill <PID> → Mark process terminated
```

## Process Protection

### Protected Tasks

The shell rejects attempts to terminate PID 1 (`kernel`) or PID 2 (`shell`).

---

# Shell

## Interactive Shell

ValaOS provides an interactive command-line shell for system inspection, tests, file operations, task management, and running programs.

### Available Commands

| Category | Commands |
|---|---|
| Help and information | `help`, `info`, `about`, `version`, `uptime`, `clear`, `echo <text>` |
| Memory and paging | `memtest`, `memstress`, `meminfo`, `paging`, `ptest`, `dptest`, `vmtest`, `vmaptest` |
| Filesystem | `pwd`, `cd`, `ls`, `mkdir`, `cat`, `touch`, `write`, `rm`, `programs` |
| Tasks and processes | `ps`, `tasks`, `kill <PID>`, `ticks`, `schedule` |
| Programs and system | `run <program> [args...]`, `disktest`, `selftest`, `reboot`, `shutdown` |

### Program Arguments

`run` accepts arguments after the program name and passes them to the user program.

---

# Validation

## ValaOS 3.0 Self-Test

### Result

The reported result is **8 passed, 0 failed**.

## Test Coverage

### ValaOS 3.0

| Check | Result |
|---|---|
| Memory | Passed |
| Paging | Passed |
| Disk | Passed |
| Virtual memory | Passed |
| Scheduler | Passed |
| Program loader | Passed |
| VFS | Passed |
| Process management | Passed |

## ValaOS 2.0 Validation

### Result

The ValaOS 2.0 self-test reported **7 passed, 0 failed**: memory, paging, disk, virtual memory, scheduler, program loader, and VFS.

---

# Running ValaOS

## Requirements

### Tools

Use WSL with Git, Make, GCC, NASM, GRUB rescue tooling, and QEMU available.

## Clone the Repository

### Git

```sh
git clone https://github.com/nehalvala0005-cloud/ValaOS.git
cd ValaOS
```

## Build

### Compile

```sh
make
```

## Create ISO

### Bootable ISO

```sh
make iso
```

## Run

### QEMU

```sh
make run
```

## Clean

### Remove Build Files

```sh
make clean
```

The clean target removes generated objects, the kernel binary, ISO, and ISO staging directory. Keep the disk image if its filesystem data is needed.

---

# Project Structure

## Main Structure

### Repository Layout

```text
ValaOS/
├── boot/grub/grub.cfg
├── kernel/
│   ├── boot.asm
│   ├── disk.c
│   ├── interrupts.asm
│   ├── interrupts.c
│   ├── kernel.asm
│   ├── kernel.c
│   ├── keyboard.c
│   ├── memory.c
│   ├── pagefault.asm
│   ├── pagefault.c
│   ├── paging.c
│   ├── shell.c
│   ├── syscall.asm
│   ├── syscall.c
│   ├── task.c
│   ├── timer.asm
│   ├── tss.asm
│   ├── tss.c
│   ├── user_mode.asm
│   └── vfs.c
├── website/
├── linker.ld
├── LICENSE
├── Makefile
└── README.md
```

---

# Version History

## Release Overview

| Version | Focus | Status |
|---|---|---|
| 3.0.0 | Userspace + Process Management | Current |
| 2.0.0 | Kernel Core + Filesystem Foundation | Previous |

---

# Roadmap

## Development Roadmap

The existing project roadmap identifies these areas for future development; this is not a committed release schedule.

| Area | Direction |
|---|---|
| Memory management | Improve allocation, virtual memory, and protection |
| Process management | Improve creation, termination, isolation, and scheduling |
| Filesystem and system calls | Expand storage capabilities and kernel interfaces |
| Drivers and networking | Add hardware support and explore a network stack |
| Graphics and architecture | Explore a graphical interface and x86-64 |

## Next Major Release

### ValaOS 4.0 — Drivers + Process System

Drivers + Process System is the stated next-major-release focus. No further ValaOS 4.0 feature details are specified here.

## Future Directions

### Networking

The existing roadmap proposes networking as a future direction; no implemented network stack is claimed.

### Graphics

A graphical environment is a longer-term possibility, after further kernel development.

### x86-64

Moving from 32-bit x86 to x86-64 remains a long-term goal.

---

# Why ValaOS?

## Project Purpose

ValaOS is a hands-on project for exploring operating-system internals, from boot and memory management to scheduling, userspace, system calls, and storage.

## Development Philosophy

### Build One Subsystem at a Time

```text
Understand → Implement → Test → Integrate → Continue
```

---

# About

## Nehal Vala

ValaOS is a personal operating-system development project by Nehal Vala, a computer engineering student.

### Areas of Interest

Operating systems, systems programming, computer architecture, and software development.

---

# Project Links

## Website

See the Website entry in the [Important Links](#important-links) table.

## GitHub

See the GitHub entry in the [Important Links](#important-links) table.

## Releases

See the Releases entry in the [Important Links](#important-links) table.

---

# License

## Project License

ValaOS uses custom project terms, not a standard open-source license. Downloading, building, running, studying, and reviewing for personal, educational, and research purposes are permitted with attribution. Redistribution, publication, modification, or substantial reuse requires prior written permission. See [LICENSE](LICENSE) for the complete terms.
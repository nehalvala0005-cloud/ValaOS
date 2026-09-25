# ValaOS

A minimal 32-bit x86 operating system prototype built from scratch using C and x86 Assembly.

ValaOS is a learning and portfolio project designed to demonstrate low-level system concepts including booting, VGA text output, keyboard input, command-line interaction, heap memory management, task management, and a simple cooperative scheduler.

## Features

- 32-bit x86 boot process
- GRUB Multiboot-compatible kernel
- Custom x86 Assembly boot code
- Protected mode setup
- VGA text-mode display
- Keyboard input driver
- Command-line shell
- Terminal scrolling
- Built-in shell commands
- System reboot and shutdown commands
- Simple heap memory allocator
- `kmalloc()` and `kfree()`
- Memory usage information
- Task structure and task table
- Task states
- Simple cooperative scheduler prototype

## Shell Commands

| Command        | Description |
|---             |---|
| `help`         | Display available commands |
| `info`         | Display system information |
| `about`        | Display information about ValaOS |
| `clear`        | Clear the terminal |
| `version`      | Display kernel version |
| `echo <text>`  | Display custom text |
| `reboot`       | Restart the system |
| `shutdown`     | Halt the system |
| `memtest`      | Test heap allocation and release |
| `meminfo`      | Display heap memory information |
| `tasks`        | Display the task table |
| `schedule`     | Run one scheduler cycle |

## Task Management

ValaOS currently maintains a simple task table containing:

- Kernel
- Shell
- Idle task

Each task contains:

- PID
- Name
- State

Supported task states include:

- READY
- RUNNING
- BLOCKED
- TERMINATED

## Scheduler

ValaOS includes a simple cooperative scheduler prototype.

The scheduler rotates the `RUNNING` state between the available tasks.

Example:

```text
PID   NAME      STATE
1     kernel    RUNNING
2     shell     READY
3     idle      READY
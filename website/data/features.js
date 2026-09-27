export const features = [
    {
        name: "Memory Management",
        description:
            "Physical memory tracking, page allocation and memory statistics."
    },
    {
        name: "Paging",
        description:
            "x86 paging, virtual memory mapping and page permission control."
    },
    {
        name: "Interrupts",
        description:
            "Hardware and software interrupt handling through the IDT."
    },
    {
        name: "Hardware Timer",
        description:
            "Programmable interval timer providing scheduler ticks."
    },
    {
        name: "Task Scheduler",
        description:
            "PID-based task management with timer-driven context switching."
    },
    {
        name: "Ring 3 User Mode",
        description:
            "User programs execute outside kernel privilege level."
    },
    {
        name: "System Calls",
        description:
            "User programs communicate with the kernel through system calls."
    },
    {
        name: "Persistent VFS",
        description:
            "Files and directories stored on persistent disk-backed storage."
    },
    {
        name: "Program Loader",
        description:
            "User programs are loaded directly from the ValaOS filesystem."
    }
];
typedef unsigned int uint32_t;

#define TASK_READY "READY"
#define TASK_RUNNING "RUNNING"
#define TASK_BLOCKED "BLOCKED"
#define TASK_TERMINATED "TERMINATED"

#define TASK_STACK_SIZE 4096

struct task_context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t eflags;
};

struct task {
    uint32_t pid;
    const char* name;
    const char* state;
    uint32_t stack_base;
    uint32_t stack_top;
    struct task_context context;
};

static unsigned char task_stacks[3][TASK_STACK_SIZE]
    __attribute__((aligned(16)));

struct task tasks[] = {
    {1, "kernel", TASK_RUNNING, 0, 0, {0}},
    {2, "shell", TASK_READY, 0, 0, {0}},
    {3, "idle", TASK_READY, 0, 0, {0}}
};

int task_count = 3;
int current_task = 0;

void task_context_init() {
    int i;

    for (i = 0; i < task_count; i++) {
        tasks[i].stack_base =
            (uint32_t)&task_stacks[i][0];

        tasks[i].stack_top =
            (uint32_t)&task_stacks[i][TASK_STACK_SIZE];

        tasks[i].context.esp =
            tasks[i].stack_top;

        tasks[i].context.eflags = 0x202;
    }
}

struct task* get_tasks() {
    return tasks;
}

int get_task_count() {
    return task_count;
}

void schedule_once() {
    int i;

    for (i = 0; i < task_count; i++) {
        tasks[i].state = TASK_READY;
    }

    tasks[current_task].state = TASK_RUNNING;

    current_task++;

    if (current_task >= task_count) {
        current_task = 0;
    }
}

static uint32_t scheduler_tick_counter = 0;

void scheduler_tick() {
    scheduler_tick_counter++;

    if (scheduler_tick_counter >= 50) {
        scheduler_tick_counter = 0;
        schedule_once();
    }
}
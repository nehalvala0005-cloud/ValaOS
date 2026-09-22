extern void print(const char* str);

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
    uint32_t cs;
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

void scheduler_tick();

static unsigned char task_stacks[3][TASK_STACK_SIZE]
    __attribute__((aligned(16)));

struct task tasks[] = {
    {1, "kernel", TASK_RUNNING, 0, 0, {0}},
    {2, "shell", TASK_READY, 0, 0, {0}},
    {3, "idle", TASK_READY, 0, 0, {0}}
};

int task_count = 3;
int current_task = 0;

static const char* user_task_state = TASK_READY;

extern void keyboard_poll();
extern void print(const char* str);

void task_shell() {
    static int started = 0;

    if (!started) {
        started = 1;
        print("[ SCHED ] Shell task is now running\n");
    }

    while (1) {
        keyboard_poll();
    }
}

void task_idle() {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void task_context_init() {
    int i;
    uint32_t* stack;

    for (i = 0; i < task_count; i++) {
        tasks[i].stack_base =
            (uint32_t)&task_stacks[i][0];

        tasks[i].stack_top =
            (uint32_t)&task_stacks[i][TASK_STACK_SIZE];
    }

    stack = (uint32_t*)tasks[1].stack_top;
    stack -= 11;

    stack[0] = 0;
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = tasks[1].stack_top;
    stack[4] = 0;
    stack[5] = 0;
    stack[6] = 0;
    stack[7] = 0;
    stack[8] = (uint32_t)task_shell;
    stack[9] = 0x10;
    stack[10] = 0x202;

    tasks[1].context.esp = (uint32_t)stack;
    tasks[1].context.eip = (uint32_t)task_shell;
    tasks[1].context.cs = 0x10;
    tasks[1].context.eflags = 0x202;

    stack = (uint32_t*)tasks[2].stack_top;
    stack -= 11;

    stack[0] = 0;
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = tasks[2].stack_top;
    stack[4] = 0;
    stack[5] = 0;
    stack[6] = 0;
    stack[7] = 0;
    stack[8] = (uint32_t)task_idle;
    stack[9] = 0x10;
    stack[10] = 0x202;

    tasks[2].context.esp = (uint32_t)stack;
    tasks[2].context.eip = (uint32_t)task_idle;
    tasks[2].context.cs = 0x10;
    tasks[2].context.eflags = 0x202;
}

struct task* get_tasks() {
    return tasks;
}

int get_task_count() {
    return task_count;
}

int task_kill(uint32_t pid) {
    int i;

    if (pid == 1 || pid == 2) {
        return -1;
    }

    for (i = 0; i < task_count; i++) {
        if (tasks[i].pid == pid) {
            if (tasks[i].state == TASK_TERMINATED) {
                return -2;
            }

            tasks[i].state = TASK_TERMINATED;
            return 1;
        }
    }

    return 0;
}

void schedule_once() {
    int i;
    int next_task = current_task;

    for (i = 1; i <= 2; i++) {
        next_task = current_task + i;

        if (next_task >= 2)
            next_task = 0;

        if (tasks[next_task].state != TASK_TERMINATED)
            break;
    }

    if (tasks[next_task].state == TASK_TERMINATED)
        return;

    current_task = next_task;

    for (i = 0; i < task_count; i++) {
        if (tasks[i].state != TASK_TERMINATED)
            tasks[i].state = TASK_READY;
    }

    tasks[current_task].state = TASK_RUNNING;
}

uint32_t task_switch_prepare(uint32_t current_esp) {
    int previous_task = current_task;

    tasks[previous_task].context.esp = current_esp;

    scheduler_tick();

    if (current_task == previous_task)
        return current_esp;

    return tasks[current_task].context.esp;
}

static uint32_t scheduler_tick_counter = 0;

void scheduler_tick() {
    scheduler_tick_counter++;

    if (scheduler_tick_counter >= 1) {
        scheduler_tick_counter = 0;
        schedule_once();
    }
}

void user_task_start() {
    user_task_state = TASK_RUNNING;
}

void user_task_exit() {
    user_task_state = TASK_TERMINATED;
}

const char* get_user_task_state() {
    return user_task_state;
}
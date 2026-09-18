typedef unsigned int uint32_t;

#define TASK_READY "READY"
#define TASK_RUNNING "RUNNING"
#define TASK_BLOCKED "BLOCKED"
#define TASK_TERMINATED "TERMINATED"

struct task {
    uint32_t pid;
    const char* name;
    const char* state;
};

struct task tasks[] = {
    {1, "kernel", TASK_RUNNING},
    {2, "shell", TASK_READY},
    {3, "idle", TASK_READY}
};

int task_count = 3;
int current_task = 0;

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
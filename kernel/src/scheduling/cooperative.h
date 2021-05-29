#pragma once
#include <stdint.h>
#include "../userspace/userspace.h"
#include "../memory/PageTableManager.h"

#define STATE_RUNNING 1
#define STATE_HALTED 0

struct Task {
    uint64_t instructionPointer;
    uint64_t* stack;
    const char* name;
    int state;
};

class TaskManager {
 public:
    Task tasks[2048];
    int currentTask = 0;
    int taskNum = 0;
    char* lastTask;

    void DoYeld(uint64_t newP);
    void AddTask(Task t);
    void DoExitTask();
    void RunNext();
};

extern TaskManager* GlobalTaskManager;
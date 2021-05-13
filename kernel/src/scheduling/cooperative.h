#pragma once
#include <stdint.h>
#include "../userspace/userspace.h"
#include "../memory/PageTableManager.h"

struct Task {
    uint64_t instructionPointer;
    uint64_t* stack;
};

class TaskManager {
 public:
    Task tasks[512];
    int currentTask = 0;
    int taskNum = 0;

    void DoYeld(uint64_t newP);
    void AddTask(Task t);
};

extern TaskManager* GlobalTaskManager;
#pragma once
#include <stdint.h>
#include "../userspace/userspace.h"
#include "../memory/PageTableManager.h"

#define STATE_RUNNING 1
#define STATE_HALTED 0

#define TASK_USER 1
#define TASK_ADMIN 2

struct Task {
    uint64_t instructionPointer;
    uint64_t* stack;
    const char* name;
    int state;
    uint8_t privilege;
};

struct InternalTask {
    uint64_t nextInstructionPointer;
    uint64_t binaryEntryPoint;
    uint64_t* stack;
    const char* name;
    int state;
    uint64_t pid;
    uint8_t privilege;
};

class TaskManager {
 public:
    InternalTask tasks[1024];
    int currentTask = 0;
    int taskNum = 0;
    char* lastTask;

    void DoYeld(uint64_t newP);
    void AddTask(Task t);
    void DoExitTask();
    void RunNext();
    uint64_t GetCurrentTaskEntryPoint();
};

extern TaskManager* GlobalTaskManager;
#include "cooperative.h"
#include "../libc/stdio.h"
#include "../io/serial.h"

TaskManager* GlobalTaskManager;

void TaskManager::DoYeld(uint64_t newP) {
    tasks[currentTask].instructionPointer = newP;
    RunNext();
}

void TaskManager::AddTask(Task t) {
    tasks[taskNum++] = t;
    GlobalTableManager.MapUserspaceMemory((void*)t.instructionPointer);
    GlobalTableManager.MapUserspaceMemory((void*)t.stack);
}

void TaskManager::DoExitTask() {
    tasks[currentTask].state = STATE_HALTED;
}

void TaskManager::RunNext() {
    currentTask++;
    if(taskNum <= currentTask) currentTask = 0;
    if(tasks[currentTask].state == STATE_HALTED) RunNext();

    Task t = tasks[currentTask];
    lastTask = (char*)t.name;
    void* rip = (void*)t.instructionPointer;
    GlobalTableManager.MapUserspaceMemory((void*)t.instructionPointer);
    GlobalTableManager.MapUserspaceMemory((void*)t.stack);
    RunInUserspace(rip,t.stack + USERSPACE_STACK_SIZE - 16);
}
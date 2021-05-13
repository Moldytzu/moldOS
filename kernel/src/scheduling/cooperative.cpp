#include "cooperative.h"
#include "../libc/stdio.h"

TaskManager* GlobalTaskManager;

void TaskManager::DoYeld(uint64_t newP) {
    tasks[currentTask].instructionPointer = newP;
    currentTask++;
    if(taskNum <= currentTask) currentTask = 0;
    void* rip = (void*)(uint64_t*)tasks[currentTask].instructionPointer;
    RunInUserspace(rip,tasks[currentTask].stack + 4096 - 8);
    //((void(*)())rip)();
}

void TaskManager::AddTask(Task t) {
    tasks[taskNum++] = t;
    GlobalTableManager.MapUserspaceMemory((void*)t.instructionPointer);
    GlobalTableManager.MapUserspaceMemory((void*)t.stack);
}
#include "cooperative.h"
#include "../libc/stdio.h"

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
    tasks[currentTask].instructionPointer = 0;
    tasks[currentTask].stack = (uint64_t*)0;
    //tasks[currentTask] = tasks[taskNum-1];
    //taskNum--;
}

void TaskManager::RunNext() {
    currentTask++;
    if(taskNum <= currentTask) currentTask = 0;
    void* rip = (void*)(uint64_t*)tasks[currentTask].instructionPointer;
    if(rip == (void*)0) RunNext();
    GlobalDisplay->update();
    GlobalTableManager.MapUserspaceMemory((void*)tasks[currentTask].stack - USERSPACE_STACK_SIZE + ((uint64_t)tasks[currentTask].stack % USERSPACE_STACK_SIZE));
    //RunInUserspace(rip,tasks[currentTask].stack + USERSPACE_STACK_SIZE - ((uint64_t)tasks[currentTask].stack % USERSPACE_STACK_SIZE) - 16);
    RunInUserspaceMultiTasking(rip);
    //((void(*)())rip)();
}
#include "cooperative.h"
#include "../libc/stdio.h"
#include "../io/serial.h"

TaskManager* GlobalTaskManager;

void TaskManager::DoYeld(uint64_t newP) {
    tasks[currentTask].nextInstructionPointer = newP;
    RunNext();
}

void TaskManager::AddTask(Task t) {
    InternalTask newT = {t.instructionPointer,t.instructionPointer,t.stack,t.name,t.state,(uint64_t)taskNum};
    tasks[taskNum++] = newT;
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

    InternalTask t = tasks[currentTask];

#ifdef Debugging_Scheduler
    GlobalCOM1->Write("Current task: ",t.name,"  Entry point: ",inttohstr(t.binaryEntryPoint),"  New IP: ");
    GlobalCOM1->Write(inttohstr(t.nextInstructionPointer),"\n");
#endif

    lastTask = (char*)t.name;
    void* rip = (void*)t.nextInstructionPointer;
    GlobalTableManager.MapUserspaceMemory((void*)t.nextInstructionPointer);
    GlobalTableManager.MapUserspaceMemory((void*)t.stack);
    RunInUserspace(rip,t.stack + USERSPACE_STACK_SIZE);
}

uint64_t TaskManager::GetCurrentTaskEntryPoint() {
    return tasks[currentTask].binaryEntryPoint;
}
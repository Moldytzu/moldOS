#include "taskmgr.h"

TaskManager* GlobalTaskManager;

void TaskManager::Schedule(InterruptStack* registers) {
    //Save registers
    if(tasks[currentTask].state != STATE_HALTED) {
        #ifdef Debugging_Scheduler 
            GlobalCOM1->Write("Saving registers for task: ",tasks[currentTask].name,"\n");
        #endif

        asmmemcpy(&tasks[currentTask].registers,(void*)registers,sizeof(InterruptStack));
    }

    //Get next task
    do {
        currentTask++;
        if(taskNum <= currentTask) currentTask = 0;
    }while(tasks[currentTask].state == STATE_HALTED); //load next task that isn't halted

    //Load new registers
    #ifdef Debugging_Scheduler 
        GlobalCOM1->Write("Loading registers for task: ",tasks[currentTask].name,"\n");
    #endif
    int kernelStack = registers->KernelRsp;
    asmmemcpy((void*)registers,&tasks[currentTask].registers,sizeof(InterruptStack));
    registers->KernelRsp = kernelStack;
}

void TaskManager::AddTask(void* entry,void* stack,const char* name,uint8_t privilege) {
    Task task;
    task.instructionPointer = (uint64_t)entry;
    task.entryPoint = (uint64_t)entry;
    task.privilege = privilege;
    task.stack = (uint64_t*)(stack+4096);
    task.state = STATE_RUNNING;
    task.name = name;
    task.registers = {0};
    task.registers.rip = (uint64_t)entry;
    task.registers.rsp = (uint64_t)stack;
    task.registers.rflags = 0x202; //interrupts
    task.registers.cs = GDTInfoSelectors.UCode;
    task.registers.ss = GDTInfoSelectors.UData;
    tasks[taskNum++] = task;
    GlobalTableManager.MapUserspaceMemory(entry);
    GlobalTableManager.MapUserspaceMemory(stack);
    
}

void TaskManager::ExitCurrentTask() {
    GlobalCOM1->Write("The task ",tasks[currentTask].name," is going to a halt.\n");
    tasks[currentTask].state = STATE_HALTED;
}
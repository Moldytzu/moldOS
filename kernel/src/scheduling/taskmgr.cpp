#include "taskmgr.h"

TaskManager* GlobalTaskManager;

void TaskManager::Schedule(InterruptStack* registers) {
    //Save registers
    if(tasks[currentTask].state != STATE_HALTED) {
        GlobalCOM1->Write("Saving registers for task: ",tasks[currentTask].name,"\n");
        tasks[currentTask].registers.rax = registers->rax;
        tasks[currentTask].registers.rcx = registers->rcx;
        tasks[currentTask].registers.rdx = registers->rdx;
        tasks[currentTask].registers.rsi = registers->rsi;
        tasks[currentTask].registers.rdi = registers->rdi;
        tasks[currentTask].registers.rbp = registers->rbp;
        tasks[currentTask].registers.rbx = registers->rbx;
        tasks[currentTask].registers.r8 = registers->r8;
        tasks[currentTask].registers.r9 = registers->r9;
        tasks[currentTask].registers.r10 = registers->r10;
        tasks[currentTask].registers.r11 = registers->r11;
        tasks[currentTask].registers.r12 = registers->r12;
        tasks[currentTask].registers.r13 = registers->r13;
        tasks[currentTask].registers.r14 = registers->r14;
        tasks[currentTask].registers.r15 = registers->r15;
        tasks[currentTask].registers.rip = registers->rip;
        tasks[currentTask].registers.cs = registers->cs;
        tasks[currentTask].registers.rflags = registers->rflags;
        tasks[currentTask].registers.rsp = registers->rsp;
        tasks[currentTask].registers.ss = registers->ss;
    }

    //Get next task
    currentTask++;
    if(taskNum <= currentTask) currentTask = 0;

    //Load new registers
    GlobalCOM1->Write("Loading registers for task: ",tasks[currentTask].name,"\n");
    registers->rax = tasks[currentTask].registers.rax;
    registers->rcx = tasks[currentTask].registers.rcx;
    registers->rdx = tasks[currentTask].registers.rdx;
    registers->rsi = tasks[currentTask].registers.rsi;
    registers->rdi = tasks[currentTask].registers.rdi;
    registers->rbp = tasks[currentTask].registers.rbp;
    registers->rbx = tasks[currentTask].registers.rbx;
    registers->r8 = tasks[currentTask].registers.r8;
    registers->r9 = tasks[currentTask].registers.r9;
    registers->r10 = tasks[currentTask].registers.r10;
    registers->r11 = tasks[currentTask].registers.r11;
    registers->r12 = tasks[currentTask].registers.r12;
    registers->r13 = tasks[currentTask].registers.r13;
    registers->r14 = tasks[currentTask].registers.r14;
    registers->r15 = tasks[currentTask].registers.r15;
    registers->rip = tasks[currentTask].registers.rip;
    registers->cs = tasks[currentTask].registers.cs;
    registers->rflags = tasks[currentTask].registers.rflags;
    registers->rsp = tasks[currentTask].registers.rsp; 
    registers->ss = tasks[currentTask].registers.ss;
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
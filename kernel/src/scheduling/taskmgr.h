#pragma once
#include <stdint.h>
#include "../memory/PageTableManager.h"
#include "../cpu/gdt.h"
#include "../io/serial.h"
#include "../memory/memory.h"
#include "../settings.h"

#define STATE_RUNNING 1
#define STATE_HALTED 0

#define TASK_USER 1
#define TASK_ADMIN 2
#define TASK_SYSTEM 3

struct InterruptStack {
    uint64_t rax; uint64_t rbx; uint64_t rcx; uint64_t rdx; uint64_t KernelRsp; uint64_t rsi; uint64_t rdi; uint64_t rbp;
    uint64_t r8; uint64_t r9; uint64_t r10; uint64_t r11; uint64_t r12; uint64_t r13; uint64_t r14; uint64_t r15; uint64_t rip; 
    uint64_t cs; uint64_t rflags; uint64_t rsp; uint64_t ss;
}__attribute__((packed));

struct Task {
    uint64_t entryPoint;
    const char* name;
    int state;
    uint8_t privilege;
    InterruptStack registers;
};

class TaskManager {
 public:
    Task tasks[1024];
    int currentTask = 0;
    int taskNum = 0;
    int isEnabled = 0;

    void Schedule(InterruptStack* registers);
    void AddTask(void* entry,void* stack,const char* name,uint8_t privilege);
    void ExitCurrentTask();
};

extern TaskManager* GlobalTaskManager;
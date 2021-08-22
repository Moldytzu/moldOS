#include "panic.h"

void KernelPanic(const char* Message) {
    asm volatile ("cli"); 
    GlobalCOM1->Write(Message,"\n");
    LogError("A kernel panic occured!");
    LogError(Message);
    LogError(GlobalTaskManager->tasks[GlobalTaskManager->currentTask].name);
    GlobalDisplay->update();
    while(1) {
        asm volatile ("hlt");
    }
}
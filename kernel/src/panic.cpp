#include "panic.h"

void KernelPanic(const char* Message) {
    GlobalCOM1->Write(Message,"\n");
    LogError("A kernel panic occured!");
    LogError(Message);
    LogError("Task: ");
    LogError(GlobalTaskManager->lastTask);
    GlobalDisplay->update();
    asm volatile ("cli");
    while(1) {
        asm volatile ("hlt");
    }
}
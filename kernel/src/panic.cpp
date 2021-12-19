#include "panic.h"

void KernelPanic(const char* Message)
{
    asm volatile ("cli");
    SerialWrite(Message,"\n");
    LogError("A kernel panic occured!");
    LogError(Message);
    LogError(GlobalTaskManager->tasks[GlobalTaskManager->currentTask].name);
    GlobalDisplay->clearScreen(0);
    GlobalDisplay->puts(VirtualTerminals[CurrentTerminal].buffer);
    GlobalDisplay->update();
    while(1)
    {
        asm volatile ("hlt");
    }
}
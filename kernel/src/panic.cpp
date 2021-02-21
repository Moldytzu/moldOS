#include "panic.h"

void KernelPanic(const char* Message) {
    GlobalCOM1->Write(Message);
    GlobalDisplay->clearScreen(0xff0000);
    GlobalDisplay->setCursorPos(0,0);
    GlobalDisplay->setColour(0xffffff);
    GlobalDisplay->puts("Something didn't work as intended and I needed to stop everything. I'm sorry! :(\nMessage: ");
    GlobalDisplay->puts(Message);
    GlobalDisplay->update();
    while(1) {
        asm volatile ("hlt");
    }
}
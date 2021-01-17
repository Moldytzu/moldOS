#include "panic.h"

void KernelPanic(const char* Message) {
    GlobalDisplay->clearScreen(0xff0000);
    GlobalDisplay->setCursorPos(0,0);
    GlobalDisplay->setColour(0xffffff);
    GlobalDisplay->puts("Something didn't work as intended and I needed to stop everything. I'm sorry! :(\nMessage: ");
    GlobalDisplay->puts(Message);
    GlobalDisplay->update();
    GlobalDisplay->update();
    GlobalDisplay->update();
    GlobalDisplay->update();
    while(1);    
}
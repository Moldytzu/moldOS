#include "userspace.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"
#include "../drivers/keyboard/keyboarddriver.h"
#include "../misc/logging/log.h"
#include "../scheduling/cooperative.h"

void ConsoleWrite(char* text) {
    printf("%s",text);
    GlobalDisplay->update();
}

void SerialWrite(char* text) {
    GlobalCOM1->Write(text);
}

void Exit(uint64_t code) {
    printf("\nProgram exit code: %co%u%co\n",YELLOW,code,WHITE);
    GlobalDisplay->update();
    GlobalTaskManager->DoExitTask();
    GlobalTaskManager->RunNext();
}

void Yeld(uint64_t ip) {
    GlobalTaskManager->DoYeld(ip);
}

void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3) {
    switch (syscall)
    {
    case SYSCALL_WRITE:
        if(arg2 == SOUT) SerialWrite((char*)(uint64_t*)arg1);
        else ConsoleWrite((char*)(uint64_t*)arg1);
        break;
    case SYSCALL_EXIT:
        Exit(arg1);
        break;
    case SYSCALL_YELD:
        Yeld(arg1);
        break;

    default:
        LogWarn(inttohstr((uint64_t)syscall));
        LogWarn("Unknown syscall! Forcing userspace exit!");
        Exit(2550);
        break;
    }
}
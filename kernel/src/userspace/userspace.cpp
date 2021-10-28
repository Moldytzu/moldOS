#include "userspace.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"
#include "../drivers/keyboard/keyboarddriver.h"
#include "../misc/logging/log.h"
#include "../scheduling/taskmgr.h"

void SysConsoleWrite(char text) {
    printf("%c",text);
}

void SysSerialWrite(char text) {
    SerialWrite(text);
}

void Exit(uint64_t code) {
    printf("\nProgram exit code: %co%u%co\n",YELLOW,code,WHITE);
    GlobalTaskManager->ExitCurrentTask();
}

                    //    RDX        RDI        RSI                       R8
void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3) {
    switch (syscall)
    {
    case SYSCALL_WRITE:
        if(arg2 == SOUT) SysSerialWrite((char)arg1);
        else SysConsoleWrite((char)arg1);
        break;
    case SYSCALL_EXIT:
        Exit(arg1);
        break;
    case SYSCALL_UPDATESCREEN:
        if(GlobalTaskManager->tasks[GlobalTaskManager->currentTask].privilege == TASK_SYSTEM)
            GlobalDisplay->update();
        else
            Exit(ERROR_ACCESS_DENIED);
        break;
    default:
        LogWarn(inttohstr((uint64_t)syscall));
        LogWarn("Unknown syscall! Forcing userspace exit!");
        Exit(ERROR_UNKNOWN_SYSCALL);
        break;
    }
}
#include "userspace.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"
#include "../drivers/keyboard/keyboarddriver.h"
#include "../misc/logging/log.h"
#include "../scheduling/taskmgr.h"

void ConsoleWrite(uint64_t* text,uint64_t resource) {
    if(resource)
        printf("%s",(GlobalTaskManager->tasks[GlobalTaskManager->currentTask].entryPoint+(uint64_t)text));
    else
        printf("%s",text);
    //GlobalDisplay->update();
}

void SerialWrite(char* text,uint64_t resource) {
    if(resource)
        SerialWrite((GlobalTaskManager->tasks[GlobalTaskManager->currentTask].entryPoint+(uint64_t)text));
    else
        SerialWrite(text);
}

void Exit(uint64_t code) {
    printf("\nProgram exit code: %co%u%co\n",YELLOW,code,WHITE);
    //GlobalDisplay->update();
    GlobalTaskManager->ExitCurrentTask();
}

                    //    RDX        RDI        RSI                       R8
void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3) {
    switch (syscall)
    {
    case SYSCALL_WRITE:
        if(arg2 == SOUT) SerialWrite((char*)(uint64_t*)arg1,arg3);
        else ConsoleWrite((uint64_t*)arg1,arg3);
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
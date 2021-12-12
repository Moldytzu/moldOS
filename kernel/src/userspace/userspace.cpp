#include <userspace/userspace.h>

#define CurrentTask GlobalTaskManager->tasks[GlobalTaskManager->currentTask]

void SysConsoleWrite(char text) {
    printf(CurrentTask.terminal,"%c",text);
}

void SysSerialWrite(char text) {
    SerialWrite(text);
}

void Exit(uint64_t code) {
    printf(CurrentTask.terminal,"\nProgram exit code: %co%u%co\n",YELLOW,code,WHITE);
    GlobalTaskManager->ExitCurrentTask();
}

                    //    RDX        RDI        RSI                       R8
uint64_t SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3) {
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
        if(CurrentTask.privilege == TASK_SYSTEM)
            GlobalDisplay->update();
        else
            return ERROR_ACCESS_DENIED;
        break;
    case SYSCALL_GETTERMINALOUTPUT:
        return (uint64_t)(uint64_t*)VirtualTerminals[arg1].buffer;
        break;
    case SYSCALL_GETPROCESSTERMINAL:
        return CurrentTask.terminal;
        break;
    default:
        LogWarn(inttohstr((uint64_t)syscall));
        LogWarn("Unknown syscall!");
        return ERROR_UNKNOWN_SYSCALL;
        break;
    }
    return 512;
}
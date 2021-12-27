#include <userspace/userspace.h>

#define CurrentTask GlobalTaskManager->tasks[GlobalTaskManager->currentTask]

void SysConsoleWrite(char text)
{
    printf(CurrentTask.terminal,"%c",text);
}

void SysSerialWrite(char text)
{
    SerialWrite(text);
}

void Exit(uint64_t code)
{
    printf(CurrentTask.terminal,"\nProgram exit code: %co%u%co\n",YELLOW,code,WHITE);
    GlobalTaskManager->ExitCurrentTask();
}

//                                  RDX            RDI             RSI                                  R8             R9
uint64_t SyscallHandler(uint64_t syscall, uint64_t arg1, uint64_t arg2, uint64_t doNotModify, uint64_t arg3, uint64_t arg4)
{
    switch (syscall)
    {
    case SYSCALL_WRITE:
        if(arg2 == SOUT) SysSerialWrite((char)arg1);
        else SysConsoleWrite((char)arg1);
        break;
    case SYSCALL_EXIT:
        Exit(arg1);
        break;
    case SYSCALL_OPEN:
    {
        FileDescriptor* descriptor = (FileDescriptor*)GlobalAllocator.RequestPage();
        memset((void*)descriptor,0,4096);
        GlobalTableManager.MapUserspaceMemory((void*)descriptor);
        fastmemcpy(descriptor,VFSOpenFile((const char*)(void*)(uint64_t)arg1),sizeof(FileDescriptor));
        return (uint64_t)(void*)descriptor;
        break;
    }
    case SYSCALL_ALLOCATEMEMORY:
    {
        if(!arg1) return 0;
        CurrentTask.memoryUse += arg1 * 4096; //it allocates pages, each being 4 kilobytes
        void* mem = GlobalAllocator.RequestPages(arg1);
        for(int i = 0; i<=4096*arg1; i++)
            GlobalTableManager.MapUserspaceMemory((void*)((uint64_t)mem+i));
        #ifdef Debugging_Scheduler
            SerialWrite(CurrentTask.name," is now using in total ", inttostr(CurrentTask.memoryUse/1024), " KB, while the whole operating system using only ");
            SerialWrite(inttostr(GlobalAllocator.GetUsedRAM()/1024/1024), " MB\n");
        #endif
        return (uint64_t)mem;
        break;
    }
    case SYSCALL_READ:
    {
        fastmemcpy((void*)arg1,VFSReadFile((FileDescriptor*)(void*)arg2),arg3);
        break;
    }
    case SYSCALL_GETTERMINALOUTPUT:
        return (uint64_t)(uint64_t*)VirtualTerminals[arg1].buffer;
        break;
    case SYSCALL_GETPROCESSTERMINAL:
        return CurrentTask.terminal;
        break;
    case SYSCALL_GETPIDBYNAME:
    {
        for(int i = 0; i<1024; i++)
        {
            if(memcmp((const void*)arg1,GlobalTaskManager->tasks[i].name,strlen(GlobalTaskManager->tasks[i].name))==0)
                return i;
        }
        return 0xFFFFFFFFFFFFFFFF;
        break;
    }
    case SYSCALL_GETPID:
        return GlobalTaskManager->currentTask;
        break;
    case SYSCALL_GETTERMINALBYPID:
        return GlobalTaskManager->tasks[arg1].terminal;
        break;
    default:
        LogWarn(inttohstr((uint64_t)syscall));
        LogWarn("Unknown syscall!");
        return ERROR_UNKNOWN_SYSCALL;
        break;
    }
    return 512;
}
#include <kernelInit.h>

/*
Special Thanks to:
- @borrrden - he fixed my buggy keyboard handler
- @AbsurdPoncho - if he didn't do a osdev series I wouldn't started this project
- @nothotscott - he has a very good tutorial on debugging that helped me (https://www.youtube.com/watch?v=llP7zB8HTls) and for some userspace tips
- @keepkonect - he's my friend that helped me do this project
- @pitust - he told me some things that I missed when doing the tss, and told me some terminology that I should fix
*/

/*
To-do list:
- better elf loader
- llfs version 2
- basic shell
- gui infrastructure
- window manager
*/

extern "C" void IdleTask();

extern "C" int kernelMain(BootInfo* binfo)
{
    InitDrivers(binfo);

    if((void*)binfo->RamFS == (void*)0)
    {
        LogError("No ram filesystem loaded!");
        while(1);
    }

    if(!LLFSCheck(binfo->RamFS))
    {
        LogError("Corrupt or unsupported ram filesystem!");
        while(1);
    }

    printf("SMBIOS Address: %x\n",binfo->SMBIOS);
    printf("SMBIOS Signature: %c%c%c%c\n",binfo->SMBIOS->Signature[0],binfo->SMBIOS->Signature[1],binfo->SMBIOS->Signature[2],binfo->SMBIOS->Signature[3]);

    SMBiosParse(binfo->SMBIOS);

    //llfs
    uint64_t fssize = LLFSGetFileSystemSize(binfo->RamFS);
    LLFSSource = (LLFSHeader*)GlobalAllocator.RequestPages(fssize/4096+1);
    fastmemcpy(LLFSSource,binfo->RamFS,fssize);
    LLFSMap(LLFSSource); //map as user memory

    VFSSource = VFS_SOURCE_RAMFS;

    //and lock the pages
    GlobalAllocator.LockPages(LLFSSource,fssize/4096+1);

    //init filesystem
    VFSInit();

    //map files from llfs into the vfs
    LLFSEntry* firstEntry = (LLFSEntry*)((uint64_t)LLFSSource+sizeof(LLFSHeader));
    uint64_t fsize = sizeof(LLFSHeader);
    for(int i = 0; i<LLFSSource->Entries; i++)
    {
        FileDescriptor descriptor;
        fastmemcpy(descriptor.path,firstEntry->Filename,368);
        descriptor.source = VFS_SOURCE_RAMFS;
        VFSAdd(descriptor);
        firstEntry = (LLFSEntry*)((uint64_t)firstEntry+sizeof(LLFSEntry)+firstEntry->FileSize);
    }

    printf("VFS total entries: %u\n",VFSTotalEntries);

    //init taskmanager
    TaskManager tmgr;
    GlobalTaskManager = &tmgr;

    void* lastAddr = malloc(1);
    void* offset = malloc(0x0000100000200000-(uint64_t)lastAddr-sizeof(HeapSegHdr)*3);
    void* address = malloc(1*1024*1024); //1 mb
    //userspace
    void* moldInit = LoadELFExecutable("/minit.melf",false);

    if(moldInit == (void*)1 || moldInit == (void*)2)
    {
        KernelPanic("moldInit is missing or corrupt.");
    }

    GlobalTableManager.MapUserspaceMemory((void*)IdleTask);
    GlobalTaskManager->AddTask((void*)IdleTask,GenerateUserspaceStack(),"Idle Task",TASK_SYSTEM);
    GlobalTaskManager->AddTask(moldInit,GenerateUserspaceStack(),"moldInit",TASK_USER);
    //jump in the userspace
    GlobalTaskManager->isEnabled = 1;
    CurrentTerminal = 2;
    RunInUserspace((void*)IdleTask,GenerateUserspaceStack());

    while(1);

    return 0;
}
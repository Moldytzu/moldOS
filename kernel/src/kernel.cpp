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

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);

	if((void*)binfo->RamFS == (void*)0) {
		LogError("No ram filesystem loaded!");
		while(1);
	}

	if(!LLFSCheck(binfo->RamFS)) {
		LogError("Corrupt or unsupported ram filesystem!");
		while(1);
	}

	//llfs
	uint64_t fssize = LLFSGetFileSystemSize(binfo->RamFS);
	LLFSHeader* llfs = (LLFSHeader*)GlobalAllocator.RequestPages(fssize/4096+1);
	memcpy(llfs,binfo->RamFS,fssize);
	LLFSMap(llfs); //map as user memory

	//and lock the pages
	GlobalAllocator.LockPages(llfs,fssize/4096+1);

	TaskManager tmgr;
	GlobalTaskManager = &tmgr;
	
	void* lastAddr = malloc(1);
	void* offset = malloc(0x0000100000200000-(uint64_t)lastAddr-sizeof(HeapSegHdr)*3);
	void* address = malloc(1*1024*1024); //1 mb

	//userspace
	void* moldInit = LoadELFExecutable(llfs,"minit.melf      ",0);
	
	if(moldInit == (void*)1 || moldInit == (void*)2) {
		KernelPanic("moldInit is missing or corrupt.");
	}

	GlobalTableManager.MapUserspaceMemory((void*)IdleTask);
	GlobalTaskManager->AddTask((void*)IdleTask,GenerateUserspaceStack(),"Idle Task",TASK_SYSTEM);
	GlobalTaskManager->AddTask(moldInit,GenerateUserspaceStack(),"moldInit",TASK_USER);

	//jump in the userspace
	GlobalTaskManager->isEnabled = 1;
	RunInUserspace((void*)IdleTask,GenerateUserspaceStack());

	while(1);

	return 0;
} 
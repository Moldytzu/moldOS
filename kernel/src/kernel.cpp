#include "kernelInit.h"
#include "userspace/apps.h"

/*
Special Thanks to:
- @borrrden - he fixed my buggy keyboard handler
- @AbsurdPoncho - if he didn't do a osdev series i wouldn't started this project
- @nothotscott - he has a very good tutorial on debugging that helped me (https://www.youtube.com/watch?v=llP7zB8HTls) and for some userspace tips
- @keepkonect - he's my friend that helped me do this project
*/

/*
To-do list:
- elf program loader
*/

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);
	PITSetDivisor(0xFFFF);

	LLFSHeader* llfs = binfo->RamFS;

	if((void*)llfs == (void*)0) {
		LogError("No ram filesystem loaded!");
		while(1);
	}

	if(!LLFSCheck(llfs)) {
		LogError("Corrupt or unsupported ram filesystem!");
		while(1);
	}


	//map the llfs as userspace memory
	uint64_t fssize = LLFSGetFileSystemSize(llfs);
	for(int i = 0;i<fssize/4096+1;i++) {
		GlobalTableManager.MapUserspaceMemory((void*)llfs+(i*4096));
	}
	//and lock the pages
	GlobalAllocator.LockPages(llfs,fssize/4096+1);

	//userspace stuff
	Task userApp = {(uint64_t)(void*)UserAPP,(uint64_t*)GenerateUserspaceStack(),"Sample User Application",STATE_RUNNING};
	Task idleTask = {(uint64_t)(void*)IdleTask,(uint64_t*)GenerateUserspaceStack(),"Idle Task",STATE_RUNNING};
	Task initApp = {(uint64_t)(void*)LLInit,(uint64_t*)GenerateUserspaceStack(),"LLInit",STATE_RUNNING};
	//Task initApp = {(uint64_t)LoadELFExecutable(llfs,"llinit.llexec   "),(uint64_t*)GenerateUserspaceStack(),"LLInit",STATE_RUNNING};


	GlobalTaskManager->AddTask(initApp);
	GlobalTaskManager->AddTask(idleTask);

	GlobalTaskManager->AddTask(userApp);

	//jump in the userspace
	RunInUserspace((void*)LLInit,(void*)(initApp.stack+USERSPACE_STACK_SIZE));

	while(1);

	return 0;
} 
#include "kernelInit.h"
#include "userspace/apps.h"

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
- preemptive multitasking (CRITICAL)
- llfs version 2
- llexec header
- basic shell
- window manager
- gui infrastructure
*/

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);

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
	Task idleTask = {(uint64_t)(void*)IdleTask,(uint64_t*)GenerateUserspaceStack(),"Idle Task",STATE_RUNNING,TASK_USER};
	Task initApp = {(uint64_t)LoadFlatBinary(llfs,"llinit.llexec   "),(uint64_t*)GenerateUserspaceStack(),"LLInit",STATE_RUNNING,TASK_ADMIN};

	GlobalTaskManager->AddTask(idleTask);
	GlobalTaskManager->AddTask(initApp);

	//jump in the userspace
	RunInUserspace((void*)idleTask.instructionPointer,(void*)(idleTask.stack+USERSPACE_STACK_SIZE));

	LOOP;

	return 0;
} 
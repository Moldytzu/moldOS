#include "kernelInit.h"
#include "userspace/apps.h"

/*
Special Thanks to:
- @borrrden - he fixed my buggy keyboard handler
- @AbsurdPoncho - if he didn't do a osdev series i won't started this project
- @nothotscott - he has a very good tutorial on debugging that helped me (https://www.youtube.com/watch?v=llP7zB8HTls) and for some userspace tips
- @keepkonect - he's my friend that helped me do this project
*/

/*
To-do list:
- ramfs
- fat32
- program loader
*/

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);

	Task userApp = {(uint64_t)(void*)UserAPP,(uint64_t*)GenerateUserspaceStack()};
	Task idleTask = {(uint64_t)(void*)IdleTask,(uint64_t*)GenerateUserspaceStack()};
	Task initApp = {(uint64_t)(void*)LLInit,(uint64_t*)GenerateUserspaceStack()};

	GlobalTaskManager->AddTask(initApp);
	GlobalTaskManager->AddTask(idleTask);

	GlobalTaskManager->AddTask(userApp);

	//jump in the userspace
	RunInUserspace((void*)LLInit,(void*)(userApp.stack+USERSPACE_STACK_SIZE-8));

	while(1);

	return 0;
} 
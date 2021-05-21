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

	LLFSHeader* llfs = binfo->RamFS;

	if((void*)llfs == (void*)0) {
		LogError("No ram filesystem loaded!");
		while(1);
	}

	//map the llfs as userspace memory
	uint64_t fssize = LLFSGetFileSystemSize(llfs);
	for(int i = 0;i<fssize/4096+1;i++) {
		GlobalTableManager.MapUserspaceMemory((void*)llfs+(i*4096));
	}

	printf("Signature: %c%c%c%c\n",llfs->Signature[0],llfs->Signature[1],llfs->Signature[2],llfs->Signature[3]);
	printf("Entries: %u\n",llfs->Entries);
	printf("Version: %u\n",llfs->Version);

	LLFSEntry* entry = LLFSOpenFile(llfs,"fileA.txt       ");

	if((void*)entry != (void*)0) {
		printf("File name: ");
		for(int i = 0;i<16;i++)
			printf("%c",entry->Filename[i]);
		printf("\n");

		char* contents = (char*)LLFSReadFile(entry);

		printf("File contents: ");
		for(int i = 0;i<entry->FileSize;i++)
			printf("%c",contents[i]);
		printf("\n");
	}

	entry = LLFSOpenFile(llfs,"fileB.txt       ");

	if((void*)entry != (void*)0) {
		printf("File name: ");
		for(int i = 0;i<16;i++)
			printf("%c",entry->Filename[i]);
		printf("\n");

		char* contents = (char*)LLFSReadFile(entry);

		printf("File contents: ");
		for(int i = 0;i<entry->FileSize;i++)
			printf("%c",contents[i]);
		printf("\n");
	}

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
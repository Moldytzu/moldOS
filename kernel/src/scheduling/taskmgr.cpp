#include <scheduling/taskmgr.h>

TaskManager* GlobalTaskManager;

int lastTerminal = 1;

void TaskManager::Schedule(InterruptStack* registers)
{
    if(!isEnabled) return;
    //Save registers
#ifdef Debugging_Scheduler
    SerialWrite("Saving registers for task: ",tasks[currentTask].name,"\n");
#endif

    fastmemcpy(&tasks[currentTask].registers,(void*)registers,sizeof(InterruptStack));

    //Get next task
    do
    {
        currentTask++;
        if(taskNum <= currentTask) currentTask = 0;
    }
    while(tasks[currentTask].state == STATE_HALTED);  //load next task that isn't halted

    //Load new registers
#ifdef Debugging_Scheduler
    SerialWrite("Loading registers for task: ",tasks[currentTask].name,"\n");
#endif
    int kernelStack = registers->KernelRsp;
    fastmemcpy((void*)registers,&tasks[currentTask].registers,sizeof(InterruptStack));
    registers->KernelRsp = kernelStack;
}

void TaskManager::AddTask(void* entry,void* stack,const char* name,uint8_t privilege,uint64_t executableSize)
{
    Task task;
    task.entryPoint = (uint64_t)entry;
    task.privilege = privilege;
    task.state = STATE_RUNNING;
    task.terminal = lastTerminal++;
    task.name = name;
    task.memoryUse = executableSize;

    memset(&task.registers,0,sizeof(InterruptStack));
    task.registers.rip = (uint64_t)entry;
    task.registers.rsp = (uint64_t)stack+0x1000;
    task.registers.rflags = 0x202; //interrupts
    task.registers.cs = GDTInfoSelectors.UCode;
    task.registers.ss = GDTInfoSelectors.UData;

    #ifdef Debugging_Scheduler
        SerialWrite("The task ",name," consummes ",inttostr(executableSize/1024)," KB\n");
    #endif

    tasks[taskNum++] = task;

    GlobalTableManager.MapUserspaceMemory(entry);
    GlobalTableManager.MapUserspaceMemory(stack);
}

void TaskManager::ExitCurrentTask()
{
    #ifdef Debugging_Scheduler
        SerialWrite("The task ",tasks[currentTask].name," is going to a halt.\n");
    #endif
    tasks[currentTask].state = STATE_HALTED;
}
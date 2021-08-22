#pragma once

//standard definitions
#include "stddef.h"
#include "stdint.h"

//drivers
#include "drivers/display/displaydriver.h" //display
#include "drivers/pci/pci.h" //pci
#include "drivers/rtc/rtc.h" //real time clock
#include "drivers/keyboard/keyboarddriver.h" // keyboard
#include "drivers/mouse/mouse.h" //ps/2 mouse
#include "drivers/ahci/ahci.h" //disk

//misc
#include "misc/bitmap/bitmap.h" //bitmap
#include "misc/colors.h" //colors
#include "misc/logging/log.h" //logging
#include "misc/power/acpi.h" //acpi

//io
#include "io/serial.h" //serial port
#include "io/msr.h" //model specific registers

//memory
#include "memory/memory.h" //memory
#include "memory/PageFrameAllocator.h" //pfa
#include "memory/PageMapIndexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/PageTableManager.h" //ptm
#include "memory/heap.h" //heap

//cpu
#include "cpu/idt.h" //idt
#include "cpu/intrerupts.h" //handlers
#include "cpu/cpu.h" //cpu
#include "cpu/gdt.h" //gdt
#include "cpu/tss.h" //tss

//libc
#include "libc/stdio.h" //standard i/o
#include "libc/math.h" //maths
#include "libc/time.h" //time

//scheduling
#include "scheduling/pit.h" //pit
#include "scheduling/taskmgr.h" //task manager

//userspace
#include "userspace/userspace.h" //userspace

//filesystem
#include "filesystem/llfs.h" //llfs

//elf
#include "elf/loader.h"

//settings
#include "settings.h"

#define DoubleBuffer

struct BootInfo {
	//display
	DisplayBuffer* GOPFrameBuffer;
	PSF1_FONT* Font;

	//memory
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;

    //acpi
    RSDP2* RSDP;

    //llfs
    LLFSHeader* RamFS;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern uint64_t _BssStart;
extern uint64_t _BssEnd;

char** CPUFeatures;

BootInfo* GlobalInfo;

DisplayDriver display;
CPU cpu;
PCI pci;
RealTimeClock rtc;
SerialPort com1;
DisplayBuffer* doubleBuffer;
Keyboard kb;
Mouse mouse;
ACPI acpi;

void* GenerateUserspaceStack() {
    void* Stack = GlobalAllocator.RequestPages(4);
    uint64_t StackSize = 0x1000*4;
    for (uint64_t t = (uint64_t)Stack; t < (uint64_t)(Stack + StackSize); t += 4096){
        GlobalTableManager.MapUserspaceMemory((void*)t);
    }
    return Stack;
}

void EnablePaging(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    GlobalTableManager = PageTableManager(PML4);

    for (uint64_t page = 0; page < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); page+= 0x1000){
        GlobalTableManager.MapMemory((void*)page, (void*)page);
    }

    uint64_t fbBase = (uint64_t)bootInfo->GOPFrameBuffer->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->GOPFrameBuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize / 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        GlobalTableManager.MapMemory((void*)t, (void*)t);
    }

    asm volatile ("mov %0, %%cr3" : : "r" (PML4));
}

void InitIntrerupts() {
	idt->Limit = 0x0FFF;
	idt->Offset = (uint64_t)GlobalAllocator.RequestPage();

    CreateIntrerupt((void*)PageFaultHandlerEntry,0xE,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)DoubleFaultHandlerEntry,0x8,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)GeneralProtectionFaultHandlerEntry,0xD,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)InvalideOpcodeHandlerEntry,0x6,IDT_TA_InterruptGate,0x08);

    CreateIntrerupt((void*)KBHandlerEntry,0x21,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)MSHandlerEntry,0x2C,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)PITHandlerEntry,0x20,IDT_TA_InterruptGate,0x08);

	asm volatile ("lidt %0" : : "m" (*idt));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111000); //mouse, keyboard, timer
    outportb(PIC2_DATA, 0b11101111);

    asm volatile("cli");
}

SDT* xsdt;
void InitACPI(BootInfo* bootInfo) {
    xsdt = (SDT*)(bootInfo->RSDP->XSDTAddress);

    MCFG* mcfg = (MCFG*)acpi.FindTable(xsdt,(char*)"MCFG");

    #ifndef Quiet
    LogInfo("Enumerating PCI");
    #endif
    pci.EnumeratePCI(mcfg);
    if(pci.DevicesIndex == 0) {
        LogWarn("No MCFG found or no PCI devices!");
        LogWarn("AHCI might not work.");
    }
}

void InitDrivers(BootInfo* bootInfo) {
    //clear the bss so it will not have random values at startup
    memset(&_BssStart,0,&_BssEnd-&_BssStart);
    
    GlobalInfo = bootInfo;
    
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    gdtInit();
	InitIntrerupts();
    PITSetFrequency(1);

    com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
    com1.Write("Kernel intialized the Serial Port!\n");

	EnablePaging(bootInfo);
    com1.Write("Enabled Paging!\n");

    mouse.Init();
    GlobalKeyboard = &kb;
    GlobalMouse = &mouse;

	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	

    InitializeHeap((void*)0x0000100000000000, 0x10);

    display.EmptyScreenBuffer = malloc(display.globalFrameBuffer->BufferSize);
    memset(display.EmptyScreenBuffer,0,display.globalFrameBuffer->BufferSize);

#ifdef DoubleBuffer
    doubleBuffer->BaseAddr = GlobalAllocator.RequestPages(display.globalFrameBuffer->BufferSize / 4096);
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;

    for (uint64_t t = (uint64_t)doubleBuffer->BaseAddr; t < doubleBuffer->BufferSize + (uint64_t)doubleBuffer->BaseAddr; t += 4096){
        GlobalTableManager.MapMemory((void*)t, (void*)t);
    }

	display.InitDoubleBuffer(doubleBuffer);
#else
    display.InitDoubleBuffer(bootInfo->GOPFrameBuffer);
#endif

	display.colour = WHITE;
	display.clearScreen(0);
	display.update();

	GlobalDisplay = &display;
	
    com1.Write("Kernel intialized the display!\n");

    #ifndef Quiet
    LogInfo("Initialized PS/2, Intrerupts, Display, Serial!");
    #endif

	CPUFeatures = cpu.getFeatures();
	#ifndef Quiet
    LogInfo("Detected CPU features!");
    #endif
    com1.Write("Kernel detected CPU features!\n");

    InitACPI(bootInfo);
    #ifndef Quiet
    LogInfo("Initialized ACPI!");
    #endif
    com1.Write("Kernel intialized ACPI!\n");


    EnableSCE();
    #ifndef Quiet
    LogInfo("Initialized Task Switching and System Calls!");

    LogInfo("Initialized Everything!");
    #endif
    com1.Write("Kernel intialized SCE!\n");


    com1.Write("Kernel finished loading in ",inttostr(TimeSinceBoot)," seconds!\n");
}
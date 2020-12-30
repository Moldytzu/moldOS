#pragma once

//definitii standard
#include "stddef.h"
#include "stdint.h"

//drivere
#include "../drivers/display/displaydriver.h" //display
#include "../drivers/sound/sounddriver.h" //sunet
#include "../drivers/pci/pci.h" //pci
#include "../drivers/rtc/rtc.h" //realtimeclock

//misc
#include "../misc/cstring.h" //miscuri pt string
#include "../misc/power.h" //power
#include "../misc/colors.h" //culori
#include "../misc/math.h" //matematica
#include "../misc/bitmap.h" //bitmap
#include "../misc/cpu.h" //cpu

//io
#include "../io/serial.h" //serial port
#include "../io/parallel.h" //parallel port

//memorie
#include "../memory/efiMemory.h" //memorie
#include "../memory/pagefileallocator.h" //pfa
#include "../memory/pagemapindexer.h" //pmi
#include "../memory/paging.h" //paging
#include "../memory/pagetablemanager.h" //ptm

struct BootInfo {
	DisplayDriver::framebuffer* framebuf;
	DisplayDriver::PSF1_FONT* font;
	void* PowerDownVoid;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

char** CPUFeatures;

DisplayDriver display;
Power power;
Sound sound;
CPU cpu;
PCI pci;
RealTimeClock rtc;
SerialPort com1;
Parallel paralel;
DisplayDriver::framebuffer* tmp;
PageTableManager pageTableManager = PageTableManager((PageTable*)0);

uint64_t mMapEntries;

void EnablePaging(BootInfo* bootInfo) {
	mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;
    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    uint64_t fbBase = (uint64_t)bootInfo->framebuf->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->framebuf->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    pageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t+= 0x1000){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    asm volatile ("mov %0, %%cr3" : : "r" (PML4));
}

void InitDrivers(BootInfo* bootInfo) {
	com1.Init();
	com1.ClearMonitor();
	com1.Write(SERIALWHITE,"Initialized Serial Port!\n");

	com1.Write("Initializing Display...\n");
	display.InitDisplayDriver(bootInfo->framebuf,bootInfo->font);	

	com1.Write("Enabling paging...\n");

	EnablePaging(bootInfo);

	com1.Write("Paging enabled!\n");

	memset(display.globalFrameBuffer->BaseAddr,0,display.globalFrameBuffer->BufferSize);

	tmp->BaseAddr = GlobalAllocator.RequestPage();
	tmp->BufferSize = display.globalFrameBuffer->BufferSize;
	tmp->Height = display.globalFrameBuffer->Height;
	tmp->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	tmp->Width = display.globalFrameBuffer->Width;

	GlobalAllocator.LockPages(tmp->BaseAddr, ((uint64_t)tmp->BufferSize / 4096) + 1);

	display.InitDoubleBuffer(tmp);
	com1.Write("Initialized Display!\n");
	
	display.clearScreen(DARKGRAY);

	display.setCursorPos(display.getWidth()/2-180, display.getHeight()/2-120);
	display.setColour(YELLOW);
	display.puts("/ \\   / \\   / \\   /  _ \\/ ___\\\n");
	display.setCursorPos(display.getWidth()/2-180, display.CursorPos.Y);
	display.puts("| |   | |   | |   | / \\||    \\\n");
	display.setCursorPos(display.getWidth()/2-180, display.CursorPos.Y);
	display.puts("| |_/\\| |_/\\| |_/\\| \\_/|\\___ |\n");
	display.setCursorPos(display.getWidth()/2-180, display.CursorPos.Y);
	display.puts("\\____/\\____/\\____/\\____/\\____/\n\n");
	display.setCursorPos(display.getWidth()/2-180, display.CursorPos.Y);
	display.puts("########");

	com1.Write("Initializing Power...\n");
	power.InitPower(bootInfo->PowerDownVoid);
	com1.Write("Initialized Power!\n");
	
	display.puts("#######");

	com1.Write("Detecting PCI devices...\n");
	pci.detectDevices();
	com1.Write("Detected ", inttostr(pci.DeviceCount), " PCI devices!\n");

	display.puts("########");

	com1.Write("Detecting CPU features...\n");
	CPUFeatures = cpu.getFeatures();
	com1.Write("Detected ",inttostr(cpu.cpuFeatures)," CPU features!\n");

	display.puts("#######\n");

	display.setColour(WHITE);
	display.clearScreen(0);
}

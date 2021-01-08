#pragma once

//definitii standard
#include "stddef.h"
#include "stdint.h"

//drivere
#include "drivers/display/displaydriver.h" //display
#include "drivers/sound/sounddriver.h" //sunet
#include "drivers/pci/pci.h" //pci
#include "drivers/rtc/rtc.h" //realtimeclock

//misc
#include "misc/cstring.h" //miscuri pt string
#include "misc/power.h" //power
#include "misc/colors.h" //culori
#include "misc/math.h" //matematica
#include "misc/bitmap.h" //bitmap
#include "misc/cpu.h" //cpu

//io
#include "io/serial.h" //serial port
#include "io/parallel.h" //parallel port

//memorie
#include "memory/efiMemory.h" //memorie
#include "memory/pagefileallocator.h" //pfa
#include "memory/pagemapindexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/pagetablemanager.h" //ptm

//intrerupturi
#include "intrerupts/gdt.h" //gdt
#include "intrerupts/idt.h" //idt
#include "intrerupts/intrerupts.h" //handlere

//printf
#include "shell/printf.h" //printf

struct UEFIFirmware {
	unsigned short* Vendor;
	uint32_t Version;
};

struct BootInfo {
	//display
	DisplayDriver::DisplayBuffer* GOPFrameBuffer;
	PSF1_FONT* Font;

	//misc
	void* PowerDownVoid;
	UEFIFirmware* Efi;

	//memory
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

char** CPUFeatures;

const char* LLOSLogo = "/ \\   / \\   / \\   /  _ \\/ ___\\\n" //
					   "| |   | |   | |   | / \\||    \\\n" //
					   "| |_/\\| |_/\\| |_/\\| \\_/|\\___ |\n" //
					   "\\____/\\____/\\____/\\____/\\____/\n";

DisplayDriver display;
Power power;
Sound sound;
CPU cpu;
PCI pci;
RealTimeClock rtc;
SerialPort com1;
Parallel paralel;
DisplayDriver::DisplayBuffer* doubleBuffer;
PageTableManager pageTableManager = PageTableManager((PageTable*)0);

void EnablePaging(BootInfo* bootInfo) {
	uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;
    GlobalAllocator = PageFrameAllocator();
	GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    uint64_t fbBase = (uint64_t)bootInfo->GOPFrameBuffer->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->GOPFrameBuffer->BufferSize + 0x1000;
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

void LoadGDT() {
	GDTDescriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT)-1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
	LoadGDT(&gdtDescriptor);
}

IDTR idtr;

void SetUpIntreruptHandler(int vector, uint64_t handler) {
	IDTDescriptorEntry* PageFault = (IDTDescriptorEntry*)(idtr.Offset + vector * sizeof(IDTDescriptorEntry));
	PageFault->setOffset(handler);
	PageFault->Type_Attributes = IDT_TA_IntreruptGate;
	PageFault->Selector = 0x08;
}

void InitIntrerupts() {
	idtr.Limit = 0x0fff;
	idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

	SetUpIntreruptHandler(0,(uint64_t)DivByZeroHandler);
	SetUpIntreruptHandler(1,(uint64_t)DebugHandler);
	SetUpIntreruptHandler(5,(uint64_t)BoundRangeHandler);
	SetUpIntreruptHandler(6,(uint64_t)InvalidOpcodeHandler);
	SetUpIntreruptHandler(7,(uint64_t)DeviceUnavailableHandler);
	SetUpIntreruptHandler(10,(uint64_t)InvalidTSSHandler);
	SetUpIntreruptHandler(11,(uint64_t)SegmentNotPresentHandler);
	SetUpIntreruptHandler(12,(uint64_t)StackFaultHandler);
	SetUpIntreruptHandler(13,(uint64_t)GeneralProtectionFaultHandler);
	SetUpIntreruptHandler(14,(uint64_t)PageFaultHandler);
	SetUpIntreruptHandler(16,(uint64_t)FloatingPointHandler);
	SetUpIntreruptHandler(17,(uint64_t)AligmentHandler);
	SetUpIntreruptHandler(19,(uint64_t)SMIDFloatHandler);
	SetUpIntreruptHandler(20,(uint64_t)VirtualizationFaultHandler);
	SetUpIntreruptHandler(30,(uint64_t)SecurityExceptionHandler);

	asm volatile("lidt %0" : : "m" (idtr));
}

void InitDrivers(BootInfo* bootInfo) {
	com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
	com1.Write(SERIALWHITE,"Initialized Serial Port!\n");

	com1.Write("Initializing Display...\n");
	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	
	com1.Write("Initialized Display!\n");

	GlobalDisplay = &display;

	com1.Write("Loading GDT...\n");
	LoadGDT();
	com1.Write("Loaded GDT!\n");

	com1.Write("Initializing Intrerupts...\n");
	InitIntrerupts();
	com1.Write("Initialized Intrerupts!\n");

	com1.Write("Enabling paging...\n");

	EnablePaging(bootInfo);

	com1.Write("Paging enabled!\n");
	
	com1.Write("Initializing Double Buffering...\n");

	doubleBuffer->BaseAddr = GlobalAllocator.RequestPage();
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;
	GlobalAllocator.LockPages(doubleBuffer->BaseAddr, ((uint64_t)doubleBuffer->BufferSize / 4096) + 1);

    for (uint64_t t = (uint64_t)doubleBuffer->BaseAddr; t < (uint64_t)doubleBuffer->BufferSize + (uint64_t)doubleBuffer->BaseAddr; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

	display.InitDoubleBuffer(doubleBuffer);
	com1.Write("Initialized Display!\n");
	power.InitPower(bootInfo->PowerDownVoid);
	com1.Write("Initialized Power!\n");

	com1.Write("Detecting PCI devices...\n");
	pci.detectDevices();
	com1.Write("Detected ", inttostr(pci.DeviceCount), " PCI devices!\n");

	com1.Write("Detecting CPU features...\n");
	CPUFeatures = cpu.getFeatures();
	com1.Write("Detected ",inttostr(cpu.cpuFeatures)," CPU features!\n");

	display.setColour(WHITE);
	display.clearScreen(0);
	display.update();
}

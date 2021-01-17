#pragma once

//definitii standard
#include "stddef.h"
#include "stdint.h"

//drivere
#include "drivers/display/displaydriver.h" //display
#include "drivers/sound/sounddriver.h" //sunet
#include "drivers/pci/pci.h" //pci
#include "drivers/rtc/rtc.h" //realtimeclock
#include "drivers/keyboard/keyboarddriver.h" // claviatura

//misc
#include "misc/power/power.h" //power
#include "misc/bitmap/bitmap.h" //bitmap
#include "misc/cpu/cpu.h" //cpu
#include "misc/colors.h" //culori
#include "misc/uefi.h" //uefi

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

//libc
#include "libc/stdio.h" //input / output
#include "libc/math.h" //matematica
#include "libc/time.h" //timp

struct BootInfo {
	//display
	DisplayDriver::DisplayBuffer* GOPFrameBuffer;
	PSF1_FONT* Font;

	//misc
	PowerInfo* Power;
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
IDTR idtr;
Keyboard kb;

void EnablePaging(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    pageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t+= 0x1000){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->GOPFrameBuffer->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->GOPFrameBuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));
}

void LoadGDT() {
	GDTDescriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT)-1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
	LoadGDT(&gdtDescriptor);
}

void InitIntrerupts() {
	idtr.Limit = 0x0fff;
	idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    IDTDescriptorEntry* int_PageFault = (IDTDescriptorEntry*)(idtr.Offset + 0xE * sizeof(IDTDescriptorEntry));
    int_PageFault->setOffset((uint64_t)PageFaultHandler);
    int_PageFault->Type_Attributes = IDT_TA_InterruptGate;
    int_PageFault->Selector = 0x08;

    IDTDescriptorEntry* int_DoubleFault = (IDTDescriptorEntry*)(idtr.Offset + 0x8 * sizeof(IDTDescriptorEntry));
    int_DoubleFault->setOffset((uint64_t)DoubleFaultHandler);
    int_DoubleFault->Type_Attributes = IDT_TA_InterruptGate;
    int_DoubleFault->Selector = 0x08;

    IDTDescriptorEntry* int_GPFault = (IDTDescriptorEntry*)(idtr.Offset + 0xD * sizeof(IDTDescriptorEntry));
    int_GPFault->setOffset((uint64_t)GeneralProtectionFaultHandler);
    int_GPFault->Type_Attributes = IDT_TA_InterruptGate;
    int_GPFault->Selector = 0x08;

    IDTDescriptorEntry* int_Keyboard = (IDTDescriptorEntry*)(idtr.Offset + 0x21 * sizeof(IDTDescriptorEntry));
    int_Keyboard->setOffset((uint64_t)KBHandler);
    int_Keyboard->Type_Attributes = IDT_TA_InterruptGate;
    int_Keyboard->Selector = 0x08;

	asm ("lidt %0" : : "m" (idtr));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111101);
    outportb(PIC2_DATA, 0b11111111);

    asm ("sti");
}

void DetectHardware() {
	pci.detectDevices();
	CPUFeatures = cpu.getFeatures();
}

void InitDoubleBuffering() {
	doubleBuffer->BaseAddr = GlobalAllocator.RequestPage();
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;
	GlobalAllocator.LockPages(doubleBuffer->BaseAddr, ((uint64_t)doubleBuffer->BufferSize / 4096) + 1);

    for (uint64_t t = (uint64_t)doubleBuffer->BaseAddr; t < (uint64_t)doubleBuffer->BufferSize + (uint64_t)doubleBuffer->BaseAddr; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }
}
void InitDisplay(BootInfo* bootInfo) {
	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	
	GlobalDisplay = &display;
}

void InitCOM1() {
	com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
}

void InitDrivers(BootInfo* bootInfo) {
	GlobalKeyboard = &kb;
	LoadGDT();
	InitIntrerupts();
	EnablePaging(bootInfo);
	
	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	
	GlobalDisplay = &display;
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

	power.InitPower(bootInfo->Power->PowerOff,bootInfo->Power->Restart);
	pci.detectDevices();
	CPUFeatures = cpu.getFeatures();
	display.setColour(WHITE);
	display.clearScreen(0);
	display.update();
	com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
}


void Panic(const char* mesaj) {
	display.clearScreen(LIGHTRED);
	display.setCursorPos(0,0);
	display.setColour(WHITE);
	display.puts("Kernel Panic!\nMessage: ");
	display.puts(mesaj);
	while(1);
}
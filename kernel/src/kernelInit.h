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
#include "drivers/mouse/mouse.h"

//misc
#include "misc/power/power.h" //power
#include "misc/bitmap/bitmap.h" //bitmap
#include "misc/cpu/cpu.h" //cpu
#include "misc/colors.h" //culori
#include "misc/uefi.h" //uefi
#include "misc/logging/log.h" //logging

//io
#include "io/serial.h" //serial port
#include "io/parallel.h" //parallel port
#include "io/ps2.h"

//memorie
#include "memory/efiMemory.h" //memorie
#include "memory/PageFrameAllocator.h" //pfa
#include "memory/PageMapIndexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/PageTableManager.h" //ptm
#include "memory/dynamic.h" //malloc free calloc

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

    //verify
    uint64_t Key;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

char** CPUFeatures;

//todo fix typo in logo
const char* LLOSLogo =  "/ \\   / \\   /  _ \\/ ___\\\n"
						"| |   | |   | / \\||    \\\n"
						"| |_/\\| |_/\\| \\_/|\\___ |\n"
						"\\____/\\____/\\____/\\____/\n";

DisplayDriver display;
Power power;
Sound sound;
CPU cpu;
PCI pci;
RealTimeClock rtc;
SerialPort com1;
Parallel paralel;
DisplayDriver::DisplayBuffer* doubleBuffer;
PageTableManager pageTableManager = NULL;
IDTR idtr;
Keyboard kb;
Logging log;
Mouse mouse;
PS2Controller ps2;

void EnablePaging(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

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

void CreateIntrerupt(void* handler,uint8_t offset,uint8_t t_a,uint8_t selector) {
    IDTDescriptorEntry* int_NewInt = (IDTDescriptorEntry*)(idtr.Offset + offset * sizeof(IDTDescriptorEntry));
    int_NewInt->setOffset((uint64_t)handler);
    int_NewInt->Type_Attributes = t_a;
    int_NewInt->Selector = selector;
}

void InitIntrerupts() {
	idtr.Limit = 0x0fff;
	idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    CreateIntrerupt((void*)PageFaultHandler,0xE,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)DoubleFaultHandler,0x8,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)GeneralProtectionFaultHandler,0xD,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)InvalideOpcodeHandler,0x6,IDT_TA_InterruptGate,0x08);

    CreateIntrerupt((void*)KBHandler,0x21,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)MSHandler,0x2C,IDT_TA_InterruptGate,0x08);

	asm ("lidt %0" : : "m" (idtr));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111001);
    outportb(PIC2_DATA, 0b11101111);

    asm ("sti");
}

void InitDrivers(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    GlobalAllocator.LockPages((void*)0,256);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    LoadGDT();
	InitIntrerupts();
    
	//EnablePaging(bootInfo);

    //ps2.Init();

    GlobalKeyboard = &kb;
    mouse.Init();
    GlobalMouse = &mouse;

	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	
	
    doubleBuffer->BaseAddr = GlobalAllocator.RequestPage();
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;
	GlobalAllocator.LockPages(doubleBuffer->BaseAddr, (doubleBuffer->BufferSize / 4096) + 6);
    for (uint64_t t = (uint64_t)doubleBuffer->BaseAddr; t < doubleBuffer->BufferSize + (uint64_t)doubleBuffer->BaseAddr; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }
	display.InitDoubleBuffer(doubleBuffer);

	display.setColour(WHITE);
	display.clearScreen(0);
	display.update();

	GlobalDisplay = &display;

	log.info("Initialized PS/2, Intrerupts, Display!");


	power.InitPower(bootInfo->Power->PowerOff,bootInfo->Power->Restart);
	log.info("Initialized Power!");

	pci.detectDevices();
    log.info("Detected PCI devices!");
	CPUFeatures = cpu.getFeatures();
	log.info("Detected CPU features!");
	
	com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
    log.info("Initialized Serial!");

    log.info("Initialized Everything!");
    
    if(bootInfo->Key*2048+2047 != 0xFFFFFF) {
        log.error("Key verification failed!");
        while(1);
    }

    log.info("");
    log.info("Welcome to LowLevelOS!");
    log.info("By Moldu' (Nov. 2020 - Jan. 2021)");
    log.info("Build date & time:");
    log.info(__DATE__);
    log.info(__TIME__);
    rtc.waitSeconds(3);

}
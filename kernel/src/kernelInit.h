#pragma once

//definitii standard
#include "stddef.h"
#include "stdint.h"

//drivere
#include "drivers/display/displaydriver.h" //display
#include "drivers/pci/pci.h" //pci
#include "drivers/rtc/rtc.h" //realtimeclock
#include "drivers/keyboard/keyboarddriver.h" // claviatura
#include "drivers/mouse/mouse.h" //ps/2 mouse
#include "drivers/ahci/ahci.h" //disk

//misc
#include "misc/power/power.h" //power
#include "misc/bitmap/bitmap.h" //bitmap
#include "misc/cpu/cpu.h" //cpu
#include "misc/colors.h" //culori
#include "misc/uefi.h" //uefi
#include "misc/logging/log.h" //logging
#include "misc/power/acpi.h" //acpi
#include "misc/edid.h" //edid (monitor info)
#include "misc/bmp.h" //bitmap image

//io
#include "io/serial.h" //serial port
#include "io/msr.h" //model specific registers

//memorie
#include "memory/efiMemory.h" //memorie
#include "memory/PageFrameAllocator.h" //pfa
#include "memory/PageMapIndexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/PageTableManager.h" //ptm
#include "memory/heap.h" //heap

//intrerupturi
#include "intrerupts/idt.h" //idt
#include "intrerupts/intrerupts.h" //handlere

//libc
#include "libc/stdio.h" //input / output
#include "libc/math.h" //matematica
#include "libc/time.h" //timp

//scheduling
#include "scheduling/pit.h" //pit
#include "scheduling/cooperative.h"

//userspace
#include "userspace/userspace.h" //userspace
#include "userspace/kotos_gdt_tss/gdt.h"
#include "userspace/kotos_gdt_tss/tss.h"

//filesystem
#include "filesystem/llfs.h" //llfs

//elf
#include "elf/loader.h"

//flat binary
#include "flatbin/loader.h" 

#include "settings.h"

#define LOOP while(1)

#define DoubleBuffer

struct BootInfo {
	//display
	DisplayBuffer* GOPFrameBuffer;
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
IDTR idtr;
Keyboard kb;
Mouse mouse;
ACPI acpi;
TaskManager tmgr;

void* GenerateUserspaceStack() {
    void* UserspacePage = GlobalAllocator.RequestPage();
    GlobalTableManager.MapUserspaceMemory(UserspacePage);
    return UserspacePage;
}

void EnablePaging(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    GlobalTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t+= 0x1000){
        GlobalTableManager.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->GOPFrameBuffer->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->GOPFrameBuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void*)fbBase, fbSize/ 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        GlobalTableManager.MapMemory((void*)t, (void*)t);
    }

    asm volatile ("mov %0, %%cr3" : : "r" (PML4));
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

    CreateIntrerupt((void*)PageFaultHandlerEntry,0xE,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)DoubleFaultHandlerEntry,0x8,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)GeneralProtectionFaultHandlerEntry,0xD,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)InvalideOpcodeHandlerEntry,0x6,IDT_TA_InterruptGate,0x08);

    CreateIntrerupt((void*)KBHandlerEntry,0x21,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)MSHandlerEntry,0x2C,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)PITHandlerEntry,0x20,IDT_TA_InterruptGate,0x08);

	asm volatile ("lidt %0" : : "m" (idtr));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111000); //mouse and keyboard
    outportb(PIC2_DATA, 0b11101111);

    asm volatile("cli");
}

SDT* xsdt;
BMPHeader* bootImg;
void InitACPI(BootInfo* bootInfo) {
    xsdt = (SDT*)(bootInfo->RSDP->XSDTAddress);

    MCFG* mcfg = (MCFG*)acpi.FindTable(xsdt,(char*)"MCFG");
    MADT* madt = (MADT*)acpi.FindTable(xsdt,(char*)"APIC");
    FADT* fadt = (FADT*)acpi.FindTable(xsdt,(char*)"FACP");
    
    acpi.fadt = fadt;

    //enable acpi mode to be sure it's enabled
    outportb(fadt->SMI_CommandPort, fadt->AcpiEnable);
    while (inportw(fadt->PM1aControlBlock) & 1 == 0);

    #ifndef Quiet
    LogInfo("Parsing MADT");
    #endif
    acpi.ParseMADT(madt);

    #ifndef Quiet
    LogInfo("Decoding DSDT");
    #endif
    char *S5Addr = (char *) fadt->Dsdt +36;
    int dsdtLength = fadt->Dsdt+1-36;

    while(dsdtLength > 0) {
        if(memcmp(S5Addr,"_S5_",4) == 0)
            break;
        dsdtLength--;
        S5Addr++;
    }

    if(dsdtLength > 0) {
        S5Addr+=5;
        S5Addr += ((*S5Addr &0xC0)>>6) +2;
        if(*S5Addr == 0x0A)
            S5Addr++;
        acpi.SLP_TYPa = *(S5Addr)<<10;
        S5Addr++;
        if(*S5Addr == 0x0A)
            S5Addr++;
        acpi.SLP_TYPb = *(S5Addr)<<10;
        acpi.SLP_EN = 1<<13;
        acpi.ShutdownPossible = 1;
    }

    acpi.ShutdownBackup = bootInfo->Power->PowerOff;
    acpi.RebootBackup = bootInfo->Power->Restart;

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
    PITSetDivisor(0xFFFF);
    asm volatile("sti");

    com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
    com1.Write("Kernel intialized the Serial Port!\n");

	EnablePaging(bootInfo);

    mouse.Init();
    GlobalKeyboard = &kb;
    GlobalMouse = &mouse;

	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	

    InitializeHeap((void*)0x0000100000000000, 0x10);

    display.EmptyScreenBuffer = malloc(display.globalFrameBuffer->BufferSize);
    memset(display.EmptyScreenBuffer,0,display.globalFrameBuffer->BufferSize);

    if(bootInfo->Key*2048+2047 != 0xFFFFFF) {
        display.InitDoubleBuffer(bootInfo->GOPFrameBuffer);
        GlobalDisplay = &display;
        display.clearScreen(0);
        LogError("Key verification failed!");
        LogError("Corrupt or non-compliant bootloader!");
        while(1);
    }

#ifdef DoubleBuffer
    doubleBuffer->BaseAddr = GlobalAllocator.RequestPage();
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;

    GlobalAllocator.LockPages((void*)((uint64_t)doubleBuffer->BaseAddr-0x1000), (display.globalFrameBuffer->BufferSize / 4096) + 100);

    for (uint64_t t = (uint64_t)doubleBuffer->BaseAddr; t < doubleBuffer->BufferSize + (uint64_t)doubleBuffer->BaseAddr; t += 4096){
        GlobalTableManager.MapMemory((void*)t, (void*)t);
    }

	display.InitDoubleBuffer(doubleBuffer);
#else
    display.InitDoubleBuffer(bootInfo->GOPFrameBuffer);
#endif

	display.setColour(WHITE);
	display.clearScreen(0);
	display.update();

	GlobalDisplay = &display;
	
    #ifndef Quiet
    LogInfo("Initialized PS/2, Intrerupts, Display, Serial!");
    #endif

	CPUFeatures = cpu.getFeatures();
	#ifndef Quiet
    LogInfo("Detected CPU features!");
    #endif

    InitACPI(bootInfo);
    #ifndef Quiet
    LogInfo("Initialized ACPI!");
    #endif

    EnableSCE();
    GlobalTaskManager = &tmgr;
    #ifndef Quiet
    LogInfo("Initialized Task Switching and System Calls!");

    LogInfo("Initialized Everything!");
    #endif

    com1.Write("Kernel finished loading in ",inttostr(TimeSinceBoot)," seconds!\n");
}
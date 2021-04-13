#pragma once

//definitii standard
#include "stddef.h"
#include "stdint.h"

//drivere
#include "drivers/display/displaydriver.h" //display
#include "drivers/sound/sounddriver.h" //sunet
#include "drivers/pci/pci.h" //pci
#include "drivers/pci/pcitranslate.h" //translate pci things
#include "drivers/rtc/rtc.h" //realtimeclock
#include "drivers/keyboard/keyboarddriver.h" // claviatura
#include "drivers/mouse/mouse.h" //ps/2 mouse
#include "drivers/ahci/ahci.h" //disk
#include "drivers/fpu/fpu.h" //floating point unit

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
#include "io/parallel.h" //parallel port
#include "io/ps2.h"
#include "io/msr.h" //model specific registers

//memorie
#include "memory/efiMemory.h" //memorie
#include "memory/PageFrameAllocator.h" //pfa
#include "memory/PageMapIndexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/PageTableManager.h" //ptm
#include "memory/heap.h" //heap

//intrerupturi
#include "intrerupts/gdt.h" //gdt
#include "intrerupts/idt.h" //idt
#include "intrerupts/intrerupts.h" //handlere

//libc
#include "libc/stdio.h" //input / output
#include "libc/math.h" //matematica
#include "libc/time.h" //timp

//scheduling
#include "scheduling/pit.h" //pit

//userspace
//#include "userspace/userspace.h" //userspace

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

};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

char** CPUFeatures;

//todo fix typo in logo
const char* LLOSLogo =  "/ \\   / \\   /  _ \\/ ___\\\n"
						"| |   | |   | / \\||    \\\n"
						"| |_/\\| |_/\\| \\_/|\\___ |\n"
						"\\____/\\____/\\____/\\____/\n";

BootInfo* GlobalInfo;

DisplayDriver display;
Power power;
Sound sound;
CPU cpu;
PCI pci;
RealTimeClock rtc;
SerialPort com1;
Parallel paralel;
DisplayBuffer* doubleBuffer;
IDTR idtr;
Keyboard kb;
Logging log;
Mouse mouse;
PS2Controller ps2;
ACPI acpi;
PCITranslate pcitranslate;

//userspace stuff
uint64_t UserspaceStack[1024];
void UserSpaceFunc() {
    //asm("rdmsr");
    for(;;);
    LOOP;
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

    asm ("mov %0, %%cr3" : : "r" (PML4));
}

void LoadGDT() {
	GDTDescriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT)-1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    //TSSInit((void*)UserspaceStack);
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
    CreateIntrerupt((void*)PITHandler,0x20,IDT_TA_InterruptGate,0x08);
    CreateIntrerupt((void*)USBHandler,0xC4,IDT_TA_InterruptGate,0x08);

    CreateIntrerupt((void*)SYSHandler,0xFF,IDT_TA_InterruptGate,0x08);

	asm ("lidt %0" : : "m" (idtr));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111000);
    outportb(PIC2_DATA, 0b11101111);

    asm volatile("sti");
}

SDT* xsdt;
BMPHeader* bootImg;
void InitACPI(BootInfo* bootInfo) {
    xsdt = (SDT*)(bootInfo->RSDP->XSDTAddress);

    MCFG* mcfg = (MCFG*)acpi.FindTable(xsdt,(char*)"MCFG");
    MADT* madt = (MADT*)acpi.FindTable(xsdt,(char*)"APIC");
    FADT* fadt = (FADT*)acpi.FindTable(xsdt,(char*)"FACP");
    BGRT* bgrt = (BGRT*)acpi.FindTable(xsdt,(char*)"BGRT");

    bootImg = (BMPHeader*)(void*)bgrt->ImageAddress;
    acpi.fadt = fadt;
    acpi.ParseMADT(madt);

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

    pci.EnumeratePCI(mcfg);
    if(pci.DevicesIndex == 0) {
        log.warn("No MCFG found or no PCI devices!");
        log.warn("AHCI might not work.");
    }
}

void InitDrivers(BootInfo* bootInfo) {
    GlobalInfo = bootInfo;
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    GlobalAllocator.LockPages((void*)0,256);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    GlobalAllocator.LockPages(bootInfo,sizeof(BootInfo)/4096+1);

    LoadGDT();
	InitIntrerupts();
    
	EnablePaging(bootInfo);

    GlobalKeyboard = &kb;
    mouse.Init();
    GlobalMouse = &mouse;

	display.InitDisplayDriver(bootInfo->GOPFrameBuffer,bootInfo->Font);	

#ifdef DoubleBuffer
    doubleBuffer->BaseAddr = GlobalAllocator.RequestPage();
	doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
	doubleBuffer->Height = display.globalFrameBuffer->Height;
	doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
	doubleBuffer->Width = display.globalFrameBuffer->Width;

    GlobalAllocator.LockPages(doubleBuffer->BaseAddr, (display.globalFrameBuffer->BufferSize / 4096) + 100);

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
	
    com1.Init();
	GlobalCOM1 = &com1;
	com1.ClearMonitor();
    com1.Write("Kernel intialized the Serial Port!\n");
    log.info("Initialized PS/2, Intrerupts, Display, Serial!");

    InitializeHeap((void*)0x0000100000000000, 0x10);
    log.info("Initialized Heap!");

    PITSetDivisor(20000);
    log.info("Initialized PIT!");

	power.InitPower(bootInfo->Power->PowerOff,bootInfo->Power->Restart);
	log.info("Initialized Power!");

	CPUFeatures = cpu.getFeatures();
	log.info("Detected CPU features!");

    InitACPI(bootInfo);
    log.info("Initialized ACPI!");

    FPUInit();
    log.info("Intialized FPU!");

    //EnableSCE();
    //log.info("Prepared Userspace!");

    //RunInUserspace((void*)UserAPP);
    //no userspace 'cause i'm dumb and i can't implement it :(

    //UserAPP();

    log.info("Initialized Everything!");
    
    if(bootInfo->Key*2048+2047 != 0xFFFFFF) {
        log.error("Key verification failed!");
        while(1);
    }

    log.info("");
    log.info("Welcome to LowLevelOS!");
    log.info("By Moldu' (Nov. 2020 - Apr. 2021)");
    log.info("Build date & time:");
    log.info(__DATE__);
    log.info(__TIME__);
    //rtc.waitSeconds(2);
}
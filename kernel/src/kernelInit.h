#pragma once

//standard definitions
#include <stddef.h>
#include <stdint.h>

//drivers
#include <drivers/display/display.h>   //display
#include <drivers/pci/pci.h>           //pci
#include <drivers/rtc/rtc.h>           //real time clock
#include <drivers/keyboard/keyboard.h> // keyboard
#include <drivers/mouse/mouse.h>       //ps/2 mouse

//misc
#include <misc/bitmap/bitmap.h> //bitmap
#include <misc/colors.h>        //colors
#include <misc/logging/log.h>   //logging
#include <misc/power/acpi.h>    //acpi
#include <misc/smbios/smbios.h> //smbios

//io
#include <io/serial.h> //serial port
#include <io/msr.h>    //model specific registers

//memory
#include <memory/memory.h> //memory
#include <memory/pfa.h>    //pfa
#include <memory/pmi.h>    //pmi
#include <memory/paging.h> //paging
#include <memory/ptm.h>    //ptm
#include <memory/heap.h>   //heap

//cpu
#include <cpu/intrerupts.h> //idt
#include <cpu/gdt.h>        //gdt
#include <cpu/tss.h>        //tss

//libc
#include <libc/stdio.h> //standard i/o
#include <libc/math.h>  //maths

//scheduling
#include <scheduling/pit.h>     //pit
#include <scheduling/taskmgr.h> //task manager

//userspace
#include <userspace/userspace.h> //userspace

//filesystem
#include <filesystem/llfs.h> //llfs
#include <filesystem/vfs.h>  //vfs

//elf
#include <elf/loader.h>

//virtual terminal
#include <terminal/terminal.h>

//settings
#include <settings.h>

#define DoubleBuffer

struct BootInfo
{
    //display
    DisplayBuffer *GOPFrameBuffer;
    PSFFont *Font;

    //memory
    EFI_MEMORY_DESCRIPTOR *mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;

    //acpi
    RSDP2 *RSDP;

    //smbios
    SMBiosHeader *SMBIOS;

    //llfs
    LLFSHeader *RamFS;
};

extern uint64_t _BssStart;
extern uint64_t _BssEnd;

char **CPUFeatures;

BootInfo *GlobalInfo;

//variables
DisplayDriver display;
DisplayBuffer *doubleBuffer;
Keyboard kb;
Mouse mouse;
PCI pci;

void *GenerateUserspaceStack()
{
    void *Stack = GlobalAllocator.RequestPages(8);
    uint64_t StackSize = 0x1000 * 8; //32k
    for (uint64_t t = (uint64_t)Stack; t < (uint64_t)Stack + StackSize; t += 4096)
    {
        GlobalTableManager.MapUserspaceMemory((void *)t);
    }
    return Stack;
}

extern "C" void LoadPML4(uint64_t pml4, uint64_t offset);
uint64_t LastVirtualAddressUsed = 0;
void EnablePaging(BootInfo *bootInfo, void *pTable, void *Start, void *End)
{

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    uint64_t fbBase = (uint64_t)bootInfo->GOPFrameBuffer->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->GOPFrameBuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void *)fbBase, fbSize / 0x1000 + 1);

    PageTable *PML4 = (PageTable *)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);
    GlobalTableManager = PageTableManager(PML4);

    PageTableManager uefi = PageTableManager((PageTable *)pTable);

    SerialWrite("Kernel Map\n");
    //map kernel
    uint64_t kpages = ((uint64_t)End - (uint64_t)Start) / 0x1000;
    LastVirtualAddressUsed = (uint64_t)End;
    LastVirtualAddressUsed += 0x1000 - (LastVirtualAddressUsed % 0x1000);
    GlobalTableManager.DefinePhysicalMemoryLocation((void *)(LastVirtualAddressUsed + 0x1000));

    for (uint64_t i = 0; i < kpages; i++)
    {
        void *VirtualAddress = (void *)(Start + i * 0x1000);
        void *PhysicalAddress = uefi.GetPhysicalAddress(VirtualAddress);
        GlobalTableManager.MapMemory(VirtualAddress, PhysicalAddress);
    }

    uint64_t memorySize = GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize);

    SerialWrite("Memory Map\n");
    //map all the memory

    for (uint64_t i = 0; i < memorySize; i += 0x1000)
    {
        LastVirtualAddressUsed += 0x1000;
        GlobalTableManager.MapMemory((void *)LastVirtualAddressUsed, (void *)i);
    }
    LastVirtualAddressUsed += 0x1000;

    SerialWrite("Defining\n");

    GlobalTableManager.DefineVirtualTableLocation();

    SerialWrite("Enabling Paging\n");

    LoadPML4((uint64_t)PML4, (uint64_t)GlobalTableManager.PhysicalMemoryVirtualAddress);
}

void InitIntrerupts()
{
    idt->Limit = 0x0FFF;
    idt->Offset = (uint64_t)GlobalAllocator.RequestPage();

    CreateIntrerupt((void *)PageFaultHandlerEntry, 0xE, IDT_TA_InterruptGate, 0x08);
    CreateIntrerupt((void *)DoubleFaultHandlerEntry, 0x8, IDT_TA_InterruptGate, 0x08);
    CreateIntrerupt((void *)GeneralProtectionFaultHandlerEntry, 0xD, IDT_TA_InterruptGate, 0x08);
    CreateIntrerupt((void *)InvalideOpcodeHandlerEntry, 0x6, IDT_TA_InterruptGate, 0x08);

    CreateIntrerupt((void *)KBHandlerEntry, 0x21, IDT_TA_InterruptGate, 0x08);
    CreateIntrerupt((void *)MSHandlerEntry, 0x2C, IDT_TA_InterruptGate, 0x08);
    CreateIntrerupt((void *)PITHandlerEntry, 0x20, IDT_TA_InterruptGate, 0x08);

    asm volatile("lidt %0"
                 :
                 : "m"(*idt));

    RemapPIC();

    outportb(PIC1_DATA, 0b11111000); //mouse, keyboard, timer
    outportb(PIC2_DATA, 0b11101111);

    asm volatile("cli");
}

void InitVTerminals()
{
    SerialWrite("A");
    GlobalAllocator.RequestPages(64); // padding so we don't allocate in crap
    for (int i = 0; i < 0x400; i++)
    {
        SerialWrite("A");
        VirtualTerminals[i].init(0x1000);
        GlobalTableManager.MapUserspaceMemory((void *)VirtualTerminals[i].buffer);
    }
}

void InitACPI(BootInfo *bootInfo)
{
    SDT *xsdt = (SDT *)(bootInfo->RSDP->XSDTAddress);

    if (memcmp(&xsdt->Signature, "XSDT", 4) != 0)
    {
        LogWarn("Wrong XSDT signature! ACPI is disabled.");
        return;
    }

    MCFG *mcfg = (MCFG *)ACPIFindTable(xsdt, (char *)"MCFG");

#ifndef Quiet
    LogInfo("Enumerating PCI");
#endif
    if (mcfg != nullptr)
    {
        pci.EnumeratePCI(mcfg);
        if (pci.DevicesIndex == 0)
        {
            LogWarn("No PCI devices!");
        }
    }
    else
    {
        LogWarn("No MCFG found!");
    }
}

void InitDrivers(BootInfo *bootInfo, void *kernelPhysicalAddress, void *Start, void *End)
{
    GlobalInfo = bootInfo;

    //anounce kernel loaded successfully
    SerialWrite(SERIAL_WHITE);
    SerialWrite("Kernel pre-display phase has begun!\n");

    //clear the bss so it will not have random values at startup
    memset(&_BssStart, 0, &_BssEnd - &_BssStart);

    //read memory map
    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);
    SerialWrite("Readed memory map!\n");

    //lock kernel pages
    uint64_t kernelSize = (uint64_t)End - (uint64_t)Start;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(Start, kernelPages);
    SerialWrite("Locked kernel pages\n");

    //gdt and intrerupts
    gdtInit();
    SerialWrite("Loading GDT\n");
    InitIntrerupts();
    SerialWrite("Loading IDT\n");
    PITSetDivisor(0xFFFF);

    //enable paging
    EnablePaging(bootInfo, kernelPhysicalAddress, Start, End);
    GlobalInfo = (BootInfo *)GlobalTableManager.GetVirtualAddress(GlobalInfo);
    SerialWrite("Enabled Paging!\n");

    //enable mouse and keyboard
    mouse.Init();
    GlobalKeyboard = &kb;
    GlobalMouse = &mouse;
    SerialWrite("Enabled mouse and keyboard!\n");

    display.InitDisplayDriver(GlobalInfo->GOPFrameBuffer, GlobalInfo->Font);

    SerialWrite("Done display\n");

    InitializeHeap((void *)LastVirtualAddressUsed, 0x10);

    SerialWrite("Heap\n");

#ifdef DoubleBuffer
    doubleBuffer->BaseAddr = GlobalAllocator.RequestPages(display.globalFrameBuffer->BufferSize / 4096 + 1);
    doubleBuffer->BufferSize = display.globalFrameBuffer->BufferSize;
    doubleBuffer->Height = display.globalFrameBuffer->Height;
    doubleBuffer->PixelPerScanLine = display.globalFrameBuffer->PixelPerScanLine;
    doubleBuffer->Width = display.globalFrameBuffer->Width;

    display.InitDoubleBuffer(doubleBuffer);
#else
    display.InitDoubleBuffer(GlobalInfo->GOPFrameBuffer);
#endif

    display.colour = WHITE;
    display.clearScreen(0);
    display.update();

    GlobalDisplay = &display;

    SerialWrite("Kernel intialized the display!\n");

    InitVTerminals();
    SerialWrite("Kernel intialized the terminals!\n");

#ifndef Quiet
    LogInfo("Initialized PS/2, Intrerupts, Display, Serial!");
#endif

    InitACPI(GlobalInfo);
#ifndef Quiet
    LogInfo("Initialized ACPI!");
#endif
    SerialWrite("Kernel intialized ACPI!\n");

    EnableSCE();
#ifndef Quiet
    LogInfo("Initialized Task Switching and System Calls!");

    LogInfo("Initialized Everything!");
#endif
    SerialWrite("Kernel intialized SCE!\n");

    SerialWrite("Kernel finished loading in ", inttostr(TimeSinceBoot), " seconds!\n");
}
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

//memorie
#include "memory/efiMemory.h" //memorie
#include "memory/pagefileallocator.h" //pfa
#include "memory/pagemapindexer.h" //pmi
#include "memory/paging.h" //paging
#include "memory/pagetablemanager.h" //ptm

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

void EnablePaging(BootInfo* bootInfo) {
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    uint64_t fbBase = (uint64_t)bootInfo->framebuf->BaseAddr;
    uint64_t fbSize = (uint64_t)bootInfo->framebuf->BufferSize+ 0x1000;
    uint64_t fbPages = fbSize / 4096 + 1;
    GlobalAllocator.LockPages(&fbBase, fbPages);	

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    PageTableManager pageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t+= 0x1000){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    asm volatile ("mov %0, %%cr3" : : "r" (PML4));

    pageTableManager.MapMemory((void*)0x600000000, (void*)0x80000);

    uint64_t* test = (uint64_t*)0x600000000;
    *test = 10;
	if(*test != 10) {
		com1.Write("Failed\n");
		while(1);
	}
}

void InitDrivers(BootInfo* bootInfo) {
	com1.Init();
	com1.ClearMonitor();
	com1.Write(SERIALWHITE,"Initialized Serial Port!\n");

	com1.Write("Enabling paging...\n");

	EnablePaging(bootInfo);

	com1.Write("Paging enabled!\n");

	com1.Write("Initializing Power...\n");
	power.InitPower(bootInfo->PowerDownVoid);
	com1.Write("Initialized Power!\n");
	
	com1.Write("Detecting PCI devices...\n");
	pci.detectDevices();
	com1.Write("Detected ", inttostr(pci.DeviceCount), " PCI devices!\n");

	com1.Write("Detecting CPU features...\n");
	CPUFeatures = cpu.getFeatures();
	com1.Write("Detected ",inttostr(cpu.cpuFeatures)," CPU features!\n");

	com1.Write("Initializing Display...\n");
	display.InitDisplayDriver(bootInfo->framebuf,bootInfo->font);	
	com1.Write("Initialized Display!\n");
}

extern "C" int _start(BootInfo* binfo) {
	InitDrivers(binfo);

	display.setColour(LIGHTRED);
	display.puts("/ \\   / \\   / \\   /  _ \\/ ___\\\n");
	display.puts("| |   | |   | |   | / \\||    \\\n");
	display.puts("| |_/\\| |_/\\| |_/\\| \\_/|\\___ |\n");
	display.puts("\\____/\\____/\\____/\\____/\\____/\n");
	display.setColour(WHITE);

	display.puts("\n\nCPU: ");
	display.setColour(ORANGE);
	display.puts(cpu.getName(),"\n");
	display.setColour(WHITE);

	display.puts("CPU Vendor: ");
	display.setColour(ORANGE);
	display.puts(cpu.getVendor(),"\n");
	display.setColour(WHITE);

	display.puts("CPU Features: ");
	display.setColour(ORANGE);
	for(int i = 0;i<cpu.cpuFeatures;i++) {
		display.puts(CPUFeatures[i]," ");
	}
	display.setColour(WHITE);

	display.puts("\n\nTotal RAM: ");
	display.setColour(YELLOW);
	display.puts(inttostr((GlobalAllocator.GetFreeRAM()+GlobalAllocator.GetUsedRAM()+GlobalAllocator.GetReservedRAM())/1024/1024), " MB");
	display.setColour(WHITE);

	display.puts("\nFree RAM: ");
	display.setColour(YELLOW);
	display.puts(inttostr(GlobalAllocator.GetFreeRAM()/1024/1024)," MB");
	display.setColour(WHITE);

	display.puts("\nUsed RAM: ");
	display.setColour(YELLOW);
	display.puts(inttostr(GlobalAllocator.GetUsedRAM()/1024/1024)," MB");
	display.setColour(WHITE);

	display.puts("\nReserved RAM: ");
	display.setColour(YELLOW);
	display.puts(inttostr(GlobalAllocator.GetReservedRAM()/1024/1024)," MB");
	display.setColour(WHITE);

	
	display.puts("\n\nScreen Width: ");
	display.setColour(LIGHTGREEN);
	display.puts(inttostr(display.getWidth()),"px");
	display.setColour(WHITE);

	display.puts("\nScreen Height: ");
	display.setColour(LIGHTGREEN);
	display.puts(inttostr(display.getHeight()),"px\n");
	display.setColour(WHITE);

	display.puts("\nDetected ");
	display.setColour(LIGHTTURQOISE);
	display.puts(inttostr(pci.DeviceCount));
	display.setColour(WHITE);
	display.puts(" PCI devices: \n");

	for(int i = 0;i<pci.DeviceCount;i++) {
		display.puts("\nPCI Device ", inttostr(i) , "\n");

		display.puts("Vendor: ");
		display.setColour(LIGHTTURQOISE);
		display.puts("0x",inttohstr(pci.Devices[i].VendorID));
		display.setColour(WHITE);

		display.puts(" Device: ");
		display.setColour(LIGHTTURQOISE);
		display.puts("0x",inttohstr(pci.Devices[i].DeviceID));
		display.setColour(WHITE);

		display.puts(" Class: ");
		display.setColour(LIGHTTURQOISE);
		display.puts(pci.Devices[i].Class);
		display.setColour(WHITE);

		display.puts(" Function: ");
		display.setColour(LIGHTTURQOISE);
		display.puts(inttostr(pci.Devices[i].Function));
		display.setColour(WHITE);

		display.puts(" Bus: ");
		display.setColour(LIGHTTURQOISE);
		display.puts(inttostr(pci.Devices[i].Bus));
		display.setColour(WHITE);

		display.puts(" Slot: ");
		display.setColour(LIGHTTURQOISE);
		display.puts(inttostr(pci.Devices[i].Slot));
		display.setColour(WHITE);
	}

	while(1) {}
	return 0;
} 
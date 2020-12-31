#include "kernelInit/kernelInit.h"

void displayLogo() {
	display.setColour(LIGHTRED);
	display.puts(LLOSLogo);
	display.setColour(WHITE);
}

void displayCPU() {
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
}

void displayRAM(BootInfo* bootInfo) {
	display.puts("\n\nTotal RAM: ");
	display.setColour(YELLOW);
	display.puts(inttostr(GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize)/1024/1024), " MB");
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
}

void displayScreen() {
	display.puts("\n\nScreen Width: ");
	display.setColour(LIGHTGREEN);
	display.puts(inttostr(display.getWidth()),"px");
	display.setColour(WHITE);

	display.puts("\nScreen Height: ");
	display.setColour(LIGHTGREEN);
	display.puts(inttostr(display.getHeight()),"px\n");
	display.setColour(WHITE);
}

void displayPCI() {
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
}

extern "C" int _start(BootInfo* binfo) {
	InitDrivers(binfo);

	int i = 0;

	while(1){
		i++;
		display.clearScreen(0);

		displayLogo();
		displayCPU();
		displayRAM(binfo);
		displayScreen();
		displayPCI();
		display.puts("\n\nI:",inttostr(i));

		display.update();
	}

	while(1) {}
	return 0;
} 
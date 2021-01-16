#include "kernelInit.h"

void displayLogo() {
	printf("%co%s%co",LIGHTRED,LLOSLogo,WHITE);
}

void displayCPU() {
	printf("\n\nCPU: %co%s%co\n",ORANGE,cpu.getName(),WHITE);
	printf("CPU Vendor: %co%s%co\n",ORANGE,cpu.getVendor(),WHITE);
	printf("CPU Features: %co",ORANGE);
	for(int i = 0;i<cpu.cpuFeatures;i++) printf("%s ",CPUFeatures[i]);
	printf("%co",WHITE);
}

void displayRAM(BootInfo* bootInfo) {
	printf("\n\nTotal RAM: %co%d MB%co",YELLOW,GetMemorySize(bootInfo->mMap, bootInfo->mMapSize / bootInfo->mMapDescSize, bootInfo->mMapDescSize)/1024/1024,WHITE);
	printf("\nFree RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetFreeRAM()/1024/1024,WHITE);
	printf("\nUsed RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetUsedRAM()/1024/1024,WHITE);
	printf("\nReserved RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetReservedRAM()/1024/1024,WHITE);
}

void displayScreen() {
	printf("\n\nScreen Width: %co%dpx%co",LIGHTGREEN,display.getWidth(),WHITE);
	printf("\nScreen Height: %co%dpx%co\n",LIGHTGREEN,display.getHeight(),WHITE);
}

void displayPCI() {
	printf("\nDetected %co%d%co PCI devices: \n",LIGHTTURQOISE,pci.DeviceCount,WHITE);

	for(int i = 0;i<pci.DeviceCount;i++) {
		printf("\nPCI Device %d:\n",i);
		printf(" Vendor: %co0x%x%co",LIGHTTURQOISE,pci.Devices[i].VendorID,WHITE);
		printf(" Device: %co0x%x%co",LIGHTTURQOISE,pci.Devices[i].DeviceID,WHITE);
		printf(" Class: %co%s%co",LIGHTTURQOISE,pci.Devices[i].Class,WHITE);
		printf(" Function: %co%d%co",LIGHTTURQOISE,pci.Devices[i].Function,WHITE);
		printf(" Bus: %co%d%co",LIGHTTURQOISE,pci.Devices[i].Bus,WHITE);
		printf(" Slot: %co%d%co",LIGHTTURQOISE,pci.Devices[i].Slot,WHITE);
	}
}

void displayTime() {
	printf("\n\nTime: %co%d:%d:%d%co",LIGHTMAGENTA,rtc.readHours(),rtc.readMinutes(),rtc.readSeconds(),WHITE);
}

void displayDate() {
	printf("\n\nDate: %co%d/%d/20%d%co",MAGENTA,rtc.readDay(),rtc.readMonth(),rtc.readYear(),WHITE);
}

void displayFirmware(BootInfo* binfo) {
	printf("\n\nUEFI Firmware Vendor: %co%s%co",LIGHTRED,shorttostr(binfo->Efi->Vendor),WHITE);
	printf("\nUEFI Firmware Version: %co%d%co",LIGHTRED,binfo->Efi->Version,WHITE);
}

void displayRandomNumber() {
	printf("\n\nRandom number: %co%d%co\n",ORANGE,rand(),WHITE);
}

void displayKeyboard() {
	printf("%s_",kb.buffer);
}

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);
	display.setColour(WHITE);
	display.clearScreen(0);
	display.update();
	srand(rtc.readTime());

	while(1){
		display.clearScreen(BLACK);
		
		displayLogo();
		displayCPU();
		displayRAM(binfo);
		displayScreen();
		displayPCI();
		displayTime();
		displayDate();
		displayFirmware(binfo);
		displayRandomNumber();
		displayKeyboard();

		display.update();
	}

	while(1);

	return 0;
} 
#include "kernelInit.h"

#define MOUSE_BORDER_COLOUR WHITE
#define MOUSE_CONTENTS_COLOUR DARKGRAY

float frametime = 0.0;
float fps = (1.0f/frametime);

BootInfo* bInfo;

/*
Special Thanks to:
- @borrrden - he fixed my buggy keyboard handler
- @AbsurdPoncho - if he didn't do a osdev series i won't started this project
- @nothotscott - he has a very good tutorial on debugging that helped me (https://www.youtube.com/watch?v=llP7zB8HTls) and for some userspace tips
- @keepkonect - he's my friend that helped me do this project
*/

uint8_t MousePointer[16*25] = {
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
1,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,
1,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,
1,1,1,1,0,0,1,1,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
};

uint8_t MousePointerFilled[16*25] = {
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,
1,2,2,2,2,1,1,2,2,1,1,0,0,0,0,0,
1,2,2,2,1,1,1,2,2,2,1,0,0,0,0,0,
1,2,2,1,1,0,1,2,2,2,1,0,0,0,0,0,
1,1,1,1,0,0,1,1,2,2,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,2,2,2,1,0,0,0,0,
0,0,0,0,0,0,0,1,2,2,2,1,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
};

void displayLogo() {
	printf("%co%s%co",LIGHTRED,LLOSLogo,WHITE);
}

void displayCPU() {
	printf("\n\nCPU: %co%s%co\n",ORANGE,cpu.getName(),WHITE);
	printf("CPU Vendor: %co%s%co\n",ORANGE,cpu.getVendor(),WHITE);
	printf("CPU Features: %co",ORANGE);
	for(int i = 0;i<cpu.cpuFeatures;i++) printf("%s ",CPUFeatures[i]);
	printf("%co\n",WHITE);
	printf("CPU Cores: %co%u%co",ORANGE,acpi.CPUCoreCount,WHITE);
}

void displayRAM() {
	printf("\n\nPhysical RAM: %co%d MB%co",YELLOW,(GlobalAllocator.GetFreeRAM()+GlobalAllocator.GetUsedRAM())/1024/1024,WHITE);
	printf("\nTotal RAM: %co%d MB%co",YELLOW,(GlobalAllocator.GetFreeRAM()+GlobalAllocator.GetUsedRAM()+GlobalAllocator.GetReservedRAM())/1024/1024,WHITE);
	printf("\nFree RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetFreeRAM()/1024/1024,WHITE);
	printf("\nUsed RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetUsedRAM()/1024/1024,WHITE);
	printf("\nReserved RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetReservedRAM()/1024/1024,WHITE);
}

void displayScreen() {
	printf("\n\nScreen Width: %co%dpx%co",LIGHTGREEN,display.getWidth(),WHITE);
	printf("\nScreen Height: %co%dpx%co\n",LIGHTGREEN,display.getHeight(),WHITE);
}

void displayPCI() {
	printf("\n\nDetected %co%d%co PCI devices: \n",LIGHTTURQOISE,pci.DevicesIndex,WHITE);

	for(int i = 0;i<pci.DevicesIndex;i++) {
		TranslatedPCIDevice device = pci.Devices[i];
		printf("\nPCI Device %d:\n",i);
		printf(" Vendor: %co%s%co",LIGHTTURQOISE,device.VendorID,WHITE);
		printf(" Device: %co%s%co",LIGHTTURQOISE,device.DeviceID,WHITE);
		printf(" Class: %co%s%co",LIGHTTURQOISE,device.Class,WHITE);
		printf(" SubClass: %co%s%co",LIGHTTURQOISE,device.Subclass,WHITE);
	}
}

void displayDateTime() {
	printf("\n\nTime: %co%d:%d:%d%co",LIGHTMAGENTA,rtc.readHours(),rtc.readMinutes(),rtc.readSeconds(),WHITE);
	printf("\nDate: %co%d/%d/20%d%co",MAGENTA,rtc.readDay(),rtc.readMonth(),rtc.readYear(),WHITE);
}

void displayFirmware(BootInfo* binfo) {
	printf("\n\nUEFI Firmware Vendor: %co%s%co",LIGHTRED,shorttostr(binfo->Efi->Vendor),WHITE);
	printf("\nUEFI Firmware Version: %co%d%co",LIGHTRED,binfo->Efi->Version,WHITE);
}

void displayRandomNumber() {
	printf("\n\nRandom number: %co%d%co\n",ORANGE,rand(),WHITE);
}

void displayKeyboard() {
	printf("\n\nKey pressed: ");
	if(kbhit())
		printf("%c",getch());
	printf("\nKeyboard buffer: %s_",kb.buffer);
}

void drawPointer() {
	int x = 0;
	int y = 0;
	for(int i = 0;i< 16*25;i++) {
		if(MousePointerFilled[i] == 1 && display.getWidth() > x+mouse.state.X)
			display.putpix((x+mouse.state.X)*4,y+mouse.state.Y,MOUSE_BORDER_COLOUR);
		else if (MousePointerFilled[i] == 2 && display.getWidth() > x+mouse.state.X)
			display.putpix((x+mouse.state.X)*4,y+mouse.state.Y,MOUSE_CONTENTS_COLOUR);
		x++;
		if(x > 15) {
			x = 0;
			y++;
		}
	}
}

void doMouse() {
	drawPointer();

	if(mouse.state.ButtonLeft)		
		acpi.DoACPIShutdown();
	else if(mouse.state.ButtonRight)
		acpi.DoACPIReboot();
}

void displayRSDP() {
	printf("\n\nRSDP Address: %x",(uint64_t)GlobalInfo->RSDP);
	printf("\nRSDP Signature: %c%c%c%c%c%c%c%c",*(uint8_t*)GlobalInfo->RSDP,*((uint8_t*)GlobalInfo->RSDP+1),*((uint8_t*)GlobalInfo->RSDP+2),*((uint8_t*)GlobalInfo->RSDP+3),*((uint8_t*)GlobalInfo->RSDP+4),*((uint8_t*)GlobalInfo->RSDP+5),*((uint8_t*)GlobalInfo->RSDP+6),*((uint8_t*)GlobalInfo->RSDP+7));
}

void displaybgrt(){
	uint24 *img_ptr = (uint24 *)(bootImg + bootImg->HeaderSize);
	for (size_t dy = bootImg->Height - 1; dy > 0; dy--) //the bmp data is stored backwards, so we count from big to little
		memcpy(display.secondFrameBuffer->BaseAddr+dy*display.getWidth()*4,img_ptr,bootImg->Width); //copy line by line, it's very fast compared to ploting pixels one by one
}

void displayDebugInfo() {
	printf("\n\n==Debug info==");
	printf("\nFPS: %f",fps);
    printf("\nTime since boot: %f seconds",TimeSinceBoot);
	printf("\n==Debug info==");
}

char serialConsoleBuffer[256];
int serialConsoleBufferIndex = 0;

void handleSerialConsole() {
	if(streq(serialConsoleBuffer,"help")) {
		com1.Write("\nLLOS serial debug console");
		com1.Write("\nsh -> shutdown computer");
		com1.Write("\nrb -> restart computer");
		com1.Write("\nhelp -> show commands");
	} else if (streq(serialConsoleBuffer,"sh")) {
		acpi.DoACPIShutdown();
	} else if (streq(serialConsoleBuffer,"rb")) {
		acpi.DoACPIReboot();
	}

	memset(serialConsoleBuffer,0,256);
	serialConsoleBufferIndex = 0;
	com1.Write("\n> ");
}

void doSerialConsole() {
	if(com1.isReceived()) {
		char r = com1.Read();
		if(r == '\r' || r == '\n') {
			handleSerialConsole();
			return;
		}
		if(r == '\b')
			return;
		serialConsoleBuffer[serialConsoleBufferIndex++] = r;
		com1.Write(r);
	}
} 

void kernelLoop() {
	display.clearScreen(BLACK);

	displayLogo();
	displayDateTime();

	displayRAM();
	displayCPU();

	displayKeyboard();
	displaybgrt();

	printf("\n\nCPU Temp: %u C*",GetCPUTemp());

	printf("\n\nUse left click to shutdown and right click to reboot!");

	displayDebugInfo();

	doMouse();

	display.update();

	//sound.playnext();
}

char* kernelExitMessage = (char*)"Unexpected kernel exit";
extern "C" int kernelMain(BootInfo* binfo) {
	bInfo = binfo;
	InitDrivers(binfo);
	srand(rtc.readTime());
	display.clearScreen(BLACK);
	com1.Write("Kernel finished loading in ",inttostr(TimeSinceBoot)," seconds!\n");

	com1.Write("> ");

	LOOP {
		float timea = TimeSinceBoot;

		kernelLoop();
		doSerialConsole();
		
		float timeb = TimeSinceBoot;
		frametime = timeb-timea;
		if(frametime == 0)
			frametime = 0.001f;
		fps = (1.0f/frametime);
	}

	IntreruptFrame f={0};
	KernelPanic(kernelExitMessage,&f);

	return 0;
} 
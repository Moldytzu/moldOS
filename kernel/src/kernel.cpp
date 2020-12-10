#include "stddef.h"
#include "stdint.h" //definitii standard

#include "misc/cstring.h" //miscuri pt string
#include "drivers/display/displaydriver.h" //display
#include "drivers/rtc/rtc.h" //real time clock
#include "misc/power.h" //power
#include "drivers/sound/sounddriver.h" //sunet
#include "memory/efiMemory.h" //memorie
#include "misc/colors.h" //culori
#include "misc/math.h" //matematica
#include "misc/bitmap.h" //bitmap
#include "memory/pagefileallocator.h" //pfa

struct BootInfo {
	DisplayDriver::framebuffer* framebuf;
	DisplayDriver::PSF1_FONT* font;
	void* PowerDownVoid;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};

DisplayDriver display;
RealTimeClock rtc;
Power power;
Sound sound;
PageFrameAllocator pfa;


/*
De facut:
- un loader de fisiere
- driver de mouse
- driver de tascatura
*/

void InitDrivers(BootInfo* binfo) {
	display.InitDisplayDriver(binfo->framebuf,binfo->font);
	power.InitPower(binfo->PowerDownVoid);
}

void Panic(char* errorstr) {
	for(int i = 10;i>0;i--) {
		display.setCursorPos(0,0);
		display.clearScreen(BLUE);
		display.puts("Kernel panic because ");
		display.puts(errorstr);
		display.cursorNewLine();
		display.puts("Shutdowning in ");
		display.puts(inttostr((uint64_t)i));
		display.puts(" seconds...");
		rtc.waitSeconds(1);
	}
	power.Shutdown();
}

void LogoTest() {
	display.puts("/ \\   / \\   / \\   /  _ \\/ ___\\");
	display.cursorNewLine();

	display.puts("| |   | |   | |   | / \\||    \\");
	display.cursorNewLine();

	display.puts("| |_/\\| |_/\\| |_/\\| \\_/|\\___ |");
	display.cursorNewLine();

	display.puts("\\____/\\____/\\____/\\____/\\____/");
}

void DesktopTest() {
	int screenWidth = display.getWidth();
	int screenHeight = display.getHeight();
	display.clearScreen(WHITE);

	//taskbar
	display.setColour(DARKGRAY);
	display.putrect(0,0,20,screenWidth);

	//startmenu
	display.setColour(GREEN);
	display.putrect(0,0,20,48);
	display.setColour(WHITE);
	display.setCursorPos(8,0);
	display.puts("LLOS");

	//taskbar
	display.setColour(GRAY);
	display.putrect(60,0,20,128);
	display.setColour(BLACK);
	display.setCursorPos(68,0);
	display.puts("app abc");

	//app frame
	display.setColour(0x82ab95);
	display.putrect(0,20,20,screenWidth);
	display.setColour(BLACK);
	display.setCursorPos(5,20);
	display.puts("app abc");

	//app content
	display.setColour(RED);
	display.putrect(432,100,100,100);
}

void NumbersTest() {
	display.puts(inttostr((uint64_t)1234));
	display.cursorNewLine();
	display.puts(inttostr((int64_t)-1654));
	display.cursorNewLine();
	display.puts(inttostr((double)2019.2,10));
	display.cursorNewLine();
	display.puts(inttostr((double)-645019.2));
	display.cursorNewLine();
	display.puts(inttohstr((uint64_t)0x2061));
	display.cursorNewLine();
	display.puts(inttohstr((uint32_t)0x2061));
	display.cursorNewLine();
	display.puts(inttohstr((uint16_t)0x2061));
	display.cursorNewLine();
	display.puts(inttohstr((uint8_t)0x2061));
}

void TimeTest() {
	display.setColour(WHITE);
	display.puts(inttostr((uint64_t)rtc.readHours()));
	display.puts(":");
	display.puts(inttostr((uint64_t)rtc.readMinutes()));
	display.puts(":");
	display.puts(inttostr((uint64_t)rtc.readSecond()));
}

void drawDesktopTest() {
	DesktopTest();
	TimeTest();
	rtc.waitSeconds(1);
}

void drawMemoryMapTest(BootInfo* binfo) {
	display.puts("Memory Map: ");
	display.cursorNewLine();
	uint64_t mMapEntries = binfo->mMapSize / binfo->mMapDescSize;

	for(int i = 0;i < mMapEntries;i++) {
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)binfo->mMap + (i * binfo->mMapDescSize));
		display.puts(EFI_MEMORY_TYPE_STRINGS[desc->type]);
		display.setColour(0xff00ff);
		display.puts("->");
		display.puts(inttostr(desc->numPages * 4096 /1024));
		display.puts("KB");
		display.setColour(WHITE);
		display.puts("  ");
		if(i%5==0 && i!=0) {display.cursorNewLine();}
	}
}

void drawUsableRamTest(BootInfo* binfo) {
	uint64_t mMapEntries = binfo->mMapSize / binfo->mMapDescSize;

	uint64_t ramsize = 0;
	uint64_t usableram = 0;

	for(int i = 0;i < mMapEntries;i++) {
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)binfo->mMap + (i * binfo->mMapDescSize));
		if(desc->type == 7) {
			usableram = usableram + (desc->numPages * 4096 / 1024);
		}
		ramsize = ramsize + (desc->numPages * 4096 / 1024);
	}
	display.puts("Total RAM: ");
	display.puts(inttostr((double)ramsize/1024/1024,2));
	display.puts("GB ");
	display.puts("Usable RAM: ");
	display.puts(inttostr((double)usableram/1024/1024,2));
	display.puts("GB ");
	display.puts("Reserved RAM: ");
	display.puts(inttostr((double)(ramsize-usableram)/1024,2));
	display.puts("MB");
}
	uint8_t testbuf[20];
void efiMemoryTest(BootInfo* binfo) {
	display.puts("Total RAM: ");
	display.puts(inttostr(GetMemorySize(binfo->mMap,binfo->mMapSize / binfo->mMapDescSize,binfo->mMapDescSize)));
	display.puts(" B");
	display.cursorNewLine();

	Bitmap bitm;
	bitm.Buffer = &testbuf[0];
	bitm.Set(0,true);
	bitm.Set(1,false);
	bitm.Set(2,false);
	bitm.Set(3,true);
	display.puts("Bitmap Test: ");
	for(int i=0;i<20;i++) {
		display.puts(bitm[i] ? "1" : "0");
	}

	display.cursorNewLine();
	display.puts("PageFileAllocator Test: ");
	display.cursorNewLine();
	display.puts("N-avem ce sa testam :)");
	pfa.readMemoryMap(binfo->mMap,binfo->mMapSize,binfo->mMapDescSize);
	display.cursorNewLine();
	display.cursorNewLine();
}

void soundTest() {
	rtc.waitSeconds(1);
	display.puts("Sound test");
	sound.play(1000);
	rtc.waitSeconds(1);
	sound.stop();
}

void shutdownTest() {
	display.puts("The computer will shutdown in 5 seconds!");
	rtc.waitSeconds(5);
	power.Shutdown();
}

void gopTest() {
	display.puts("GOP Test:");
	display.cursorNewLine();
	display.puts("Address: 0x");
	display.puts(inttohstr((uint32_t)(uint64_t)display.globalFrameBuffer->BaseAddr));
	display.cursorNewLine();
	display.puts("Buffer Size: 0x");
	display.puts(inttohstr((uint32_t)(uint64_t)display.globalFrameBuffer->BufferSize));
	display.cursorNewLine();
	display.puts("Width: ");
	display.puts(inttostr((uint64_t)display.globalFrameBuffer->Width));
	display.cursorNewLine();
	display.puts("Height: ");
	display.puts(inttostr((uint64_t)display.globalFrameBuffer->Height));
	display.cursorNewLine();
	display.puts("PixelsPerScanLine: ");
	display.puts(inttostr((uint64_t)display.globalFrameBuffer->PixelPerScanLine));
	display.cursorNewLine();
}

void runTests(BootInfo* binfo) {
	gopTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	LogoTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	NumbersTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	TimeTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	drawMemoryMapTest(binfo);
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	drawUsableRamTest(binfo);
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	efiMemoryTest(binfo);
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	soundTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);

	shutdownTest();
	display.cursorNewLine();
	display.cursorNewLine();
	rtc.waitSeconds(2);
}

extern "C" int _start(BootInfo* binfo) {
	InitDrivers(binfo);

	runTests(binfo);

	while(1) {}

	return 0;
} 
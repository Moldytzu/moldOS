#include "stddef.h"
#include "stdint.h" //definitii standard

#include "misc/cstring.h" //miscuri pt string
#include "drivers/display/displaydriver.h" //display
#include "drivers/rtc/rtc.h" //real time clock
#include "drivers/rtc/rtc.h" //real time clock
#include "misc/power.h" //real time clock
#include "memory/efiMemory.h"

struct BootInfo {
	DisplayDriver::framebuffer* framebuf;
	DisplayDriver::PSF1_FONT* font;
	void* PowerDownVoid;
	void* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
};
DisplayDriver display;
RealTimeClock rtc;
Power power;

/*
De facut:
- un loader de fisiere
- driver de mouse
- driver de tascatura
*/

void Panic(char* errorstr) {
	for(int i = 10;i>0;i--) {
		display.setCursorPos(0,0);
		display.clearScreen(display.BLUE);
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
	display.cursorNewLine();
}

void DesktopTest() {
	int screenWidth = display.getWidth();
	int screenHeight = display.getHeight();
	display.clearScreen(display.WHITE);

	//taskbar
	display.setColour(display.DARKGRAY);
	display.putrect(0,0,20,screenWidth);

	//startmenu
	display.setColour(display.GREEN);
	display.putrect(0,0,20,48);
	display.setColour(display.WHITE);
	display.setCursorPos(8,0);
	display.puts("LLOS");

	//taskbar
	display.setColour(display.GRAY);
	display.putrect(60,0,20,128);
	display.setColour(display.BLACK);
	display.setCursorPos(68,0);
	display.puts("app abc");

	//app frame
	display.setColour(0x82ab95);
	display.putrect(0,20,20,screenWidth);
	display.setColour(display.BLACK);
	display.setCursorPos(5,20);
	display.puts("app abc");

	//app content
	display.setColour(display.RED);
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
	display.setCursorPos(display.getWidth()-64,0);
	display.setColour(display.WHITE);
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

extern "C" int _start(BootInfo* binfo) {
	display.InitDisplayDriver(binfo->framebuf,binfo->font);
	power.InitPower(binfo->PowerDownVoid);

	display.cursorNewLine();
	uint64_t mMapEntries = binfo->mMapSize / binfo->mMapDescSize;

	for(int i = 0;i < mMapEntries;i++) {
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)binfo->mMap + (i * binfo->mMapDescSize));
		display.puts(inttostr(desc->numPages * 4096 / 1024));
		display.cursorNewLine();
	}

	while(1) {}

	return 0;
}
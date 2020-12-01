#include "stddef.h"
#include "stdint.h" //definitii standard

#include "misc/cstring.h" //miscuri pt string
#include "drivers/display/displaydriver.h" //display
#include "drivers/rtc/rtc.h"

DisplayDriver display;
RealTimeClock rtc;

/*
De facut:
- un loader de fisiere
- driver de mouse
- driver de tascatura
*/

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
	display.clearScreen(display.WHITE);

	display.setColour(display.DARKGRAY);
	display.putrect(0,0,20,display.getWidth());
	display.setColour(display.WHITE);

	display.setCursorPos(display.getWidth()/2-20,2);
	display.puts("LLOS");

	display.setColour(display.BLACK);
	display.putbox(0,20,display.getHeight()/2-20,display.getWidth()/2);

	display.putbox(display.getWidth()*2,20,display.getHeight()/2-20,display.getWidth()/2);

	display.putbox(0,display.getHeight()/2,display.getHeight()/2,display.getWidth()/2);

	display.putbox(display.getWidth()*2,display.getHeight()/2,display.getHeight()/2,display.getWidth()/2);
	display.setColour(display.WHITE);
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
	for(;;){
		display.clearScreen(display.BLACK);
		display.puts(inttostr((uint64_t)rtc.readHours()));
		display.puts(":");
		display.puts(inttostr((uint64_t)rtc.readMinutes()));
		display.puts(":");
		display.puts(inttostr((uint64_t)rtc.readSecond()));
		display.cursorNewLine();
		display.puts(inttostr((uint64_t)rtc.readTime()));
		rtc.waitSeconds(1);
	}
}

extern "C" int _start(DisplayDriver::framebuffer* framebuf, DisplayDriver::PSF1_FONT* font,void* PowerDownVoid) {
	display.InitDisplayDriver(framebuf,font);

	TimeTest();

	while(1) {}

	return 0;
}
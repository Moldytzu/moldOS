#include "common/utils.h" //utils
#include "io/memory.h" //memory
#include "drivers/hardwareinfo/cpu.h"
#include "drivers/sound/sounddriver.h" //sound
#include "drivers/timer.h" //system timer
#include "drivers/keyboard/keyboarddriver.h" //keyboard
#include "drivers/display/displaydriver.h" //display
#include "drivers/power/power.h" //power

/*
De facut:
- un loader de fisiere
- driver de mouse
*/

void Logo() {
	puts(WHITE,"/ \\   / \\   / \\   /  _ \\/ ___\\");
	cursorNewLine();

	puts(WHITE,"| |   | |   | |   | / \\||    \\");
	cursorNewLine();

	puts(WHITE,"| |_/\\| |_/\\| |_/\\| \\_/|\\___ |");
	cursorNewLine();

	puts(WHITE,"\\____/\\____/\\____/\\____/\\____/");
	cursorNewLine();
}

int _start(framebuffer* framebuf, PSF1_FONT* font,void* PowerDownVoid) {
	InitDisplayDriver(framebuf,font);
	InitKeyboard();
	InitPower(PowerDownVoid);

	clearScreen(WHITE);

	putrect(DARKGRAY,0,0,20,getWidth());
	
	setCursorPos(getWidth()/2-20,2);
	puts(WHITE,"LLOS");

	putbox(BLACK,0,20,getHeight()/2-20,getWidth()/2);

	putbox(BLACK,getWidth()*2,20,getHeight()/2-20,getWidth()/2);

	putbox(BLACK,0,getHeight()/2,getHeight()/2,getWidth()/2);

	putbox(BLACK,getWidth()*2,getHeight()/2,getHeight()/2,getWidth()/2);

/*
	Logo();

	puts(YELLOW,"Text normal");

	cursorNewLine();
	highlightputs(WHITE,DARKGRAY,"Text evidentiat");

	cursorNewLine();
	boxedputs(WHITE,GREEN,"Text in cutie");
	
	cursorNewLine();
	puts(LIGHTRED,inttocstring(strlen("alex")));

	putrect(RED,200,200,200,200);

	putbox(LIGHTGREEN,600,500,80,20);

	while(1) {
		WriteKey(YELLOW);
	}*/

	Breakpoint();
	return 0;
}
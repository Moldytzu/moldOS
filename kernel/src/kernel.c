#include "common/utils.h" //utils
#include "io/memory.h" //memory
#include "drivers/hardwareinfo/cpu.h"
#include "drivers/sound/sounddriver.h" //sound
#include "drivers/timer.h" //system timer
#include "drivers/keyboard/keyboarddriver.h" //keyboard
#include "drivers/display/displaydriver.h" //display
#include "drivers/power/power.h" //power

int _start() {
	InitKeyboard();
	while(1) {
		if(GetKey() == ESC_KEY) {
			Reset();
		}	
	}
	return 0;
}
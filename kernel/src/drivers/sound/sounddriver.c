#include "sounddriver.h"

void play_sound(long long nFrequence) {
 	long long Div;
 	int tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6);
 	outportb(0x42, (int) (Div) );
 	outportb(0x42, (int) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = inportb(0x61);
  	if (tmp != (tmp | 3)) {
 		outportb(0x61, tmp | 3);
 	}
 }
 
 void stop_sound() {
	int tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
 }
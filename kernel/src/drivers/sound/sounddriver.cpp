#include "sounddriver.h"

void Sound::play(long long nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6);
 	outportb(0x42, (uint8_t) (Div) );
 	outportb(0x42, (uint8_t) (Div >> 8));
 
        //And play the sound using the PC speaker
 	tmp = inportb(0x61);
  	//if (tmp != (tmp | 3)) {
 		outportb(0x61, tmp | 3);
 	//}
}

void Sound::stop() {
    int tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
}

int abs(int num) {
	if(num<0) return -num;
	return num;
}

#define WHOLENOTE (60000 * 4) / 180
void Sound::playnext() {
	Note curnote = Melody[CurrentNote];

	int divider = curnote.Duration;
	int duration = 0;
	if(divider > 0) {
		duration = (WHOLENOTE) / divider;
	} else if(divider < 0) {
		duration = (WHOLENOTE) / abs(divider);
		duration *= 1.5;
	}

	play(curnote.Frequency);
	PITSleep((uint64_t)duration);
	stop();
	CurrentNote++;

	if(CurrentNote > MelodyLen) CurrentNote = 0;
}
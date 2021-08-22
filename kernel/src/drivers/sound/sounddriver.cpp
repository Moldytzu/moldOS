#include "sounddriver.h"

void Sound::play(long long nFrequence) {
	PIT2SetFreq(nFrequence);
 	outportb(0x61, inportb(0x61) | 3);
}

void Sound::stop() {
 	outportb(0x61, inportb(0x61) & 0xFC);
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
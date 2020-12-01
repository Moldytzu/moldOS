#include "sounddriver.h"

inline unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}

inline void outportb(unsigned int port,unsigned char value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

void Sound::play_sound(long long nFrequence) {
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

void Sound::stop_sound() {
    int tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
}
#include "defs.h"

unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   __asm__ volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}
void outportb(unsigned int port,unsigned char value)
{
   __asm__ volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

void outportw(unsigned short int port, unsigned short int data) {
	asm volatile("outw %%ax, %%dx"
            :
            : "d"(port), "a"(data));
}

int strlen(char* string) {
   int r = 0;
   while(string[r] != '\0') {
      r++;
   }
   return r;
}

int intlen(int tmp) {
   int nums = 0;
   while(tmp != 0) {
      nums++;
      tmp /= 10;
   }
   return nums;
}

char* inttocstring(int num) 
{
   int tmp = num;
   int nums = 0;
   while(tmp != 0) {
      nums++;
      tmp /= 10;
   }
   char* str;
   int index = 0;
   while(num!=0) {
      switch(num%10) {
         case 0:
            str[nums-index] = '0';
            break;
         case 1:
            str[nums-index] = '1';
            break;
         case 2:
            str[nums-index] = '2';
            break;
         case 3:
            str[nums-index] = '3';
            break;
         case 4:
            str[nums-index] = '4';
            break;
         case 5:
            str[nums-index] = '5';
            break;
         case 6:
            str[nums-index] = '6';
            break;
         case 7:
            str[nums-index] = '7';
            break;
         case 8:
            str[nums-index] = '8';
            break;
         case 9:
            str[nums-index] = '9';
            break;
      } 
      index++;
      num/=10;
   }
   str[index+1] = '\0'; 

   return str+1; 
} 

void doNothing() {
	__asm__("nop");
}

void Breakpoint() {
	while(1) {doNothing();}
}
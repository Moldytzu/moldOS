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

void doNothing() {
	__asm__("nop");
}

void Breakpoint() {
	while(1) {doNothing();}
}
unsigned char inportb(unsigned int port);
void outportb(unsigned int port,unsigned char value);
void outportw(unsigned short int port, unsigned short int data);
void doNothing();
void Breakpoint();
void reverseString(char str[], int length);
char* inttocstring(int num);
int strlen(char* string);
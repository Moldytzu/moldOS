#pragma once
#include "stdint.h"
#include "stddef.h"
#include "../../misc/math.h"
#include "../../memory/efiMemory.h"
#include "../../memory/pagefileallocator.h"
#include "../../misc/psf1.h"

class DisplayDriver {
public:
typedef struct {
	void* BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} DisplayBuffer;

//drawing pixels
void putpix(int x,int y);
void putrect(int x,int y,int h,int w);
void putbox(int xx,int yy,int h,int w);
void scroll();

//text rendering
void puts(const char* ch);
void puts(const char* ch, const char* ch2);
void puts(const char* ch, const char* ch2, const char* ch3);
void puts(const char* ch, const char* ch2, const char* ch3, const char* ch4);
void puts(const char* ch, const char* ch2, const char* ch3, const char* ch4, const char* ch5);

void putc(char ch,unsigned int xx,unsigned int yy);
void putc(char ch);
void setCursorPos(int x,int y);
void boxedputs(unsigned int highcolour,const char* ch);
void highlightputs(unsigned int highcolour,const char* ch);
void cursorNewLine();
void returnCursor();

int getLenght(char* ch);

//misc
void clearScreen(unsigned int colour);
void setColour(unsigned int colo);
void checkScroll();

//initializare
void InitDisplayDriver(DisplayBuffer* framebuf, PSF1_FONT* font);
void InitDoubleBuffer(DisplayBuffer* f);

//ceva geturi
int getWidth();
int getHeight();

//chestii de double buffer
void update();

//variabile
DisplayDriver::DisplayBuffer* secondFrameBuffer;
DisplayDriver::DisplayBuffer* globalFrameBuffer;
PSF1_FONT* globalFont;
Point CursorPos;
unsigned int colour;
bool scrol = false;
};

extern DisplayDriver* GlobalDisplay;
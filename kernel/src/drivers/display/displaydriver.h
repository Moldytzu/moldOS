#pragma once
#include "stdint.h"
#include "stddef.h"
#include "../../libc/stdio.h"
#include "../../libc/math.h"
#include "../../memory/memory.h"
#include "../../memory/PageFrameAllocator.h"
#include "../../misc/psf1.h"

typedef struct {
	void* BaseAddr;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelPerScanLine;
} DisplayBuffer;

class DisplayDriver {
public:

//drawing pixels
void putpix(int x,int y);
void putpix(int x,int y,uint32_t clr);

//text rendering
void puts(const char* ch);
void putc(char ch,uint32_t xx,uint32_t yy);
void putc(char ch);
void boxedputs(uint32_t highcolour,const char* ch);
void highlightputs(uint32_t highcolour,const char* ch);

//cursor
void setCursorPos(int x,int y);
void cursorNewLine();
void advanceCursor();

//misc
void clearScreen(uint32_t colour);
void checkScroll();
void scroll();

//initializare
void InitDisplayDriver(DisplayBuffer* framebuf, PSFFont* font);
void InitDoubleBuffer(DisplayBuffer* f);

uint64_t getWidth();
uint64_t getHeight();

void update();

//vars
DisplayBuffer* secondFrameBuffer;
DisplayBuffer* globalFrameBuffer;
PSFFont* globalFont;
Point CursorPos;
uint64_t colour;
void* EmptyScreenBuffer;
};

extern DisplayDriver* GlobalDisplay;
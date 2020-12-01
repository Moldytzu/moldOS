#include "../../common/utils.h"

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

typedef struct {
	unsigned int X;
	unsigned int Y;
} Point;

typedef struct {
	void* BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} framebuffer;

framebuffer* globalFrameBuffer;
PSF1_FONT* globalFont;
Point CursorPos;

void putc(unsigned int colour,char ch,unsigned int x,unsigned int y) {
	unsigned int* pixPtr = (unsigned int*)globalFrameBuffer->BaseAddr;
	char* fontPtr = globalFont->glyphBuffer + (ch * globalFont->psf1_Header->charsize);
	for(unsigned long my = y;my < y+16;my++) {
		for(unsigned long mx = x;mx < x+8;mx++) {
			if((*fontPtr & (0b10000000 >> (mx-x))) > 0) {
					*(unsigned int*)(pixPtr + mx + (my * globalFrameBuffer->PixelPerScanLine)) = colour;
			}
		}
		fontPtr++;
	}
}

void puts(unsigned int colour,char* ch)
{
	while(*ch != 0) {
		putc(colour,*ch,CursorPos.X,CursorPos.Y);
		CursorPos.X+=8;
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			CursorPos.X = 0;
			CursorPos.Y += 16;
		}
		ch++;
	}
}

void setCursorPos(int x,int y) {
	CursorPos.X = x;
	CursorPos.Y = y;
}

void InitDisplayDriver(framebuffer* framebuf, PSF1_FONT* font) {
	globalFont = font;
	globalFrameBuffer = framebuf;
	clearScreen(0);
	setCursorPos(0,0);
}

void clearScreen(unsigned int colour) {
	for(unsigned int y = 0;y< globalFrameBuffer->Height;y++) {
		for(unsigned int x = 0; x<globalFrameBuffer->Width*4;x+=4) {
			*(unsigned int*)(x+(y* globalFrameBuffer->PixelPerScanLine * 4) + globalFrameBuffer->BaseAddr) = colour;
		}
	}
}

void putpix(unsigned int colour,int x,int y) {
	*(unsigned int*)(x+(y* globalFrameBuffer->PixelPerScanLine * 4) + globalFrameBuffer->BaseAddr) = colour;
}

void putrect(unsigned int colour,int xx,int yy,int h,int w) {
	for(unsigned int y = yy;y<(yy+h);y++) {
		for(unsigned int x = xx; x<(xx+w*4);x+=4) {
			putpix(colour,x,y);
		}
	}
}

void putbox(unsigned int colour,int xx,int yy,int h,int w) {
	for(unsigned int x = xx; x<(xx+w*4);x+=4) {
		putpix(colour,x,yy);
		putpix(colour,x,(yy+h));
	}
	for(unsigned int y = yy;y<(yy+h);y++) {
		putpix(colour,xx,y);
		putpix(colour,(xx+w*4),y);
	}
}

int getLenght(char* ch) {
	int l=0;
	while(*ch != 0) {
		l+=1;
		ch++;
	}
	return l;
}

void boxedputs(unsigned int colour,unsigned int highcolour,char* ch) {
	putbox(highcolour,CursorPos.X,CursorPos.Y,16,getLenght(ch)*8);
	while(*ch != 0) {
		putc(colour,*ch,CursorPos.X,CursorPos.Y);
		setCursorPos(CursorPos.X+8,CursorPos.Y);
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			setCursorPos(0,CursorPos.Y+16);
		}
		ch++;
	}
}

void highlightputs(unsigned int colour,unsigned int highcolour,char* ch) {
	putrect(highcolour,CursorPos.X,CursorPos.Y,16,getLenght(ch)*8);
	while(*ch != 0) {
		putc(colour,*ch,CursorPos.X,CursorPos.Y);
		CursorPos.X+=8;
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			CursorPos.X = 0;
			CursorPos.Y += 16;
		}
		ch++;
	}
}

void cursorNewLine() {
	setCursorPos(0,CursorPos.Y+16);
}

int getWidth() {
	return globalFrameBuffer->Width;
}

int getHeight() {
	return globalFrameBuffer->Height;
}
#include "displaydriver.h"
#include "../../misc/math.h"

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
	void* BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} framebuffer;

DisplayDriver::framebuffer* globalFrameBuffer;
DisplayDriver::PSF1_FONT* globalFont;
Point CursorPos;
unsigned int colour;

void DisplayDriver::putc(char ch,unsigned int xx,unsigned int yy) {
    unsigned int* pixPtr = (unsigned int*)globalFrameBuffer->BaseAddr;
    char* fontPtr = (char*)globalFont->glyphBuffer + (ch * globalFont->psf1_Header->charsize);
    for (unsigned long y = yy; y < yy + 16; y++){
        for (unsigned long x = xx; x < xx+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xx))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * globalFrameBuffer->PixelPerScanLine)) = colour;
                }

        }
        fontPtr++;
    }
}

void DisplayDriver::puts(const char* ch)
{
	char* cr = (char*)ch;
	while(*cr != 0) {
		putc(*cr,CursorPos.X,CursorPos.Y);
		CursorPos.X+=8;
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			CursorPos.X = 0;
			CursorPos.Y += 16;
		}
		cr++;
	}
}

void DisplayDriver::setCursorPos(int x,int y) {
	CursorPos.X = x;
	CursorPos.Y = y;
}

void DisplayDriver::InitDisplayDriver(framebuffer* framebuf, PSF1_FONT* font) {
	globalFont = font;
	globalFrameBuffer = framebuf;
	clearScreen(0);
	setCursorPos(0,0);
	colour = 0xffffff;
}

void DisplayDriver::clearScreen(unsigned int colour) {
	for(unsigned int y = 0;y< globalFrameBuffer->Height;y++) {
		for(unsigned int x = 0; x<globalFrameBuffer->Width*4;x+=4) {
			*(unsigned int*)(x+(y* globalFrameBuffer->PixelPerScanLine * 4) + globalFrameBuffer->BaseAddr) = colour;
		}
	}
	CursorPos.X = 0;
	CursorPos.Y = 0;
}

void DisplayDriver::putpix(int x,int y) {
	*(unsigned int*)(x+(y* globalFrameBuffer->PixelPerScanLine * 4) + globalFrameBuffer->BaseAddr) = colour;
}

void DisplayDriver::putrect(int xx,int yy,int h,int w) {
	for(unsigned int y = yy;y<(yy+h);y++) {
		for(unsigned int x = xx; x<(xx+w*4);x+=4) {
			putpix(x,y);
		}
	}
}

void DisplayDriver::putbox(int xx,int yy,int h,int w) {
	for(unsigned int x = xx; x<(xx+w*4);x+=4) {
		putpix(x,yy);
		putpix(x,(yy+h));
	}
	for(unsigned int y = yy;y<(yy+h);y++) {
		putpix(xx,y);
		putpix((xx+w*4),y);
	}
}

int DisplayDriver::getLenght(char* ch) {
	int l=0;
	while(*ch != 0) {
		l+=1;
		ch++;
	}
	return l;
}

void DisplayDriver::boxedputs(unsigned int highcolour,const char* ch) {
	char* cr = (char*)ch;
	unsigned int bak = colour;
	colour = highcolour;
	putbox(CursorPos.X,CursorPos.Y,16,getLenght(cr)*8);
	while(*cr != 0) {
		putc(*cr,CursorPos.X,CursorPos.Y);
		setCursorPos(CursorPos.X+8,CursorPos.Y);
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			setCursorPos(0,CursorPos.Y+16);
		}
		cr++;
	}
	colour = bak;
}

void DisplayDriver::highlightputs(unsigned int highcolour,const char* ch) {
	char* cr = (char*)ch;
	unsigned int bak = colour;
	colour = highcolour;
	putrect(CursorPos.X,CursorPos.Y,16,getLenght(cr)*8);
	while(*cr != 0) {
		putc(*cr,CursorPos.X,CursorPos.Y);
		CursorPos.X+=8;
		if(CursorPos.X + 8 > globalFrameBuffer->Width) {
			CursorPos.X = 0;
			CursorPos.Y += 16;
		}
		cr++;
	}
	colour = bak;
}

void DisplayDriver::cursorNewLine() {
	setCursorPos(0,CursorPos.Y+16);
}

int DisplayDriver::getWidth() {
	return globalFrameBuffer->Width;
}

int DisplayDriver::getHeight() {
	return globalFrameBuffer->Height;
}

void DisplayDriver::setColour(unsigned int colo) {
	colour = colo;
}
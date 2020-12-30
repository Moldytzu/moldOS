#include "displaydriver.h"
#include "../../misc/math.h"
#include "../../memory/efiMemory.h"
#include "../../memory/pagefileallocator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

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

void DisplayDriver::putc(char ch,unsigned int xx,unsigned int yy) {
    unsigned int* pixPtr = (unsigned int*)secondFrameBuffer->BaseAddr;
    char* fontPtr = (char*)globalFont->glyphBuffer + (ch * globalFont->psf1_Header->charsize);
    for (unsigned long y = yy; y < yy + 16; y++){
        for (unsigned long x = xx; x < xx+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xx))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * secondFrameBuffer->PixelPerScanLine)) = colour;
                }
        }
        fontPtr++;
    }
}

void DisplayDriver::putc(char ch) {
	putc(ch,CursorPos.X,CursorPos.Y);
	CursorPos.X+=8;
	if(CursorPos.X + 8 > secondFrameBuffer->Width) {
		CursorPos.X = 0;
		CursorPos.Y += 16;
		checkScroll();
	}
}

void DisplayDriver::scroll() {
	for(int j = 0; j<=8;j++) {
	for(int i = 0;i<getHeight();i++) {
		for(int m = 0;m<getWidth()*4;m++) {
			*(unsigned int*)(secondFrameBuffer->BaseAddr+(i*getWidth()+m)*4) = *(unsigned int*)(secondFrameBuffer->BaseAddr+((i + 1) * getWidth() + m)*4); //cod scris de la 12 la 2 noaptea
		}
	}
	}
}

void DisplayDriver::checkScroll() {
	if(CursorPos.Y+17 >= getHeight()) { 
		scroll();
		CursorPos.X=0;
		CursorPos.Y=CursorPos.Y-17;
	} //shî asta la fel
}

void DisplayDriver::puts(const char* ch)
{
	checkScroll();
	for(int i = 0;ch[i] != '\0';i++) {
		if(ch[i] == '\n') {
			cursorNewLine();
			checkScroll();
			continue;
		}
		putc(ch[i],CursorPos.X,CursorPos.Y);
			CursorPos.X+=8;
			if(CursorPos.X + 8 > secondFrameBuffer->Width) {
				CursorPos.X = 0;
				CursorPos.Y += 16;
				checkScroll();
			}
	}
}

void DisplayDriver::puts(const char* ch, const char* ch2) {
	puts(ch);
	puts(ch2);
}

void DisplayDriver::puts(const char* ch, const char* ch2, const char* ch3) {
	puts(ch);
	puts(ch2);
	puts(ch3);
}

void DisplayDriver::puts(const char* ch, const char* ch2, const char* ch3, const char* ch4) {
	puts(ch);
	puts(ch2);
	puts(ch3);
	puts(ch4);
}

void DisplayDriver::puts(const char* ch, const char* ch2, const char* ch3, const char* ch4, const char* ch5) {
	puts(ch);
	puts(ch2);
	puts(ch3);
	puts(ch4);
	puts(ch5);
}

void DisplayDriver::setCursorPos(int x,int y) {
	CursorPos.X = x;
	CursorPos.Y = y;
}

void DisplayDriver::InitDoubleBuffer(framebuffer* f) {
	secondFrameBuffer = f;
}

void DisplayDriver::InitDisplayDriver(framebuffer* framebuf, PSF1_FONT* font) {
	globalFont = font;
	globalFrameBuffer = framebuf;
	//clearScreen(0);
	colour = 0xffffff;
}

void DisplayDriver::clearScreen(unsigned int colour) {
	memset(secondFrameBuffer->BaseAddr,colour,secondFrameBuffer->BufferSize);
	setCursorPos(0,0);
}

void DisplayDriver::putpix(int x,int y) {
	*(unsigned int*)(x+(y* secondFrameBuffer->PixelPerScanLine * 4) + secondFrameBuffer->BaseAddr) = colour;
}

void DisplayDriver::putrect(int xx,int yy,int h,int w) {
	for(unsigned int y = yy;y<(yy+h);y++) {
		for(unsigned int x = xx*4; x<(xx+w)*4;x+=4) {
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
		if(CursorPos.X + 8 > secondFrameBuffer->Width) {
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
		if(CursorPos.X + 8 > secondFrameBuffer->Width) {
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
	return secondFrameBuffer->Width;
}

int DisplayDriver::getHeight() {
	return secondFrameBuffer->Height;
}

void DisplayDriver::setColour(unsigned int colo) {
	colour = colo;
}

void DisplayDriver::update() {
	memcpy(globalFrameBuffer->BaseAddr,secondFrameBuffer->BaseAddr,globalFrameBuffer->BufferSize);
}

#pragma GCC diagnostic pop
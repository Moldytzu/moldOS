#include "displaydriver.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

DisplayDriver* GlobalDisplay;

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
	advanceCursor();
}

void DisplayDriver::scroll() {
	asmmemcpy(secondFrameBuffer->BaseAddr-((uint64_t)secondFrameBuffer->BufferSize%4),(void*)((uint64_t)secondFrameBuffer->BaseAddr+((getWidth()*4)*16)-((uint64_t)secondFrameBuffer->BufferSize%4)),globalFrameBuffer->BufferSize-((getWidth()*4)*16)-((uint64_t)secondFrameBuffer->BufferSize%4));
	memset(secondFrameBuffer->BaseAddr+globalFrameBuffer->BufferSize-((getWidth()*4)*16)-((uint64_t)secondFrameBuffer->BufferSize%4),0,(getWidth()*4*16)-((uint64_t)secondFrameBuffer->BufferSize%4));
}

void DisplayDriver::checkScroll() {
	if(CursorPos.Y+17 >= getHeight()) { 
		scroll();
		CursorPos.X=0;
		CursorPos.Y=CursorPos.Y-17;
	}
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
		advanceCursor();
	}
}

void DisplayDriver::setCursorPos(int x,int y) {
	CursorPos.X = x;
	CursorPos.Y = y;
}

void DisplayDriver::InitDoubleBuffer(DisplayBuffer* f) {
	secondFrameBuffer = f;
}
	
void DisplayDriver::InitDisplayDriver(DisplayBuffer* framebuf, PSF1_FONT* font) {
	globalFont = font;
	globalFrameBuffer = framebuf;
	colour = 0xffffff;
}

void DisplayDriver::clearScreen(unsigned int colour) {
	if(colour == 0) {
		asmmemcpy(secondFrameBuffer->BaseAddr,EmptyScreenBuffer,secondFrameBuffer->BufferSize);	
		//memset(secondFrameBuffer->BaseAddr,0,secondFrameBuffer->BufferSize);
	} else {
		for(uint64_t y = 0;y<=getHeight();y++) {
			for(uint64_t x = 0;x<=getWidth()*4;x+=4) {
				putpix(x,y,colour);
			}
		}
	}
	setCursorPos(0,0);
}

void DisplayDriver::putpix(int x,int y) {
	*(uint64_t *)(x + (y * secondFrameBuffer->PixelPerScanLine * 4) + secondFrameBuffer->BaseAddr) = colour;
}

void DisplayDriver::putpix(int x,int y,unsigned int clr) {
	*(uint64_t*)(x+(y* secondFrameBuffer->PixelPerScanLine * 4) + secondFrameBuffer->BaseAddr) = clr;
}

void DisplayDriver::putrect(int xx,int yy,int h,int w) {
	for(unsigned int y = yy;y<(yy+h);y++) {
		for(unsigned int x = xx*4; x<(xx+w)*4;x+=4) {
			putpix(x,y);
		}
	}
}

void DisplayDriver::putrect(int xx,int yy,int h,int w,unsigned int clr) {
	for(unsigned int y = yy;y<(yy+h);y++) {
		for(unsigned int x = xx*4; x<(xx+w)*4;x+=4) {
			putpix(x,y,clr);
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

void DisplayDriver::putbox(int xx,int yy,int h,int w,unsigned int clr) {
	for(unsigned int x = xx; x<(xx+w*4);x+=4) {
		putpix(x,yy,clr);
		putpix(x,(yy+h),clr);
	}
	for(unsigned int y = yy;y<(yy+h);y++) {
		putpix(xx,y,clr);
		putpix((xx+w*4),y,clr);
	}
}

void DisplayDriver::boxedputs(unsigned int highcolour,const char* ch) {
	char* cr = (char*)ch;
	putbox(CursorPos.X,CursorPos.Y,16,strlen(cr)*10,highcolour);
	puts(cr);
}

void DisplayDriver::highlightputs(unsigned int highcolour,const char* ch) {
	char* cr = (char*)ch;
	putrect(CursorPos.X,CursorPos.Y,16,strlen(cr)*10,highcolour);
	puts(cr);
}

void DisplayDriver::cursorNewLine() {
	setCursorPos(0,CursorPos.Y+16);
}

uint64_t DisplayDriver::getWidth() {
	return globalFrameBuffer->Width;
}

uint64_t DisplayDriver::getHeight() {
	return globalFrameBuffer->Height;
}

void DisplayDriver::setColour(unsigned int colo) {
	colour = colo;
}

void DisplayDriver::update() {
	asmmemcpy(globalFrameBuffer->BaseAddr,secondFrameBuffer->BaseAddr,globalFrameBuffer->BufferSize);
}

void DisplayDriver::advanceCursor() {
	CursorPos.X+=8;
	if(CursorPos.X + 8 > globalFrameBuffer->Width) {
		CursorPos.X = 0;
		CursorPos.Y += 16;
		checkScroll();
	}
}

#pragma GCC diagnostic pop
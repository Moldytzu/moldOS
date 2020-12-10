#pragma once
class DisplayDriver {
public:
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

//drawing pixels
void putpix(int x,int y);
void putrect(int x,int y,int h,int w);
void putbox(int xx,int yy,int h,int w);

//text rendering
void puts(const char* ch);
void putc(char ch,unsigned int xx,unsigned int yy);
void setCursorPos(int x,int y);
void boxedputs(unsigned int highcolour,const char* ch);
void highlightputs(unsigned int highcolour,const char* ch);
void cursorNewLine();

int getLenght(char* ch);

//misc
void clearScreen(unsigned int colour);
void setColour(unsigned int colo);

//initializare
void InitDisplayDriver(framebuffer* framebuf, PSF1_FONT* font);

//ceva geturi
int getWidth();
int getHeight();

const int BLACK = 0;
const int BLUE = 0x080064;
const int GREEN = 0x006400;
const int TURQOISE = 0x006464;
const int RED = 0x640000;
const int PINK = 0xd28c8c;
const int BROWN = 0x643000;
const int GRAY = 0xc4c4c4;
const int DARKGRAY = 0x646464;
const int LIGHTBLUE = 0x0000ff;
const int LIGHTGREEN = 0x00ff00;
const int LIGHTTURQOISE = 0x00ffff;
const int LIGHTRED = 0xff0000;
const int LIGHTPINK = 0xffc4c4;
const int YELLOW = 0xffff00;
const int WHITE = 0xffffff;
};
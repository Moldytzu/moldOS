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

void putpix(unsigned int colour,int x,int y);
void putrect(unsigned int colour,int x,int y,int h,int w);
void puts(unsigned int colour,char* ch);
void putc(unsigned int colour,char ch,unsigned int x,unsigned int y);
void setCursorPos(int x,int y);
void InitDisplayDriver(framebuffer* framebuf, PSF1_FONT* font);
void clearScreen();
void highlightputs(unsigned int colour,unsigned int highcolour,char* ch);

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
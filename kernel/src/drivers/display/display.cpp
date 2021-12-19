#include <drivers/display/display.h>

DisplayDriver* GlobalDisplay;

void DisplayDriver::putc(char ch,uint32_t xx,uint32_t yy)
{
    char* fontPtr = (char*)globalFont->glyphBuffer + (ch * globalFont->header->charsize);
    for (unsigned long y = yy; y < yy + 16; y++)
    {
        for (unsigned long x = xx; x < xx+8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xx))) > 0)
            {
                *(uint32_t*)((uint32_t*)secondFrameBuffer->BaseAddr + x + (y * secondFrameBuffer->PixelPerScanLine)) = colour;
            }
        }
        fontPtr++;
    }
}

void DisplayDriver::putc(char ch)
{
    putc(ch,CursorPos.X,CursorPos.Y);
    advanceCursor();
}

void DisplayDriver::scroll()
{
    fastmemcpy((void*)((uint64_t)secondFrameBuffer->BaseAddr-(secondFrameBuffer->BufferSize%4)),(void*)((uint64_t)secondFrameBuffer->BaseAddr+((getWidth()*4)*16)-(secondFrameBuffer->BufferSize%4)),globalFrameBuffer->BufferSize-((getWidth()*4)*16)-(secondFrameBuffer->BufferSize%4));
    memset((void*)((uint64_t)secondFrameBuffer->BaseAddr+globalFrameBuffer->BufferSize-((getWidth()*4)*16)-(secondFrameBuffer->BufferSize%4)),0,(getWidth()*4*16)-(secondFrameBuffer->BufferSize%4));
}

void DisplayDriver::checkScroll()
{
    if(CursorPos.Y+17 >= getHeight())
    {
        scroll();
        CursorPos.X=0;
        CursorPos.Y=CursorPos.Y-17;
    }
}

void DisplayDriver::puts(const char* ch)
{
    for(int i = 0; ch[i] != '\0'; i++)
    {
        if(ch[i] == '\n')
        {
            cursorNewLine();
            continue;
        }
        putc(ch[i],CursorPos.X,CursorPos.Y);
        advanceCursor();
    }
}

void DisplayDriver::setCursorPos(int x,int y)
{
    CursorPos.X = x;
    CursorPos.Y = y;
}

void DisplayDriver::InitDoubleBuffer(DisplayBuffer* f)
{
    secondFrameBuffer = f;
}

void DisplayDriver::InitDisplayDriver(DisplayBuffer* framebuf, PSFFont* font)
{
    globalFont = font;
    globalFrameBuffer = framebuf;
    colour = 0xffffff;
}

void DisplayDriver::clearScreen(uint32_t colour)
{
    if(colour == 0)
        fastmemcpy(secondFrameBuffer->BaseAddr,EmptyScreenBuffer,secondFrameBuffer->BufferSize);
    else
    {
        for(uint64_t y = 0; y<=getHeight(); y++)
        {
            for(uint64_t x = 0; x<=getWidth()*4; x+=4)
            {
                putpix(x,y,colour);
            }
        }
    }
    setCursorPos(0,0);
}

void DisplayDriver::putpix(int x,int y)
{
    *(uint32_t*)(x*4 + (secondFrameBuffer->PixelPerScanLine*y*4) + (uint64_t)secondFrameBuffer->BaseAddr) = colour;
}

void DisplayDriver::putpix(int x,int y,uint32_t clr)
{
    *(uint32_t*)(x*4 + (secondFrameBuffer->PixelPerScanLine*y*4) + (uint64_t)secondFrameBuffer->BaseAddr) = clr;
}

void DisplayDriver::cursorNewLine()
{
    setCursorPos(0,CursorPos.Y+16);
    checkScroll();
}

uint64_t DisplayDriver::getWidth()
{
    return globalFrameBuffer->Width;
}

uint64_t DisplayDriver::getHeight()
{
    return globalFrameBuffer->Height;
}

void DisplayDriver::update()
{
    fastmemcpy(globalFrameBuffer->BaseAddr,secondFrameBuffer->BaseAddr,globalFrameBuffer->BufferSize);
}

void DisplayDriver::advanceCursor()
{
    CursorPos.X+=8;
    if(CursorPos.X + 8 >= globalFrameBuffer->Width)
        cursorNewLine();
}
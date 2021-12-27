#include <drivers/display/display.h>

DisplayDriver* GlobalDisplay;

void DisplayDriver::putc(char ch,uint32_t xx,uint32_t yy)
{
    char* fontPtr = (char*)globalFont->glyphBuffer + (ch * globalFont->header->charsize); //get start of the character we want to display
    for (unsigned long y = yy; y < yy + 16; y++)
    {
        for (unsigned long x = xx; x < xx+8; x++)
        {
            //the characters are stored like this
            //0b01111110
            //0b10000001
            //0b10000001
            //0b11111111
            //0b10000001
            //0b10000001
            //0b10000001
            //0b10000001
            //so we want to get the bit that we use to index into the character
            //corresponding to the current X value
            //example: 0b10000000 >> 2 = 0b00100000
            //and then we do AND on this ^ and the current byte in the font so
            //for example we have the current byte in the font 0b01111110 and we want to get if there is a pixel
            //at the position X=6. for this we do first of all 0b10000000 >> X (6) = 0b00000010
            //and then we do 0b01111110 (the current byte) & 0b00000010 (the indexer) = 0b10
            //we display a point if the value is larger than 0, it means that the result of that and
            //we got a lighten pixel from the character.
            //if we don't get a lighten pixel, the result of the and will be 0 and thus we will skip drawing that dot.
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
    if(ch == '\n') //handle new line
    {
        cursorNewLine();
        return;
    }
    putc(ch,CursorPos.X,CursorPos.Y); //put the character at the current cursor position
    advanceCursor(); //advance the cursor
}

void DisplayDriver::scroll()
{
    #define secondBufferPixels (secondFrameBuffer->BufferSize%4)
    #define lastLineOfCharacters ((getWidth()*4)*16)
    //shift the last line of characters up by one line
    memcpy((void*)((uint64_t)secondFrameBuffer->BaseAddr-secondBufferPixels),(void*)((uint64_t)secondFrameBuffer->BaseAddr+lastLineOfCharacters-secondBufferPixels),globalFrameBuffer->BufferSize-lastLineOfCharacters-secondBufferPixels);
    memset((void*)((uint64_t)secondFrameBuffer->BaseAddr+globalFrameBuffer->BufferSize-lastLineOfCharacters-secondBufferPixels),0,lastLineOfCharacters-secondBufferPixels);
}

void DisplayDriver::checkScroll()
{
    if(CursorPos.Y+17 >= getHeight()) //check if we're overflowing
    {
        scroll();//scroll a line
        CursorPos.X=0;
        CursorPos.Y=CursorPos.Y-17; //move the cursor up
    }
}

void DisplayDriver::puts(const char* ch)
{
    for(int i = 0; ch[i] != '\0'; i++) //for each character do
    {
        if(ch[i] == '\n') //check if newline
        {
            cursorNewLine();
            continue;
        }
        putc(ch[i],CursorPos.X,CursorPos.Y); //put character
        advanceCursor(); //advance cursor
    }
}

void DisplayDriver::setCursorPos(int x,int y)
{
    CursorPos.X = x;
    CursorPos.Y = y;
}

void DisplayDriver::InitDoubleBuffer(DisplayBuffer* f)
{
    secondFrameBuffer = f; //set the back buffer
}

void DisplayDriver::InitDisplayDriver(DisplayBuffer* framebuf, PSFFont* font)
{
    globalFont = font;
    globalFrameBuffer = framebuf;
    colour = 0xffffff; //white
}

void DisplayDriver::clearScreen(uint32_t colour)
{
    if(colour == 0)
        memset(secondFrameBuffer->BaseAddr,0,secondFrameBuffer->BufferSize); //clear the buffer
    else
    {
        for(uint64_t y = 0; y<=getHeight(); y++)
        {
            for(uint64_t x = 0; x<=getWidth(); x++)
            {
                putpix(x,y,colour); //draw as a rectangle or square if the aspect ratio is 1:1 depends on how you're taking it
            }
        }
    }
    setCursorPos(0,0); //move in the upper left corner
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
    memcpy(globalFrameBuffer->BaseAddr,secondFrameBuffer->BaseAddr,globalFrameBuffer->BufferSize); //copy the back buffer onto the front one
}

void DisplayDriver::advanceCursor()
{
    CursorPos.X+=8;
    if(CursorPos.X + 8 >= globalFrameBuffer->Width) //if overflows do newline
        cursorNewLine();
}
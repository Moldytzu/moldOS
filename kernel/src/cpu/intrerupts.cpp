#include <cpu/intrerupts.h>

IDTR *idt;

void IDTDescriptorEntry::setOffset(uint64_t Offset)
{
    Offset0 = (uint16_t)(Offset & 0x000000000000ffff);
    Offset1 = (uint16_t)((Offset & 0x00000000ffff0000) >> 16);
    Offset2 = (uint32_t)((Offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTDescriptorEntry::getOffset()
{
    uint64_t offset = 0;
    offset |= (uint64_t)Offset0; //lower 16 bits
    offset |= (uint64_t)Offset1 << 16; //middle 16 bits
    offset |= (uint64_t)Offset2 << 32; //the rest of the bits
    //16+16+32=64
    return offset;
}

//Intrerrupt handlers
void GeneralProtectionFaultHandler()
{
    KernelPanic("General Protection Fault");
    while (1)
        ;
}
void PageFaultHandler()
{
    KernelPanic("Page Fault");
    while (1)
        ;
}
void DoubleFaultHandler()
{
    KernelPanic("Double Fault");
    while (1)
        ;
}

void InvalideOpcodeHandler()
{
    KernelPanic("Invalid Opcode");
    while (1)
        ;
}

void KBHandler()
{
    uint8_t keycode = inportb(0x60);
    GlobalKeyboard->Handle(keycode); //handle scancode
    PIC_EndMaster();
}

void MSHandler()
{
    uint8_t data = inportb(0x60);
    GlobalMouse->Handle(data); //handle packet
    PIC_EndSlave();
}

bool underline = false;
int times = 0;
void HandleBlink()
{
    if (underline)
        GlobalDisplay->putc('_');
    if (times == Blink_Time) //display each X intrerrupts
    {
        underline = !underline;
        times = 0; //reset counter
    }
    times++;
}

void PITHandler(InterruptStack *istack)
{
    PITTick();
    if (VirtualTerminals[CurrentTerminal].initialized) //don't display if it's not initialized
    {
        GlobalDisplay->clearScreen(BLACK); //clear screen
        GlobalDisplay->colour = WHITE; //reset style
        ANSIPrint(VirtualTerminals[CurrentTerminal].buffer); //display with style
        HandleBlink(); //handle cursor blinking
        GlobalDisplay->update(); //update screen
    }
    GlobalTaskManager->Schedule(istack); //schedule
    PIC_EndMaster();
}

void PIC_EndMaster()
{
    outportb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave()
{
    outportb(PIC2_COMMAND, PIC_EOI);
    outportb(PIC1_COMMAND, PIC_EOI);
}

void RemapPIC()
{
    uint8_t a1, a2;

    a1 = inportb(PIC1_DATA);
    io_wait();
    a2 = inportb(PIC2_DATA);
    io_wait();

    outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outportb(PIC1_DATA, 0x20);
    io_wait();
    outportb(PIC2_DATA, 0x28);
    io_wait();

    outportb(PIC1_DATA, 4);
    io_wait();
    outportb(PIC2_DATA, 2);
    io_wait();

    outportb(PIC1_DATA, ICW4_8086);
    io_wait();
    outportb(PIC2_DATA, ICW4_8086);
    io_wait();

    outportb(PIC1_DATA, a1);
    io_wait();
    outportb(PIC2_DATA, a2);
}

void CreateIntrerupt(void *handler, uint8_t offset, uint8_t typeAttributes, uint8_t selector)
{
    IDTDescriptorEntry *int_NewInt = (IDTDescriptorEntry *)(idt->Offset + offset * sizeof(IDTDescriptorEntry));
    int_NewInt->setOffset((uint64_t)handler);
    int_NewInt->Type_Attributes = typeAttributes;
    int_NewInt->Selector = selector;
}

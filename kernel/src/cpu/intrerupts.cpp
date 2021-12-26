#include <cpu/intrerupts.h>

IDTR* idt;

void IDTDescriptorEntry::setOffset(uint64_t Offset)
{
    Offset0 = (uint16_t)(Offset & 0x000000000000ffff);
    Offset1 = (uint16_t)((Offset & 0x00000000ffff0000) >> 16);
    Offset2 = (uint32_t)((Offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTDescriptorEntry::getOffset()
{
    uint64_t offset = 0;
    offset |= (uint64_t)Offset0;
    offset |= (uint64_t)Offset1 << 16;
    offset |= (uint64_t)Offset2 << 32;
    return offset;
}

void GeneralProtectionFaultHandler()
{
    KernelPanic("General Protection Fault");
    while(1);
}
void PageFaultHandler()
{
    KernelPanic("Page Fault");
    while(1);
}
void DoubleFaultHandler()
{
    KernelPanic("Double Fault");
    while(1);
}

void InvalideOpcodeHandler()
{
    KernelPanic("Invalid Opcode");
    while(1);
}

void KBHandler()
{
    uint8_t keycode = inportb(0x60);
    GlobalKeyboard->Handle(keycode);
    PIC_EndMaster();
}

void MSHandler()
{
    uint8_t data = inportb(0x60);
    GlobalMouse->Handle(data);
    PIC_EndSlave();
}

void PITHandler(InterruptStack* istack)
{
    PITTick();
    if(VirtualTerminals[CurrentTerminal].initialized)
    {
        GlobalDisplay->clearScreen(0);
        ANSIPrint(VirtualTerminals[CurrentTerminal].buffer);
        GlobalDisplay->update();
    }
    GlobalTaskManager->Schedule(istack);
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

void CreateIntrerupt(void* handler,uint8_t offset,uint8_t typeAttributes,uint8_t selector)
{
    IDTDescriptorEntry* int_NewInt = (IDTDescriptorEntry*)(idt->Offset + offset * sizeof(IDTDescriptorEntry));
    int_NewInt->setOffset((uint64_t)handler);
    int_NewInt->Type_Attributes = typeAttributes;
    int_NewInt->Selector = selector;
}

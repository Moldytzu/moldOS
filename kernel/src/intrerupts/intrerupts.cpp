#include "intrerupts.h"
#include "../userspace/userspace.h"

void GeneralProtectionFaultHandler() {
    IntreruptFrame f;
    KernelPanic("General Protection Fault",&f);
    while(1);
}
void PageFaultHandler() {
    IntreruptFrame f;
    KernelPanic("Page Fault",&f);
    while(1);
}
void DoubleFaultHandler() {
    IntreruptFrame f;
    KernelPanic("Double Fault",&f);
    while(1);
}

void InvalideOpcodeHandler() {
    IntreruptFrame f;
    KernelPanic("Invalid Opcode",&f);
    while(1);
}

void KBHandler() {
    uint8_t keycode = inportb(0x60);
    GlobalKeyboard->Handle(keycode);
    PIC_EndMaster();
}

void MSHandler() {
    uint8_t data = inportb(0x60);
    GlobalMouse->Handle(data);
    PIC_EndSlave();
}

void PITHandler() {
    PITTick();
    PIC_EndMaster();
}

/*void SYSHandler() {
    register uint64_t a asm("rax"); 
    register uint64_t b asm("rbx"); 
    register uint64_t c asm("rcx"); 
    SYSHandle(a,b,c);
}*/

void PIC_EndMaster(){
    outportb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outportb(PIC2_COMMAND, PIC_EOI);
    outportb(PIC1_COMMAND, PIC_EOI);
}
   

void RemapPIC(){
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
#include "intrerupts.h"
#include "../libc/stdio.h"
#include "../drivers/keyboard/keyboarddriver.h"

void trigger(const char* Message) {
	GlobalDisplay->clearScreen(LIGHTRED);
	GlobalDisplay->setCursorPos(0,0);
	GlobalDisplay->setColour(WHITE);
	GlobalDisplay->puts("Kernel Panic!\nMessage: ");
	GlobalDisplay->puts(Message);
    GlobalDisplay->update();
	while(1);
}

__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct IntreruptFrame* frame) {
    trigger("General Protection Fault detected!");
}
__attribute__((interrupt)) void PageFaultHandler(struct IntreruptFrame* frame) {
    trigger("Page Fault detected!");
}

__attribute__((interrupt)) void DoubleFaultHandler(struct IntreruptFrame* frame) {
    trigger("Double Fault detected!");
}

__attribute__((interrupt)) void KBHandler(struct IntreruptFrame* frame) {
    uint8_t keycode = inportb(0x60);
    GlobalKeyboard->Handle(keycode);
    EndMaster();
}

void EndMaster(){
    outportb(PIC1_COMMAND, PIC_EOI);
}

void EndSlave(){
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
#include "intrerupts.h"

void trigger(const char* Message) {
    GlobalCOM1->Write(Message);
    GlobalDisplay->clearScreen(0);
    GlobalDisplay->puts(Message);
    GlobalDisplay->update();
    while(true);
}

__attribute__((interrupt)) void DivByZeroHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void DebugHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void BoundRangeHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void InvalidOpcodeHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void DeviceUnavailableHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void InvalidTSSHandler(struct interrupt_frame* frame) {
    trigger("Invalid TSS detected!");
}
__attribute__((interrupt)) void SegmentNotPresentHandler(struct interrupt_frame* frame) {
    trigger("Segment Not Present detected!");
}
__attribute__((interrupt)) void StackFaultHandler(struct interrupt_frame* frame) {
    trigger("Stack-Segment Fault detected!");
}
__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct interrupt_frame* frame) {
    trigger("General Protection Fault detected!");
}
__attribute__((interrupt)) void PageFaultHandler(struct interrupt_frame* frame) {
    trigger("Page Fault detected!");
}
__attribute__((interrupt)) void FloatingPointHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void AligmentHandler(struct interrupt_frame* frame) {
    trigger("Aligment Check detected!");
}
__attribute__((interrupt)) void SMIDFloatHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void VirtualizationFaultHandler(struct interrupt_frame* frame) {}

__attribute__((interrupt)) void SecurityExceptionHandler(struct interrupt_frame* frame) {
    trigger("Security Exception detected!");
}
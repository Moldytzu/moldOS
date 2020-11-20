#include "../../common/utils.h"

void Reset() {
	//reset vector
	__asm__ ("jmp 0xFFFF0");
	__asm__ ("jmp 0xFFFFF0");
	Breakpoint();
}

void Shutdown() {
	//nem avut o implementare :))))
	Breakpoint();
}
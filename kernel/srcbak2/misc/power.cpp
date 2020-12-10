#include "power.h"

void* PowerDownVoid;

void Power::InitPower(void* pdwn) {
    PowerDownVoid = pdwn;
}

void Power::Shutdown() {
	((void(*)())PowerDownVoid)();
}
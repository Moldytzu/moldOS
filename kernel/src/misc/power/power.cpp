#include "power.h"

void* PowerDownVoid;
void* PowerRestartVoid;

void Power::InitPower(void* pdwn,void* rest) {
    PowerDownVoid = pdwn;
    PowerRestartVoid = rest;
}

//use acpi instead
void Power::Shutdown() {
	//((void(*)())PowerDownVoid)();
}

//use acpi instead
void Power::Restart() {
    //((void(*)())PowerRestartVoid)();
}
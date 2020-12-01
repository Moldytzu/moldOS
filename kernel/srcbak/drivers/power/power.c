#include "../../common/utils.h"

void* PowerDwn;

void InitPower(void* PowDwn) {
	PowerDwn = PowDwn;
}

void Reset() {
	((void(*)())PowerDwn)(1);
	Breakpoint();
}

void Shutdown() {
	((void(*)())PowerDwn)(0);
	Breakpoint();
}
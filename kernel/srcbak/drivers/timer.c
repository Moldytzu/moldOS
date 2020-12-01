#define cmos_address 0x70
#define cmos_data 0x71

void Wait(int sec) {
	int currentsec = getRTCtime()+sec;
	while(currentsec != getRTCtime()) {
		doNothing();
	}
}

unsigned char getRTCregister(int reg) {
	outportb(cmos_address,reg);
	unsigned char toRet = inportb(cmos_data);
	return toRet;
}

int getRTCtime() {
	return getRTCregister(0x00)+getRTCregister(0x02)+getRTCregister(0x04)+getRTCregister(0x07)+getRTCregister(0x08)+getRTCregister(0x09);
}

int getRTCsecond() {
	return getRTCregister(0x00);
}

int getRTChours() {
	return getRTCregister(0x04);
}

int getRTCyear() {
	return getRTCregister(0x09);
}

int getRTCminutes() {
	return getRTCregister(0x02);
}
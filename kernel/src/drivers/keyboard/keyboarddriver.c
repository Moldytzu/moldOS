void InitKeyboard() {
	outportb(0x64,0xAE);
	outportb(0x64,0xA8);
}

void WaitForKey() {
	while (!(inportb(0x64) & 0x1)){doNothing();}
}

int GetKey() {
	WaitForKey();
	return inportb(0x60);
}

int GetKeyRaw() {
	return inportb(0x60);
}
int memcmp(const void* aptr,const void* bptr,int n) {
	const unsigned char* a = aptr,*b = bptr;
	for(int i = 0;i<n;i++) {
		if(a[i]<b[i]) return -1;
		else if(a[i] > b[i]) return 1;
	}
	return 0;
}

int getLowMemory() {
    unsigned short total;
    unsigned char lowmem, highmem;
 
    outportb(0x70, 0x30);
    lowmem = inportb(0x71);
    outportb(0x70, 0x31);
    highmem = inportb(0x71);
 
    total = lowmem | highmem << 8;
    return total;
}
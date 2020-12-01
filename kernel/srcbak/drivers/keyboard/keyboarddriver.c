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

void WriteKey(unsigned int color) {
	switch(GetKey()) {
		case 2:
			puts(color,"1");
			break;
		case 3:
			puts(color,"2");
			break;
		case 4:
			puts(color,"3");
			break;
		case 5:
			puts(color,"4");
			break;
		case 6:
			puts(color,"5");
			break;
		case 7:
			puts(color,"6");
			break;
		case 8:
			puts(color,"7");
			break;
		case 9:
			puts(color,"8");
			break;
		case 10:
			puts(color,"9");
			break;
		case 11:
			puts(color,"0");
			break;
		case 12:
			puts(color,"-");
			break;
		case 13:
			puts(color,"=");
			break;
		case 16:
			puts(color,"q");
			break;
		case 17:
			puts(color,"w");
			break;
		case 18:
			puts(color,"e");
			break;
		case 19:
			puts(color,"r");
			break;
		case 20:
			puts(color,"t");
			break;
		case 21:
			puts(color,"y");
			break;
		case 22:
			puts(color,"u");
			break;
		case 23:
			puts(color,"i");
			break;
		case 24:
			puts(color,"o");
			break;
		case 25:
			puts(color,"p");
			break;
		case 26:
			puts(color,"[");
			break;
		case 27:
			puts(color,"]");
			break;
		case 30:
			puts(color,"a");
			break;
		case 31:
			puts(color,"s");
			break;
		case 32:
			puts(color,"d");
			break;
		case 33:
			puts(color,"f");
			break;
		case 34:
			puts(color,"g");
			break;
		case 35:
			puts(color,"h");
			break;
		case 36:
			puts(color,"j");
			break;
		case 37:
			puts(color,"k");
			break;
		case 38:
			puts(color,"l");
			break;
		case 39:
			puts(color,";");
			break;
		case 40:
			puts(color,"\'");
			break;
		case 44:
			puts(color,"z");
			break;
		case 45:
			puts(color,"x");
			break;
		case 46:
			puts(color,"c");
			break;
		case 47:
			puts(color,"v");
			break;
		case 48:
			puts(color,"b");
			break;
		case 49:
			puts(color,"n");
			break;
		case 50:
			puts(color,"m");
			break;
		case 53:
			puts(color,"/");
			break;
		case 51:
			puts(color,",");
			break;
		case 52:
			puts(color,".");
			break;
		case 57:
			puts(color," ");
			break;
	}
}
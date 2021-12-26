#include <terminal/ansi.h>

void ANSIPrint(const char* text) {
    GlobalDisplay->puts(text);
}
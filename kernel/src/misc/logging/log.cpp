#include "log.h"

void LogInfo(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(LIGHTBLUE);
    GlobalDisplay->puts("INFO");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();
    GlobalCOM1->Write(SERIALBLUE,"[INFO] ",SERIALWHITE,text,"\n");
}

void LogWarn(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(YELLOW);
    GlobalDisplay->puts("WARN");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();
    GlobalCOM1->Write(SERIALYELLOW,"[WARN] ",SERIALWHITE,text,"\n");
}

void LogError(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(LIGHTRED);
    GlobalDisplay->puts("ERROR");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();  
    GlobalCOM1->Write(SERIALRED,"[ERROR] ",SERIALRED,text,"\n");
}
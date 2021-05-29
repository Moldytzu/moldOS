#include "log.h"
#include "../../settings.h"

void LogInfo(const char* text) {
    GlobalDisplay->puts("[");
    GlobalDisplay->setColour(LIGHTBLUE);
    GlobalDisplay->puts("INFO");
    GlobalDisplay->setColour(WHITE);
    GlobalDisplay->puts("] ");
    GlobalDisplay->puts(text);
    GlobalDisplay->cursorNewLine();
    GlobalDisplay->update();
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALBLUE,"[INFO] ",SERIALWHITE,text,"\n");
    #endif
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
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALYELLOW,"[WARN] ",SERIALWHITE,text,"\n");
    #endif
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
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALRED,"[ERROR] ",SERIALRED,text,"\n");
    #endif
}
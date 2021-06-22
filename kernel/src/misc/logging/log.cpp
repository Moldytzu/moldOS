#include "log.h"
#include "../../settings.h"

void LogInfo(const char* text) {
    printf("[%coINFO%co] %s\n",LIGHTBLUE,WHITE,text);
    GlobalDisplay->update();
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALBLUE,"[INFO] ",SERIALWHITE,text,"\n");
    #endif
}

void LogWarn(const char* text) {
    printf("[%coWARN%co] %s\n",YELLOW,WHITE,text);
    GlobalDisplay->update();
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALYELLOW,"[WARN] ",SERIALWHITE,text,"\n");
    #endif
}

void LogError(const char* text) {
    printf("[%coERROR%co] %s\n",LIGHTRED,WHITE,text);
    GlobalDisplay->update();  
    #ifdef Logging_Serial
    GlobalCOM1->Write(SERIALRED,"[ERROR] ",SERIALRED,text,"\n");
    #endif
}
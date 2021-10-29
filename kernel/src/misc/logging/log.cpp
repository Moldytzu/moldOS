#include <misc/logging/log.h>

void LogInfo(const char* text) {
    printf("[%co%d:%d:%d/INFO%co] %s\n",LIGHTBLUE,RTCreadHours(),RTCreadMinutes(),RTCreadSeconds(),WHITE,text);
    GlobalDisplay->update();
    #ifdef Logging_Serial
    SerialWrite(SERIAL_BLUE,"[INFO] ",SERIAL_WHITE,text,"\n");
    #endif
}

void LogWarn(const char* text) {
    printf("[%co%d:%d:%d/WARN%co] %s\n",YELLOW,RTCreadHours(),RTCreadMinutes(),RTCreadSeconds(),WHITE,text);
    GlobalDisplay->update();
    #ifdef Logging_Serial
    SerialWrite(SERIAL_YELLOW,"[WARN] ",SERIAL_WHITE,text,"\n");
    #endif
}

void LogError(const char* text) {
    printf("[%co%d:%d:%d/ERROR%co] %s\n",LIGHTRED,RTCreadHours(),RTCreadMinutes(),RTCreadSeconds(),WHITE,text);
    GlobalDisplay->update();  
    #ifdef Logging_Serial
    SerialWrite(SERIAL_RED,"[ERROR] ",SERIAL_RED,text,"\n");
    #endif
}
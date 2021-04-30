#pragma once
#include "ports.h"

class SerialPort
{
public:
    void Init();
    void Write(char chr);
    void Write(const char* chr);
    void Write(const char* chr,const char* chr2);
    void Write(const char* chr,const char* chr2,const char* chr3);
    void Write(const char* chr,const char* chr2,const char* chr3,const char* chr4);
    void Write(const char* chr,const char* chr2,const char* chr3,const char* chr4,const char* chr5);
    char Read();
    void ClearMonitor();
    int isTransmitEmpty();
    int isReceived();
};

extern SerialPort* GlobalCOM1;
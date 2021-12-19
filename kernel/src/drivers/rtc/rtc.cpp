#include <drivers/rtc/rtc.h>

uint32_t BCDToBinary(uint32_t bcd)
{
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

void RTCwaitUpdate()
{
    int old = RTCgetRegister(0x0);
    while(RTCgetRegister(0x0)==old);
}

int RTCgetUpdateInProgress()
{
    outportb(CMOS_ADDRESS, 0x0A);
    return (inportb(CMOS_DATA) & 0x80);
}

unsigned char RTCgetRegister(int reg)
{
    outportb(CMOS_ADDRESS, reg);
    return inportb(CMOS_DATA);
}

uint32_t RTCreadSeconds()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x0));
}

uint32_t RTCreadHours()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x4));
}

uint32_t RTCreadMinutes()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x2));
}

uint32_t RTCreadDay()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x7));
}

uint32_t RTCreadMonth()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x8));
}

uint32_t RTCreadYear()
{
    while(RTCgetUpdateInProgress());
    return BCDToBinary(RTCgetRegister(0x9));
}

uint32_t RTCreadTime()
{
    return RTCreadHours()*3600+RTCreadMinutes()*60+RTCreadSeconds();
}

void RTCwaitSeconds(uint32_t secs)
{
    RTCwaitUpdate();
    int lastsec = RTCreadTime()+secs;
    while(lastsec != RTCreadTime()) {}
}
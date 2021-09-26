#include "rtc.h"

void RTCwaitUpdate() {
    int old = RTCgetRegister(0x0);
    while(RTCgetRegister(0x0)==old);
}

int RTCgetUpdateInProgress() {
    outportb(0x70, 0x0A);
    return (inportb(0x71) & 0x80);
}

unsigned char RTCgetRegister(int reg) {
    outportb(0x70, reg);
    return inportb(0x71);
}

uint32_t RTCreadSeconds() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x0);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadHours() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x4);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadMinutes() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x2);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadDay() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x7);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadMonth() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x8);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadYear() {
    while(RTCgetUpdateInProgress());
    int second = RTCgetRegister(0x9);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RTCreadTime() {
    return RTCreadHours()*3600+RTCreadMinutes()*60+RTCreadSeconds();
}

void RTCwaitSeconds(uint32_t secs) {
    RTCwaitUpdate();
    int lastsec = RTCreadTime()+secs;
    while(lastsec != RTCreadTime()){}
}
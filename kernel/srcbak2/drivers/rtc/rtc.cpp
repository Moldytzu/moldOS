#include "rtc.h"

unsigned char getRegister(int reg) {
    outportb(0x70, reg);
    return inportb(0x71);
}

uint32_t RealTimeClock::readSecond() {
    int second = getRegister(0x0);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readHours() {
    int second = getRegister(0x4);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readMinutes() {
    int second = getRegister(0x2);
    return (second & 0x0F) + ((second / 16) * 10);
}

uint32_t RealTimeClock::readTime() {
    return readHours()*3600+readMinutes()*60+readSecond();
}

void RealTimeClock::waitSeconds(uint32_t secs) {
    int lastsec = readTime()+secs;
    while(lastsec != readTime()){}
}
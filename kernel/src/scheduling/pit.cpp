#include <scheduling/pit.h>

double TimeSinceBoot = 0;
uint16_t Divisor = 65535;

void PITSleep(double seconds) {
    double start = TimeSinceBoot;
    while(TimeSinceBoot < start + seconds) {
        asm("hlt");
    }
}
void PITSleep(uint64_t ms) {
    PITSleep((double)ms / 1000);
}

void PITSetDivisor(uint16_t divisor) {
    Divisor = divisor;
    outportb(0x40,(uint8_t)(divisor & 0x00FF));
    io_wait();
    outportb(0x40,(uint8_t)((divisor & 0xFF00) >> 8));
}

uint64_t PITGetFrequency() {
    return PITBaseFrequency / Divisor;
}

void PITSetFrequency(uint64_t frequency) {
    PITSetDivisor(PITBaseFrequency / frequency);
}

void PITTick() {
    TimeSinceBoot += 1 / (double)PITGetFrequency();
}

void PIT2SetFreq(uint32_t frequency) {
    int zdivisor = PITBaseFrequency / frequency;
    outportb(0x43, 0xb6);
    outportb(0x42, zdivisor & 0xFF); //low
    outportb(0x42, zdivisor >> 8);   //high
}
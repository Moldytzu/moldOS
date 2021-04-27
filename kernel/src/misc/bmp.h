#pragma once
#include <stdint.h>

struct uint24 {
	unsigned uint24 : 24;
} __attribute__((packed));

struct BMPHeader {
    char rsrv[2];
    uint32_t Size;
    uint16_t rsrv2[2];
    uint32_t ImgOffset;
    uint32_t HeaderSize;
    uint32_t Width;
    uint32_t Height;
    uint16_t Planes;
    uint16_t BPP;
    uint32_t Compression;
    uint32_t ImageSize;
    uint32_t XPPM;
    uint32_t YPPM;
    uint32_t CLR;
    uint32_t ICLR;
}__attribute__((packed));
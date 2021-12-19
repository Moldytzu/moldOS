#pragma once
#include <stdint.h>

struct PSFHeader
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
};

struct PSFFont
{
    PSFHeader* header;
    void* glyphBuffer;
};
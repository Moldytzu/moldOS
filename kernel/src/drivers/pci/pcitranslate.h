#pragma once
#include <stdint.h>
#include "pcidefs.h"
    
const char* PCITranslateClass(uint8_t classid);
const char* PCITranslateSubClass(uint8_t classid,uint8_t subclassid);
const char* PCITranslateVendor(uint16_t vendorid);
const char* PCITranslateDeviceID(uint16_t vendorid,uint16_t deviceid);
TranslatedPCIDevice PCITranslateDevice(struct PCIDevice* device);

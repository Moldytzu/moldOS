#pragma once
#include <stdint.h>

typedef struct {
  uint8_t   Header[8];                        //EDID header "00 FF FF FF FF FF FF 00"
  uint16_t  ManufactureName;                  //EISA 3-character ID
  uint16_t  ProductCode;                      //Vendor assigned code
  uint32_t  SerialNumber;                     //32-bit serial number
  uint8_t   WeekOfManufacture;                //Week number
  uint8_t   YearOfManufacture;                //Year
  uint8_t   EdidVersion;                      //EDID Structure Version
  uint8_t   EdidRevision;                     //EDID Structure Revision
  uint8_t   VideoInputDefinition;
  uint8_t   MaxHorizontalImageSize;           //cm
  uint8_t   MaxVerticalImageSize;             //cm
  uint8_t   DisplayTransferCharacteristic;
  uint8_t   FeatureSupport;
  uint8_t   RedGreenLowBits;                  //Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0
  uint8_t   BlueWhiteLowBits;                 //Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0
  uint8_t   RedX;                             //Red-x Bits 9 - 2
  uint8_t   RedY;                             //Red-y Bits 9 - 2
  uint8_t   GreenX;                           //Green-x Bits 9 - 2
  uint8_t   GreenY;                           //Green-y Bits 9 - 2
  uint8_t   BlueX;                            //Blue-x Bits 9 - 2
  uint8_t   BlueY;                            //Blue-y Bits 9 - 2
  uint8_t   WhiteX;                           //White-x Bits 9 - 2
  uint8_t   WhiteY;                           //White-x Bits 9 - 2
  uint8_t   EstablishedTimings[3];
  uint8_t   StandardTimingIdentification[16];
  uint8_t   DetailedTimingDescriptions[72];
  uint8_t   ExtensionFlag;                    //Number of (optional) 128-byte EDID extension blocks to follow
  uint8_t   Checksum;
} EDIDInfo;
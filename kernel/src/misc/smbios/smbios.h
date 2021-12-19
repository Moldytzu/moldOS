#include <stdint.h>

struct SMBiosHeader
{
    char Signature[4];
    uint8_t Checksum;            
    uint8_t Length;              
    uint8_t MajorVersion;       
    uint8_t MinorVersion;       
    uint16_t MaxStructureSize;    
    uint8_t EntryPointRevision; 
    char FormattedArea[5];      
    char Signature2[5];    
    uint8_t Checksum2;           
    uint16_t TableLength;        
    unsigned int TableAddress;   
    uint16_t NumberOfStructures; 
    uint8_t BCDRevision;         
};
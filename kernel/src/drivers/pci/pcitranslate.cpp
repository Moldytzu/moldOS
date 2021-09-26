#include "pcitranslate.h"
#include "../../libc/stdio/cstring.h"

TranslatedPCIDevice PCITranslateDevice(PCIDevice* device) {
    TranslatedPCIDevice newDevice;
    newDevice.BIST = device->BIST;
    newDevice.CacheLineSize = device->CacheLineSize;
    newDevice.Class = PCITranslateClass(device->Class);
    newDevice.Command = device->Command;
    newDevice.DeviceID = PCITranslateDeviceID(device->VendorID,device->DeviceID);
    newDevice.HeaderType = device->HeaderType;
    newDevice.LatencyTimer = device->LatencyTimer;
    newDevice.ProgramInterface = device->ProgramInterface;
    newDevice.RevisionID = device->RevisionID;
    newDevice.Status = device->Status;
    newDevice.Subclass = PCITranslateSubClass(device->Class,device->Subclass);
    newDevice.VendorID = PCITranslateVendor(device->VendorID); 
    return newDevice;
}

const char* PCITranslateClass(uint8_t classid) {
    switch (classid)
    {
    case 0:
        return "Unclassified";
        break;
    case 0x1:
        return "Mass Storage Controller";
        break;
    case 0x2:
        return "Network Controller";
        break;
    case 0x3:
        return "Display Controller";
        break;
    case 0x4:
        return "Multimedia Controller";
        break;
    case 0x5:
        return "Memory Controller";
        break;
    case 0x6:
        return "Bridge Device";
        break;    
    case 0x7:
        return "Simple Communication Controller";
        break;
    case 0x8:
        return "Base System Peripheral";
        break;
    case 0x9:
        return "Input Device Controller";
        break;
    case 0xA:
        return "Docking Station";
        break;
    case 0xB:
        return "Processor";
        break;
    case 0xC:
        return "Serial Bus Controller";
        break;
    case 0xD:
        return "Wireless Controller";
        break; 
    case 0xE:
        return "Intelligent Controller";
        break;
    case 0xF:
        return "Satellite Communication Controller";
        break;
    case 0x10:
        return "Encryption Controller";
        break;
    case 0x11:
        return "Signal Processing Controller";
        break;
    case 0x12:
        return "Processing Accelerator";
        break;
    case 0x13:
        return "Non-Essential Instrumentation";
        break;
    case 0x40:
        return "Co-Processor";
        break;
    default:
        return "Unclassified";
        break;
    }
}
const char* PCITranslateSubClass(uint8_t classid,uint8_t subclassid) {
    switch (classid)
    {
    case 0:
        switch (subclassid)
        {
            case 0x01:
                return "VGA-Compatible Device";
                break;
            default:
                return "Non-VGA-Compatible";
                break;
        }
        break;
    case 0x1:
        switch (subclassid)
        {
            case 0:
                return "SCSI Bus Controller";
                break;
            case 0x1:
                return "IDE Controller";
                break;
            case 0x2:
                return "Floppy Disk Controller";
                break;
            case 0x3:
                return "IPI Bus Controller";
                break;
            case 0x4:
                return "RAID Controller";
                break;
            case 0x5:
                return "ATA Controller";
                break;
            case 0x6:
                return "Serial ATA";
                break;
            case 0x7:
                return "Serial Attached SCSI";
                break;
            case 0x8:
                return "Non-Volatile Memory Controller";
                break;           
            default:
                return "Other";
                break;
        }
        break;
    case 0x2:
        switch (subclassid)
        {
            case 0:
                return "Ethernet Controller";
                break;
            case 0x1:
                return "Token Ring Controller";
                break;
            case 0x2:
                return "FDDI Controller";
                break;
            case 0x3:
                return "ATM Controller";
                break;
            case 0x4:
                return "ISDN Controller";
                break;
            case 0x5:
                return "WorldFip Controller";
                break;
            case 0x6:
                return "PICMG 2.14 Multi Computing";
                break;
            case 0x7:
                return "Infiniband Controller";
                break;
            case 0x8:
                return "Fabric Controllerr";
                break;           
            default:
                return "Other";
                break;
        }
        break;
    case 0x3:
        switch (subclassid)
        {
            case 0:
                return "VGA Compatible Controller";
                break;
            case 0x1:
                return "XGA Controller";
                break;
            case 0x2:
                return "3D Controller (Not VGA-Compatible)";
                break;           
            default:
                return "Other";
                break;
        }
        break;
    case 0x4:
        switch (subclassid)
        {
            case 0:
                return "Multimedia Video Controller";
                break;
            case 0x1:
                return "Multimedia Audio Controller";
                break;
            case 0x2:
                return "Computer Telephony Device";
                break;
            case 0x3:
                return "Audio Device";
                break;
            default:
                return "Other";
                break;
        }
        break;
    case 0x5:
        switch (subclassid)
        {
            case 0:
                return "RAM Controller";
                break;
            case 0x1:
                return "Flash Controller";
                break;
            default:
                return "Other";
                break;
        }
        break;
    case 0x6:
        switch (subclassid)
        {
            case 0:
                return "Host Bridge";
                break;
            case 0x1:
                return "ISA Bridge";
                break;
            case 0x2:
                return "EISA Bridge";
                break;
            case 0x3:
                return "MCA Bridge";
                break;
            case 0x4:
                return "PCI-to-PCI Bridge";
                break;
            case 0x5:
                return "PCMCIA Bridge";
                break;
            case 0x6:
                return "NuBus Bridge";
                break;
            case 0x7:
                return "CardBus Bridge";
                break;
            case 0x8:
                return "RACEway Bridge";
                break;    
            case 0x9:
                return "PCI-to-PCI Bridge";
                break;  
            case 0xA:
                return "InfiniBand-to-PCI Host Bridge";
                break;         
            default:
                return "Other";
                break;
        }
        break;
    case 0x7:
        switch (subclassid)
        {
            case 0:
                return "Serial Controller";
                break;
            case 0x1:
                return "Parallel Controller";
                break;
            case 0x2:
                return "Multiport Serial Controller";
                break;
            case 0x3:
                return "Modem";
                break;
            case 0x4:
                return "IEEE 488.1/2 (GPIB) Controller";
                break;
            case 0x5:
                return "Smart Card";
                break;
            default:
                return "Other";
                break;
        }
        break;
    case 0x8:
        switch (subclassid)
        {
            case 0:
                return "PIC";
                break;
            case 0x1:
                return "DMA Controller";
                break;
            case 0x2:
                return "Timer";
                break;
            case 0x3:
                return "RTC Controller";
                break;
            case 0x4:
                return "PCI Hot-Plug Controller";
                break;
            case 0x5:
                return "SD Host controller";
                break;
            case 0x6:
                return "IOMMU";
                break;
            default:
                return "Other";
                break;
        }
        break;
    case 0x9:
        switch (subclassid)
        {
            case 0:
                return "Keyboard Controller";
                break;
            case 0x1:
                return "Digitizer Pen";
                break;
            case 0x2:
                return "Mouse Controller";
                break;
            case 0x3:
                return "Scanner Controller";
                break;
            case 0x4:
                return "Gameport Controller";
                break;
            default:
                return "Other";
                break;
        }
        break;
    case 0xA:
        switch (subclassid)
        {
            case 0:
                return "Generic";
                break;         
            default:
                return "Other";
                break;
        }
        break;
    case 0xB:
        switch (subclassid)
        {
            case 0:
                return "386";
                break;
            case 0x1:
                return "486";
                break;
            case 0x2:
                return "Pentium";
                break;
            case 0x3:
                return "Pentium Pro";
                break;
            case 0x10:
                return "Alpha";
                break;
            case 0x20:
                return "PowerPC";
                break;
            case 0x30:
                return "MIPS";
                break;
            case 0x40:
                return "Co-Processor";
                break;         
            default:
                return "Other";
                break;
        }
        break;
    case 0xC:
        switch (subclassid)
        {
            case 0:
                return "FireWire (IEEE 1394) Controller";
                break;
            case 0x1:
                return "ACCESS Bus";
                break;
            case 0x2:
                return "SSA";
                break;
            case 0x3:
                return "USB Controller";
                break;
            case 0x4:
                return "Fibre Channel";
                break;
            case 0x5:
                return "SMBus";
                break;
            case 0x6:
                return "InfiniBand";
                break;
            case 0x7:
                return "IPMI Interface";
                break;
            case 0x8:
                return "SERCOS Interface (IEC 61491)";
                break;
            case 0x9:
                return "CANbus";
                break;          
            default:
                return "Other";
                break;
        }
        break;
    case 0xD:
        switch (subclassid)
        {
            case 0:
                return "iRDA Compatible Controller";
                break;
            case 0x1:
                return "Consumer IR Controller";
                break;
            case 0x10:
                return "RF Controller";
                break;
            case 0x11:
                return "Bluetooth Controller";
                break;
            case 0x12:
                return "Broadband Controller";
                break;
            case 0x20:
                return "Ethernet Controller (802.1a)";
                break;
            case 0x21:
                return "Ethernet Controller (802.1b)";
                break;       
            default:
                return "Other";
                break;
        }
        break;
    case 0xE:
        switch (subclassid)
        {         
            default:
                return "I20";
                break;
        }
        break;
    case 0xF:
        switch (subclassid)
        {
            case 0x1:
                return "Satellite TV Controller";
                break;
            case 0x2:
                return "Satellite Audio Controller";
                break;
            case 0x3:
                return "Satellite Voice Controller";
                break;
            case 0x4:
                return "Satellite Data Controller";
                break;
        }
        break;
    case 0x10:
        switch (subclassid)
        {
            case 0:
                return "Network and Computing Encrpytion/Decryption";
                break;
            case 0x10:
                return "Entertainment Encryption/Decryption";
                break;         
            default:
                return "Other";
                break;
        }
        break;
    case 0x11:
        switch (subclassid)
        {
            case 0:
                return "DPIO Modules";
                break;
            case 0x1:
                return "Performance Counters";
                break;
            case 0x10:
                return "Communication Synchronizer";
                break;
            case 0x20:
                return "Signal Processing Management";
                break;        
            default:
                return "Other";
                break;
        }
        break;
    default:
        break;
    }
    return "";
}
const char* PCITranslateVendor(uint16_t vendorid) {
    switch (vendorid)
    {
    case 0x8086:
        return "Intel Corp.";
        break;
    case 0x1234:
        return "Brain Actuated Technologies";
        break;
    case 0x10DE:
        return "NVIDIA Corp.";
        break;
    default:
        return inttohstr(vendorid);
        break;
    }
}
const char* PCITranslateDeviceID(uint16_t vendorid,uint16_t deviceid) {
    switch (vendorid)
    {
    case 0x1234:
        switch (deviceid)
        {
        case 0x1111:
            return "QEMU Virtual Video Controller";
            break;
        
        default:
            return inttohstr(deviceid);
            break;
        }
        break;
    case 0x8086:
        switch (deviceid)
        {
        case 0x9b63:
            return "H410 Host Bridge";
            break;
        case 0xa3af:
            return "H410 USB 3 Controller";
            break;
        case 0xa3b1:
            return "H410 Signal Processing Controller";
            break;
        case 0xa3ba:
            return "H410 Communication Controller";
            break;
        case 0xa382:
            return "H410 SATA Controller";
            break;
        case 0xa3da:
            return "H410 ISA Bridge";
            break;
        case 0xa3a1:
            return "H410 Memory Controller";
            break;
        case 0xa3f0:
            return "H410 Audio Device";
            break;
        case 0xa3a3:
            return "H410 System Management Bus";
            break;
        case 0x1911:
            return "Xeon E3-1200 v5/v6 / E3-1500 v5 / 6th/7th Gen Core Processor Gaussian Mixture Model";
            break;
        case 0x1901:
            return "Xeon E3-1200 v5/E3-1500 v5/6th Gen Core Processor PCIe Controller (x16)";
            break;
        case 0x0D55:
            return "Ethernet Connection (12) I219-V";
            break;
        case 0x29c0:
            return "82G33/G31/P35/P31 Express DRAM Controller";
            break;
        case 0x10d3:
            return "82574L Gigabit Network Connection";
            break;
        case 0x2934:
            return "82801I (ICH9 Family) USB UHCI Controller";
            break;
        case 0x2935:
            return "82801I (ICH9 Family) USB UHCI Controller";
            break;
        case 0x2936:
            return "82801I (ICH9 Family) USB UHCI Controller";
            break;
        case 0x293A:
            return "82801I (ICH9 Family) USB2 EHCI Controller";
            break;
        case 0x2918:
            return "82801IB (ICH9) LPC Interface Controller";
            break;
        case 0x2922:
            return "82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller";
            break;        
        case 0x2930:
            return "82801I (ICH9 Family) SMBus Controller";
            break; 
        default:
            return inttohstr(deviceid);
            break;
        }
        break;
    case 0x10DE:
        switch (deviceid)
        {
        case 0x1C82:
            return "GP107 [Geforce GTX 1050 Ti]";
            break;
        case 0x0FB9:
            return "GP107GL High Definition Audio Controller";
            break;
        default:
            return inttohstr(deviceid);
            break;
        }
        break;
    default:
        return inttohstr(deviceid);
        break;
    }
}
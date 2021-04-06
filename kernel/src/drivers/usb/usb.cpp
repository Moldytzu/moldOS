#include "usb.h"

#define ResetFlag 1 << 2

UHCI_REGISTERS* UHCIRegisters;
uint16_t BasePort;
void* UHCIStack = NULL;

void UHCIConfigure () {
    outportw(BasePort+4,0); //enabling int
    outportw(BasePort+6,0);
    UHCIStack = GlobalAllocator.RequestPage();
    uint32_t* z = (uint32_t*)UHCIStack;
    for(int i = 0;i<1024;i++)
        z[i] = 1 << 0; // setting all the uhci stack to terminate flag
    
    outportl(BasePort+8,(uint32_t)(uint64_t)UHCIStack); //setting uhci stack so uhci controller can use it
    outportb(BasePort+12,0x40);
    outportw(BasePort+2,0x1F);

    outportw(BasePort+0,CMD_MAX_PACKET | CMD_CONFIG_DONE | CMD_RUN_STOP); // telling uhci controller that the configuration is done
}

void UHCIInit(PCIDevice* device) {
    UHCIRegisters = (UHCI_REGISTERS*)((PCIHeader0*)device)->BAR4;
    BasePort = (uint16_t)(uint64_t)(void*)((PCIHeader0*)device)->BAR4;
    printf("Found UHCI USB controller!\n");GlobalDisplay->update();
    UHCIConfigure();
    printf("Configured UHCI USB Controller!\n");GlobalDisplay->update();

    //printf("Registers:\n USBCMD:%x\n USBSTS:%x\n USBINTR:%x\n FRNUM:%x\n FRBASEADD:%x\n SOFMOD:%x\n PORTSC1:%x\n PORTSC2:%x\n",UHCIRegisters->USBCMD,UHCIRegisters->USBSTS,UHCIRegisters->USBINTR,UHCIRegisters->FRNUM,UHCIRegisters->FRBASEADD,UHCIRegisters->SOFMOD,UHCIRegisters->PORTSC1,UHCIRegisters->PORTSC2);
    
}
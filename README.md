# LowLevelOS (LLOS)

LLOS is a W.I.P operating system writen in C and C++ that targets x86_64 hardware.

It aims to be easy to use, small and simple.

<p float="left">
<img width="37%" src="https://forthebadge.com/images/badges/works-on-my-machine.svg">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<img width="25%" src="https://forthebadge.com/images/badges/0-percent-optimized.svg">
</p>

# Features:
 - Drivers:
    - EFI GOP (Graphics Output Protocol)
         - PSF1 Font
         - 32-bit Colours
    - RTC (Real Time Clock) 
    - PS/2 Mouse&Keyboard
    - AHCI
         - Read SATA and SATAPI drives
    - PCI
 - Hardware Communications:
    - Parallel Port (LPT1)
    - Serial Port (COM1)
    - PCI (Peripheral Component Interconnect)
 - Misc:
    - CPU Info
         - Name
         - Vendor
         - Features
         - Temperature
    - Paging
    - Shutdown&Reboot
         - ACPI
         - UEFI Runtime Services
    - Userspace
         - Cooperative Multitasking
         - System Calls
    - Kernel LibC

# Photo Galery:
NOTICE: The photos may be outdated!

![](https://i.imgur.com/nE6Pm7g.png)
LLOS running 2 tasks (A and B)

![](https://i.imgur.com/7539MjY.png)
LLOS kernel panic

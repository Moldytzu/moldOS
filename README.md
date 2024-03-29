# moldOS

moldOS is a basic operating system writen in C and C++ that targets x86_64 hardware.

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
 - Hardware Communications:
    - Serial Port (COM)
    - PCI
    - ACPI
 - Misc:
    - CPU Info
         - Name
         - Vendor
         - Features
         - Temperature
    - Userspace
         - Preemptive Multitasking
         - System Calls
         - ELF
    - Kernel LibC
 - Filesystem:
     - LLFS 1.0

# Building:
To build moldOS you need to run a x86_64 Debian-based Linux distro. Run the prepare.sh script, then run buildtoolchain.sh (it will take a while), and then run ./build.sh

# Running:
Run run.sh to start the virtual machine with moldOS installed.

# Contributing:
Before commiting anything, run the style.sh script. It styles the source code using the astyle formater.
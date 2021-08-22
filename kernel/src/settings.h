#pragma once

//LLOS Build Settings
//Comment to disable an option
//Uncomment to enable

//=========
//Debugging
//=========
//#define Debugging_AHCI //display AHCI ports
//#define Debugging_ACPI //shows ACPI-related stuff
//#define Debugging_Scheduler //show how the tasks are switched

//========
//Hardware
//========
#define Serial_Console //enables the serial console

//====
//Misc
//====
//#define Logging_Serial //logs in the serial console everything that's logged on the screen
#define Quiet //makes the kernel boot up process quiet

#pragma once

//moldOS Build Settings
//Comment to disable a setting
//Uncomment to enable

//=========
//Debugging
//=========
//#define Debugging_ACPI //debug ACPI
#define Debugging_Scheduler //debug userspace and the scheduler

//========
//Hardware
//========
#define Serial_Console //enables the serial console

//====
//Misc
//====
//#define Logging_Serial //logs in the serial console everything that's logged with the LogInfo,LogWarn and LogError functions
//#define Quiet //makes the kernel boot up process quiet
#define Blink //enable blinking cursor
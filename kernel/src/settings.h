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

//=====
//Video
//=====
#define Blink_Time 10 //interval in which the cursor switches between show/hide. DON'T COMMENT, CHANGE VALUE
#define Blink //enable blinking cursor
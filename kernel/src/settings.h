#pragma once

//LLOS Build Settings
//Comment to disable an option
//Uncomment to enable

//=========
//Debugging
//=========
//#define Debugging_Messages //enables things like displaying what's in the MADT or what drives are connected to the AHCI controller
//#define Debugging_Scheduler //shows in the serial console what task is running

//========
//Hardware
//========
#define Serial_Console //enables the serial console

//====
//Misc
//====
//#define Logging_Serial //logs in the serial console everything that's logged on the screen
#define Quiet //makes the kernel boot up process quiet

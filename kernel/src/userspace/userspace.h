#pragma once

extern "C" void UserSpaceFunc();
extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack);
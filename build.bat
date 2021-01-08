@echo off
echo [1m[97m========Compilez butlaudarul========[0m
wsl cd LLOS/gnu-efi;make -s -j8 bootloader

echo [1m[97m========Churat bildu========[0m
rd /s /q kernel\lib >NUL
md kernel\lib >NUL

echo [1m[97m========Compilez kernelu========[0m
wsl cd LLOS/kernel;make -s kernel
wsl cd LLOS/kernel/bin;strip --strip-all kernel.llexec

echo [1m[97m========Fhac imajinea========[0m
del kernel\bin\LLOS.iso >NUL
wsl cd LLOS/kernel;make -s -j8 buildimg
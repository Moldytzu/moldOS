@echo off
echo [1m[97m========Compilez butlaudarul========[0m
wsl cd LLOS/gnu-efi;make bootloader

echo [1m[97m========Churat bildu========[0m
rd /s /q kernel\lib
md kernel\lib

echo [1m[97m========Compilez kernelu========[0m
wsl cd LLOS/kernel;make kernel

echo [1m[97m========Fhac imajinea========[0m
wsl cd LLOS/kernel;make buildimg

echo [1m[97mApasa orice tasta pentru a porni QEMU![0m
pause >NUL

echo [1m[97m========Rulez hemulatorul========[0m[95m
kernel\run.bat >NUL
echo [0m
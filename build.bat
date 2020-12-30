@echo off
echo [1m[97m========Compilez butlaudarul========[0m
wsl cd LLOS/gnu-efi;make -j8 bootloader

echo [1m[97m========Churat bildu========[0m
rd /s /q kernel\lib
md kernel\lib

echo [1m[97m========Compilez kernelu========[0m
wsl cd LLOS/kernel;make kernel

echo [1m[97m========Fhac imajinea========[0m
del kernel\bin\LLOS.iso
wsl cd LLOS/kernel;make -j8 buildimg
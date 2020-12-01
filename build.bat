@echo off
echo ========Compilez butlaudarul========
wsl cd LLOS/gnu-efi;make bootloader

echo ========Churat bildu========
rd /s /q kernel\lib
md kernel\lib

echo ========Compilez kernelu========
wsl cd LLOS/kernel;make kernel

echo ========Fhac imajinea========
wsl cd LLOS/kernel;make buildimg

pause

echo ========Rulez hemulatorul========
kernel\run.bat
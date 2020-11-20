@echo off
echo ========Compilez butlaudarul========
wsl cd LLOS/gnu-efi;make bootloader

echo ========Compilez kernelu========
wsl cd LLOS/kernel;make kernel

echo ========Fhac imajinea========
wsl cd LLOS/kernel;make buildimg

pause

echo ========Rulez emulatorul========
kernel\run.bat
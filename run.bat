@echo off
cl /I"F:\SDL3-devel-3.2.16-VC\SDL3-3.2.16\include" main.c gameboy.c /link /LIBPATH:"F:\SDL3-devel-3.2.16-VC\SDL3-3.2.16\lib\x86" SDL3.lib
if %errorlevel% == 0 main.exe cpu_instrs.gb

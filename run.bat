@echo off
cl /I"SDL3-win\include" main.c gameboy.c /link /LIBPATH:"SDL3-win\lib\x86" SDL3.lib /out:a.exe
if %errorlevel% == 0 set "PATH=%PATH%;SDL3-win\lib\x86"
if %errorlevel% == 0 a.exe cpu_instrs.gb

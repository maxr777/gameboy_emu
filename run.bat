@echo off
cl /I"SDL3-win\include" main.c gameboy.c /link /LIBPATH:"SDL3-win\lib\x86" SDL3.lib /out:a.exe
if %errorlevel% == 0 (
    if not exist SDL3.dll copy "SDL3-win\lib\x86\SDL3.dll" .
    a.exe cpu_instrs.gb
)

@echo off
if not exist "build\assets\" (
    xcopy /E /I /Y "assets" "build\assets"
)

zig cc src/main.c -I./raylib/windows/include -L./raylib/windows/lib -lraylib -lgdi32 -lwinmm -Wl,--subsystem=windows -O3 -s -o ./build/game.exe
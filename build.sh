#!/bin/bash
filename=$1
rm ${filename}.exe
shift
/mnt/c/msys64/mingw64/bin/gcc.exe ${filename}.c interface.c -o ${filename}.exe -Wall -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
cmd.exe /c ${filename}.exe $1

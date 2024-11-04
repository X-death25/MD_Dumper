#!/bin/bash
# sudo apt-get install mingw-w64 g++-mingw-w64

i686-w64-mingw32-gcc main.c -I./windows_deps/include/SDL2 -I./windows_deps/include/libusb-1.0 -L./windows_deps/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lusb-1.0 -mwindows -o MD_Dumper.exe
mkdir Windows_Build
cp -dR Informations.txt ./Windows_Build/
cp *.csv ./Windows_Build/
cp MD_Dumper.exe ./Windows_Build/
cp -dR images ./Windows_Build/
cp -dR MD_Dumper_GUI.bat ./Windows_Build/
cp ./windows_deps/bin/*.dll ./Windows_Build

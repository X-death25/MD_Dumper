#!/bin/bash

clang main.c -DSDLGUI -I/opt/homebrew/include/SDL2 -I/opt/homebrew/include/libusb-1.0 -L/opt/homebrew/lib -lSDL2main -lSDL2_image -lSDL2 -lusb-1.0 -o MD_Dumper_GUI
clang main.c -I/opt/homebrew/include/libusb-1.0 -L/opt/homebrew/lib -lusb-1.0 -o MD_Dumper
mkdir -p MacOS_Build
cp Informations.txt ./MacOS_Build
cp *.csv ./MacOS_Build/
cp MD_Dumper ./MacOS_Build/
cp MD_Dumper_GUI ./MacOS_Build/
cp -R images ./MacOS_Build/

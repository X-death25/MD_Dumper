#!/bin/bash

gcc main.c -I/usr/include/SDL2 -I/usr/include/libusb-1.0 -L/usr/lib -lSDL2main -lSDL2_image -lSDL2 -lusb-1.0 -o MD_Dumper
mkdir Linux_Build
cp -dR Informations.txt ./Linux_Build
cp *.csv ./Linux_Build/
cp MD_Dumper ./Linux_Build/
cp -dR images ./Linux_Build/

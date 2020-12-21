#!/bin/bash

filename=example
vidname='Video'
viddesc='My first YouTube Video. youtu.be/zx6yKF3q4-M'

#create output folders
mkdir -p ${filename}256 ${filename}565

#convert from png to 565 (normal quality)
../../convert -p -i png/%04d.png -f 1 -s 1 -l 10 -o ${filename}565/ 
#-n "$vidname" -d "$viddesc"

#convert from gif to 256 (very low quality)
../../convert -g -i ${filename}.gif -o ${filename}256/ 
#-n "$vidname" -d "$viddesc"

echo ""
echo "Press enter to close."
read i

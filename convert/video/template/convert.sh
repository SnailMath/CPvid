#!/bin/bash

#This is a simple script to convert videos (.mp4 or any other format, any framerate, any size) to the format for CPvid. Just fill out these few lines:
#please install ffmpeg with the command `sudo apt install ffmpeg`

infile='template.mp4'	# the file you have (can be mp4 or something else, any framerate, size or aspect ratio)

filename='template'		# the name how output folder should be called
vidname='Video'			# the name displayed in CPvid
viddesc='You can change name, description and quality in ./convert.sh'		# the description that CPvid will display

quality=2	#1=low quality(106x176px), 2=medium quality(160x264px), 3=high quality(320x528px)
color=2		#1=low quality(256colors), 2=normal quality(65536colors) [3=create both]
rotate=0	#0=no rotation, 2=rotate left, (1=rotate right, note: most apps use left rotation for landscape, so don't use right.)






#########################################################







#delete old files
rm ${filename}565 -r -f
rm ${filename}256 -r -f
rm png -r -f
rm ${filename}.gif -f

#set scale
w=320 ; h=528 # this is used if a wrong option is given
if [ ${quality} = 1 ] ; then w=106 ; h=176 ; fi 	# low
if [ ${quality} = 2 ] ; then w=160 ; h=264 ; fi 	# medium
#if [ ${quality} = 3 ] ; then w=320 ; h=528 ; fi	# high

#set rotation
transp=''
if [ ${rotate} = 2 ] ; then transp='transpose=2,' ; fi #this is added to the filter option of ffmpeg
if [ ${rotate} = 1 ] ; then transp='transpose=1,' ; fi




#the actual conversion of the video.

#convert in full color
if [ ${color} != 1 ] # 2=full color
then
	#make folder for the png files
	mkdir -p png
	#convert mp4 to png
	ffmpeg -i ${infile} -vf ${transp}fps=10,scale=w=$w:h=$h:force_original_aspect_ratio=decrease png/%04d.png
	#create output folder
	mkdir -p ${filename}565
	#convert png to .565 for CPvid
	../../convert -p -i png/%04d.png -f 1 -s 1 -l $(ls png/ | wc -l | tr -d '\n') -o ${filename}565/ -n "$vidname" -d "$viddesc"
fi

#convert in gif color
if [ ${color} != 2 ] # 1=less colors (gif)
then
	# convert mp4 to gif (.gif format reduces colors)
	ffmpeg -i ${infile} -vf ${transp}fps=10,scale=w=$w:h=$h:force_original_aspect_ratio=decrease ${filename}.gif	
	#create output folder
	mkdir -p ${filename}256
	#convert .gif to .256 for CPvid
	../../convert -g -i ${filename}.gif -o ${filename}256/ -n "$vidname" -d "$viddesc"
fi




echo ""
echo "Press enter to close."
read i

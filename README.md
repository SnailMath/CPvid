# CPvid
A simple video player for the Classpad II 

(This requires [hollyhock-2](https://github.com/SnailMath/hollyhock-2) to be installed on the calculator.)

The program CPvid.hhk plays back videos on the Classpad 2. The videos need to be in a specific format in folders called vid0 to vid9.
There is an example video in the folder vid0.

To convert videos, first use a program like blender to convert them to png sequences.
I recommend scaling down the video to 160px wide. The videos cannot be larger than 160x264 but I recommend 160x160.
place the png sequence in the folder vid_source. Change the number of the last image in convert.c
The calculator plays at 10fps, so when your video was 30fps leave the value STEP at 3, so only every third frame gets converted, making the video 10fps.
After `make` and `convert` there should be a lot of files with the extension .565 in the folder vid0. These are all the frames. There will be a file called video.txt,
this contains info about the video. Just copy the whole folder to the calculator's flash storage. 

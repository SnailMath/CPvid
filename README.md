# CPvid
A simple video player for the Classpad II 

(This requires [hollyhock-2](https://github.com/SnailMath/hollyhock-2) to be installed on the calculator.)

This was inspired by the hollyhock projeckt from The6P4C.

## What is CPvid?
The program CPvid.hhk plays back videos on the Classpad 2. The videos need to be in a specific format.
There are two formats available. The 565-format and the 256-format. In the 565 format there are all 65536 colors available. In the 256-format, only 256 colors are used. The 256-format uses half as much storage than the 565-format.
See 'converting videos' for a turorial on how to convert your own videos to these formats.

## Playing videos
- You need to have hollyhock-2 installed (or hollyhock).
- place CPvid.hhk on the storage of the calculator. (run `make` in the folder CPvid)
- Convert a video or download the example (example video in convert/video/example/example256 )
- Place the folder with the files of the video onto the root of the calculator. (In case of the example eiter `example256` or `example565`)
- When the file transfer is finished, 
disconnect the calculator from the pc and open hollyhock 
(On the calc, go to "System", click "System" on the top left, select "Hollyhock-2 Laucher"
- Select "Videoplayer" in the dropdown and click "Run"
- Select your video in the dropdown and click "Play"
- When the video finishes, press `Clear` to close it

(You can download a longer example [here in the folder 'stuff'](https://drive.google.com/drive/u/0/folders/1T42uvVZ3Ps-bAghsFQeWIDF85kpocLed)

## converting videos
- convert video to png sequence (Use blender for example. If your video was 30fps select step=3, so the exported video will have 10fps.
Downscale it so it is 160px wide. The screen of the calc is 320x528, but videos up to 160x264 are scaled up by 2)
- place the png sequence in the folder convert/video/tamplate/png
- if you want the smaller filesize, convert the image sequence to a gif animation (using gimp, open the first png file, click on `file->open` as layers and select everything else, click import, go to `file->export as` and use `template.gif` as the filename, click ok, select animated gif, click ok)
- run convert.sh in the folder convert/video/template (before that, you need to `make` in the folder convert )
- you have the folder template565 and template256 with the video in 2 different qualities, 565 is better image quality, 256 is smaller filesize.
- see `playing videos` on how to play the video.

(you can change the name in convert.sh to change the name of the output folder and input .gif file, if 'template' is not a good name for your video.)

## No warranty!

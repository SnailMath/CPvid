# CPvid
A simple video player for the Classpad II 

_Click [here](https://github.com/SnailMath/CPvid/releases) to download the program. Place everything from inside the .zip file onto the flash of the calculator._


(This requires [hollyhock-2](https://github.com/SnailMath/hollyhock-2) to be installed on the calculator, at least version 1.0.2)

This was inspired by the hollyhock projeckt from The6P4C.

## What is CPvid?
The program CPvid.hhk plays back videos on the Classpad 2. The videos need to be in a specific format.
There are two formats available. The 565-format and the 256-format. In the 565 format there are all 65536 colors available. In the 256-format, only 256 colors are used. The 256-format uses half as much storage than the 565-format.
See 'converting videos' for a turorial on how to convert your own videos to these formats.

## Playing videos
- You need to have hollyhock-2 installed (or hollyhock).
- place CPvid.hhk on the storage of the calculator. (run `make` in the folder CPvid or download the released file from the .zip file [here](https://github.com/SnailMath/CPvid/releases))
- Convert a video or use the example from the .zip file
- Place the folder with the files of the video onto the root of the calculator. (In case of the example eiter `example256` or `example565`)
- When the file transfer is finished, disconnect the calculator from the pc (click eject first) and open hollyhock 
(On the calc, go to "System", click "System" on the top left, select "Hollyhock-2 Laucher"
- Select "Videoplayer" in the dropdown and click "Run"
- Select your video in the dropdown and click "Play"
- Press `EXE` to pause the video, left and right arrow to wind forward and backward, use Backspace to show debug info.
- When the video finishes, the last frame stays on screen. Press `Clear` to close it or `EXE` to play it again.

(You can download a longer example [here in the folder 'stuff'](https://drive.google.com/drive/u/0/folders/1T42uvVZ3Ps-bAghsFQeWIDF85kpocLed), passwd: `123456`)

## converting videos
- download [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) into the folder convert/ (click on raw, right click, save as)
- run `make` in the folder convert/ 
- You need the video you want in any file format, for example in .mp4 format.
- place the video in the folder convert/video/tamplate/ (copy and rename this folder for multiple videos)
- open convert.sh in a text editor and change the filename in the line `infile=`
- you can also change the other six options in the following lines
- run `./convert.sh` in the folder convert/video/template (or how you have renamed it)
- you have the folder template565 and/or template256 (depending on what you set as `color=` in convert.sh) 
- (To use sound, convert your audio to 11025 samples/sec 8bit .wav and place the file audio.wav into that folder.)
- see `playing videos` on how to play the video.

##Now with audio!
- I had the idea to add audio. I know that this idea was stupid. Use an 8bit 11025 samples/second .wav file called `audio.wav`. 
- Video (10fps) is synced to wait for 1102.5 frames to be elapsed, or skip frames if video is lagging behind.
- The fastest serial speed is 11520bps, but I change the SCBRR register from 7 to 0 to make it 8 times faster (922kbit/s).
- 8 bytes are one sample, so we have 64 bits for pwm. So 6 bit audio is used (the last 2 bit of 8bit .wav are ignored).
- If this 6bit sample value is 0, we send no ones and 64 zeroes, if the sample value is 63, we send 63 ones and one zero.
- connect a small capacitor to the serial out to remove DC and connect that to an amplifier. (ON YOUR OWN RISK)
- If no audio file is there, no audio is played.
- The debug info (press backspace) shows the frame number and the time in the audio and the video in 1/11025th of a second (aka number of samples).

## No warranty!

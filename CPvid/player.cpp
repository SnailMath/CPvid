#include "player.hpp"
#include "myGetKey.hpp"
#include "audio.hpp"

namespace Player{
uint16_t *vram;
int width, height;

void loadvideo(Video::VideoInfo *video) {
	LCD_VRAMBackup();
	Debug_Printf(0,0,false,0,"Playback Error. Press Clear.");

	vram = LCD_GetVRAMAddress();
	LCD_GetSize(&width, &height);

	bool DEBUG = false;

	//initialize the audio
	bool hasAudio = Audio::init(video->folder);
	
	//Load basic info about video (size, mode, scale)
	uint16_t vidw = *((uint16_t*)(video->w));
	uint16_t vidh = *((uint16_t*)(video->h)); 
    
	uint16_t mode = *((uint16_t*)(video->mode)); 
	int      m256 = mode==256;
	#define speed 1102   //The audio has 11025 samples per second aka 1102.5 samples per frame, that means each frame takes 1102.5 samples.
	#define frameToTime(frame) ((frame*2205)>>1) //calculate the current time (in samples) from the current frame number

	//Check if the video needs to be scaled up
	uint8_t s = 3;					//Assume: scale by 3
	if((vidw*3>width) || (vidh*3>height)) s=2;	//If this is to big, make it 2
	if((vidw*2>width) || (vidh*2>height)) s=1;	//If this is still to big, make it 1

	uint16_t offx = (width -(vidw*s))>>1; //the free space on the left to center the image
	uint16_t offy = (height-(vidh*s))>>1; //the free space on the top  to center the image

	uint8_t data[vidw*vidh*(m256?1:2)]; //The array for the image data
	uint8_t *image = (uint8_t*)vram + ((offx+(offy*width))*2); //Pointer to the top left corner of the image

	uint16_t palette[256];
    
	if(m256){
		char palname[64];
		strcpy(palname, video->folder);
		strcat(palname, "\\palette.bin");
		int fd = open(palname,OPEN_READ);
		read(fd, (void*)palette, 256*2);
		close(fd);  
	}

	char filename[64];	//The name of the image file
	int countdigit = 0;	//The number of the first of the four digits for counting up
	strcpy(filename,video->folder);
	while(filename[countdigit]!=0) countdigit++; //increase 'countdigit' so it points after the name of the folder
    	if(m256){
		strcat(filename,"\\0000.256");
    	}else{
		strcat(filename,"\\0000.565");
    	}
	countdigit++; //countdigit now points to the '0' after the '\' 

	uint32_t key1, key2;
	uint32_t old1 = 0;
	uint32_t old2 = 0;

	uint8_t status = PLAY;
	int frame = 0; //set number of the current frame to 0
	bool screenchanged = true; //make sure this gets actually displayed

	Audio::pos(0); //go to start of audio
	Audio::play(); //start the playback

	blackScreen();
	while(true){
		//only access files, if we need to...
		if(screenchanged){
			//convert binary to decimal
			int32_t dec0001 = frame;	//store the    1's
			int32_t dec0010 = 0;		//store the   10's
			int32_t dec0100 = 0;		//store the  100's
			int32_t dec1000 = 0;		//store the 1000's
			while(dec0001>=1000){dec0001-=1000;dec1000++;}
			while(dec0001>= 100){dec0001-= 100;dec0100++;}
			while(dec0001>=  10){dec0001-=  10;dec0010++;}
			//build the filename
			filename[countdigit+0]=dec1000+'0';
			filename[countdigit+1]=dec0100+'0';
			filename[countdigit+2]=dec0010+'0';
			filename[countdigit+3]=dec0001+'0';
			//Open the file
			int fd = open(filename,OPEN_READ);
			if (fd <= 0){
				status=PAUSE;
				if(hasAudio)Audio::pause();
			} else {
				//File opened successfull
				read(fd, (void*)data, (vidw*vidh*(m256?1:2))); //Read the whole frame
				close(fd);
			}

			//display the image
			int iy = 0;
			while (iy<vidh){
				int ix = 0;
				while (ix<vidw){
					uint16_t color;
					if (m256){
						color = palette[data[ix+(iy*vidw)]];
					}else{
						color = *((uint16_t*)( data + ((ix+(iy*vidw))*2) ));
					}
					*((uint16_t*)( image + ( ((ix*s)+0) + (((iy*s)+0)*width) )*2 )) = color;
					if (s>=2) {
						*((uint16_t*)( image + ( ((ix*s)+1) + (((iy*s)+0)*width) )*2 )) = color;
						*((uint16_t*)( image + ( ((ix*s)+0) + (((iy*s)+1)*width) )*2 )) = color;
						*((uint16_t*)( image + ( ((ix*s)+1) + (((iy*s)+1)*width) )*2 )) = color;
						if (s==3) {
							*((uint16_t*)( image + ( ((ix*s)+2) + (((iy*s)+0)*width) )*2 )) = color;
							*((uint16_t*)( image + ( ((ix*s)+2) + (((iy*s)+1)*width) )*2 )) = color;
							*((uint16_t*)( image + ( ((ix*s)+2) + (((iy*s)+2)*width) )*2 )) = color;
							*((uint16_t*)( image + ( ((ix*s)+1) + (((iy*s)+2)*width) )*2 )) = color;
							*((uint16_t*)( image + ( ((ix*s)+0) + (((iy*s)+2)*width) )*2 )) = color;
						}//if(s==3)
					}//if(s>2)
					ix++;
				}//while(ix<vidw)
				iy++;
			}//while(iy<vidh)
		screenchanged=false; //the content of the vram matches now what it is supposed to.
		//LCD_Refresh(); //do that further down...
		}//if(screenchanged)

		if(DEBUG){
			Debug_Printf(0,0,true,0,"%04d", frame);
			Debug_Printf(0,1,true,0,status==PLAY?"PLAY ":"PAUSE");
			Debug_Printf(6,0,true,0,"Audio %8d",Audio::getTime());
			Debug_Printf(6,1,true,0,"Video %8d",frameToTime(frame)); //because the speed is something point 5... 
		}

		LCD_Refresh();

		//Read the keyboard
		old1 = key1; old2 = key2;
		myGetKey(&key1, &key2);

		if(old1!=key1 || old2!=key2){ //the keys have changed
			//Process keypresses
			if(testKey(key1,key2,KEY_CLEAR)){//clear
				break;
			}
			if(testKey(key1,key2,KEY_EXE)){//EXE
				if(status==PLAY){//toggle play/pause
					status = PAUSE; 
					if(hasAudio)Audio::pause();
				}else{
					status = PLAY;
					if(hasAudio)Audio::play();
				}
			}
			if(testKey(key1,key2,KEY_LEFT)){//LEFT
				if(status==PLAY){
					frame-=10;
				}else{
					frame-=1;
					screenchanged=true; //make sure the frame on the screen will actually get changed, even tho we are on pause
				}
			}
			if(testKey(key1,key2,KEY_RIGHT)){//RIGHT
				if(status==PLAY){
					frame+=10;
				}else{
					frame+=1;
					screenchanged=true; //make sure the frame on the screen will actually get changed, even tho we are on pause
				}
			}
			if(testKey(key1,key2,KEY_UP)){ //UP skips forward
				frame+=100;
				screenchanged=true; //make sure the frame on the screen will actually get changed, even tho we are on pause
			}
			if(testKey(key1,key2,KEY_DOWN)){ //DOWN skips backwards
				frame-=100;
				screenchanged=true; //make sure the frame on the screen will actually get changed, even tho we are on pause
			}
			if(testKey(key1,key2,KEY_BACKSPACE)){ //BACKSPACE toggles DEBUG
				DEBUG=!DEBUG;
				int i = 0; while (i<(width*12*2)) vram[i++]=0; //clear the top 2 lines
			}
			if(frame<0)frame=0;
			if(hasAudio)Audio::pos(frameToTime(frame));
		}

		if(status==PLAY){ //when playing go to the next frame
			frame++;
			screenchanged=true; //make sure the frame on the screen will actually get changed
		}

		//sync video and audio
		if(hasAudio){
			int32_t audiotime = Audio::getTime();
			int32_t videotime = frameToTime(frame); //calculate the time the (next) frame should be displayed at
			while(audiotime>videotime+speed){ //if the audio is ahead, skip frames
				frame++;
				audiotime = Audio::getTime();
				videotime = frameToTime(frame); //calculate the time the (next) frame should be displayed at
			}
			while(audiotime<videotime-speed){ //if the video is ahead, wait
				audiotime = Audio::getTime();
				videotime = frameToTime(frame); //calculate the time the (next) frame should be displayed at
			}
		}//if(hasAudio)
	}//while true

	if(hasAudio)
		Audio::stop();

	LCD_VRAMRestore();
	LCD_Refresh();
}

void blackScreen(){
	int i = 0;
	while (i<(width*height)){
		vram[i++]=0;
	}
}


}

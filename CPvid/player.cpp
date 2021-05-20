#include "player.hpp"
#include "myGetKey.hpp"

//countUp(str,nr,step) adds 'step' to the decimal 4 digit number in the string str and calls count to do the carrying.
//step can be positive or negative.
//1000s place: str[nr+0]
// 100s place: str[nr+1]
//  10s place: str[nr+2]
//   1s place: str[nr+3]
#define countUp(str,nr, step) str[nr+3]+=step; count(str,nr+3) count(str,nr+2) count(str,nr+1) 
//count(str,nr) does the carrying for countUp(). Also works for subtraction.
#define count(str,nr) if(str[nr]>'9'){str[nr]-=10; str[nr-1]++;} if(str[nr]<'0'){str[nr]+=10; str[nr-1]--;}

namespace Player{
uint16_t *vram;
int width, height;
int playSpeed = 1; //1 is fastest, 10 is slowest

void loadvideo(Video::VideoInfo *video) {
	LCD_VRAMBackup();
    Debug_Printf(0,0,false,0,"Playback Error. Press Clear.");

	vram = LCD_GetVRAMAddress();
	LCD_GetSize(&width, &height);
	
	//Load basic info about video (size, mode, scale)
	uint16_t vidw = *((uint16_t*)(video->w));
    uint16_t vidh = *((uint16_t*)(video->h)); 
    
    uint16_t mode = *((uint16_t*)(video->mode)); 
    int      m256 = mode==256;
    
	uint8_t s = 3;
	if((vidw*3>width) || (vidh*3>height)) s=2;
	if((vidw*2>width) || (vidh*2>height)) s=1;

    uint16_t offx = (width -(vidw*s))>>1; //free space on the left to center the image
    uint16_t offy = (height-(vidh*s))>>1; //free space on the top  to center the image
    
    
    uint8_t data[vidw*vidh*(m256?1:2)];
	uint8_t *image = (uint8_t*)vram + ((offx+(offy*width))*2);

    uint16_t palette[256];
    
    if(m256){
        char palname[64];
        strcpy(palname, video->folder);
        strcat(palname, "\\palette.bin");
        int fd = open(palname,OPEN_READ);
        read(fd, (void*)palette, 256*2);
        close(fd);  
    }

	char filename[64]; //The name of the first file
	int countdigit = 0; //The number of the first of the four digits for counting up

	uint32_t key1, key2;
	int keychange;
	uint32_t old1 = 0;
	uint32_t old2 = 0;
	uint8_t status = START;
	blackScreen();
	while(1==1){
		//Clear the screen
		//blackScreen();

		//Read the keyboard
		old1 = key1; old2 = key2;
		myGetKey(&key1, &key2);

		if(old1!=key1 || old2!=key2)
			keychange = 1;
		else
			keychange = 0;

		//DEBUG
		//Debug_PrintNumberHex_Dword(key1,0,0);
		//Debug_PrintNumberHex_Dword(key2,0,1);

		//Process keypresses
		if (keychange){
			if(testKey(key1,key2,KEY_CLEAR)){
				break;
			}
			if(testKey(key1,key2,KEY_EXE)){
				if(status==PLAY){
					status=PAUSE2;
				}else if (status==PAUSE){
					status=PLAY;
				}else if(status==STOP){ //if status==STOP
					status=START;
				}
			}
			if(testKey(key1,key2,KEY_LEFT)){
				if (status==PLAY)
					status=BACKWARD;
				else if (status==PAUSE)
					status=BACK;
			}
			if(testKey(key1,key2,KEY_RIGHT)){
				if (status==PLAY)
					status=FORWARD;
				else if (status==PAUSE)
					status=STEP;
			}
			if(testKey(key1,key2,KEY_UP)){
				if (playSpeed>1)
					playSpeed--;
			}
			if(testKey(key1,key2,KEY_DOWN)){
				if (playSpeed<10)
					playSpeed++;
			}
		}
	
		//If the video should start from the beginning
		if(status==START){
			//construct the filename
			strcpy(filename,video->folder);
			strcat(filename,"\\");
			countdigit = 0; //The first character of the file path after IMG_ , this is the first of the four digits for counting up
			while(filename[countdigit]!=0) countdigit++;
    		if(m256){
    			strcat(filename,"0000.256");
    		}else{
			    strcat(filename,"0000.565");
    		}
			status=PLAY;
		}else if(status==PLAY){
			countUp(filename,countdigit,1)
		}else if (status==STEP){
			countUp(filename,countdigit,1)
			status=PAUSE2;
		}else if (status==BACK){
			countUp(filename,countdigit,-1)
			status=PAUSE2;
		}else if (status==FORWARD){
			countUp(filename,countdigit,10)
			status=PLAY;
		}else if (status==BACKWARD){
			countUp(filename,countdigit,-10)
			status=PLAY;
		}

		//Open the file
		int fd;
		if (status==PLAY || status==PAUSE2) {
			fd = open(filename,OPEN_READ);
			if (fd <= 0){
				//Error while opening the file
				status = STOP;
				//break;
			} else {
				//File opened successfull
				read(fd, (void*)data, (vidw*vidh*(m256?1:2))); //Read the whole frame
				close(fd);
			}
		}

		//display the image
		if (status==PLAY || status==PAUSE2) {
			if (status==PAUSE2){
				status=PAUSE;
			}
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
					 }
					}
					ix++;
				}
				iy++;
			}
				
		}

		//Refresh the screen
		for(int i=0; i<playSpeed;i++)
			LCD_Refresh();
	}

	//LCD_Refresh();
	//Debug_WaitKey();
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

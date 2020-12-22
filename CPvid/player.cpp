#include "player.hpp"


#define countUp(str,nr) str[nr+3]++; count(str,nr+3) count(str,nr+2) count(str,nr+1)
#define count(str,nr) if(str[nr]>'9'){str[nr]-=10; str[nr-1]++;}

namespace Player{
uint16_t *vram;
int width, height;

void loadvideo(Video::VideoInfo *video) {
	LCD_VRAMBackup();
    Debug_Printf(0,0,false,0,"Playback Error. Press Clear.");

	vram = LCD_GetVRAMAddress();
	LCD_GetSize(&width, &height);
	
	uint16_t vidw = *((uint16_t*)(video->w));
    uint16_t vidh = *((uint16_t*)(video->h)); 
    
    uint16_t mode = *((uint16_t*)(video->mode)); 
    int      m256 = mode==256;
    
	uint8_t s = 2;
	if((vidw*2>width) || (vidh*2>height)) s=1;

    uint16_t offx = (width -(vidw*s))>>1; //free space on the left to center the image
    uint16_t offy = (height-(vidh*s))>>1; //free space on the top  to center the image
    
	char filename[64]; //The name of the first file
	strcpy(filename,video->folder);
	strcat(filename,"\\");
	int countdigit = 0; //The first character of the file path after IMG_ , this is the first of the four digits for counting up
	while(filename[countdigit]!=0) countdigit++;
    if(m256){
    	strcat(filename,"0000.256");
    }else{
	    strcat(filename,"0000.565");
    }
    
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

	while(1==1){
		int fd = open(filename,OPEN_READ);
		if (fd <= 0){
			break;
		}
		blackScreen();
	
		int count = read(fd, (void*)data, (vidw*vidh*(m256?1:2))); //Read the whole frame
        //getAddr(fd, 0, (const void**)(&data));

		close(fd);

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
				if (s==2) {
				*((uint16_t*)( image + ( ((ix*2)+1) + (((iy*2)+0)*width) )*2 )) = color;
				*((uint16_t*)( image + ( ((ix*2)+0) + (((iy*2)+1)*width) )*2 )) = color;
				*((uint16_t*)( image + ( ((ix*2)+1) + (((iy*2)+1)*width) )*2 )) = color;
				}
	
				ix++;
			}
			iy++;
		}


		countUp(filename,countdigit)

		LCD_Refresh();
	}

	LCD_Refresh();
	Debug_WaitKey();
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
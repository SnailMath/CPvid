#include <appdef.hpp>
#include <sdk/os/file.hpp>
#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/lcd.hpp>
#include <sdk/os/debug.hpp>
#include <sdk/os/gui.hpp>

#include "video.hpp"

//#define vidw 160
//#define vidh  90

#define PIXEL(x, y) (vram[(x) + (y) * width])

#define count(str,nr) if(str[nr]>'9'){str[nr]-=10; str[nr-1]++;}
#define countUp(str,nr) str[nr+3]++; count(str,nr+3) count(str,nr+2) count(str,nr+1)

APP_NAME("Videoplayer")
APP_DESCRIPTION("A program to play videos on your calculator.")
APP_AUTHOR("SnailMath")
APP_VERSION("0.0.0")

void blackScreen();
void loadvideo(Video::VideoInfo *video);

uint16_t *vram;
int width, height;


//int GlobalSelectedVideo; 

class Select : public GUIDialog {
    public:
        //This var is public
        int selectedVideo; 
        //The constructor
        Select() : GUIDialog( 
            GUIDialog::Height95, GUIDialog::AlignTop, "SnailMath Videoplayer", GUIDialog::KeyboardStateNone ), 
    	    vidNames( GetLeftX()+10, GetTopY()+10, GetRightX()-10, GetBottomY()-10, VID_NAMES_EVENT_ID),
    	    vidInfo( GetLeftX()+10, GetTopY()+90, GetRightX()-10, GetBottomY()-10, "No Videos where found. Use the program from github.com/SnailMath/CPvid to vonvert image sequences from .png to .565 format and place the films in the folders \"vid0/\" to \"vid9\" on the flash." ),
    	    play( GetLeftX()+10, GetTopY()+45, GetLeftX()+10+100, GetTopY()+45+35, "Play", PLAY_EVENT_ID ),
    	    close( GetRightX()-10-100, GetTopY()+45, GetRightX()-10, GetTopY()+45+35, "Close", CLOSE_EVENT_ID )
    	{
            selectedVideo = 0;
            //GlobalSelectedVideo = selectedVideo; 
            Video::LoadVideoInfo();

            //Add videos to dropdown/

            int i = 0;
            while (i<Video::numVideos){
                struct Video::VideoInfo *vid = &Video::videos[i];
			    const char *name = vid->name;
			    vidNames.AddMenuItem(*(new GUIDropDownMenuItem(name, i+1, GUIDropDownMenuItem::FlagEnabled | GUIDropDownMenuItem::FlagTextAlignLeft)));
                i++;
            }

            vidNames.SetScrollBarVisibility(GUIDropDownMenu::ScrollBarVisibleWhenRequired);
            AddElement(vidNames);
            AddElement(vidInfo);

            if (Video::numVideos>0){
                AddElement(play);
            }
            AddElement(close);

            UpdateVideoInfo();
	    }

        virtual int OnEvent(GUIDialog_Wrapped *dialog, GUIDialog_OnEvent_Data *event){
            int eventID = event->GetEventID();
            if (eventID==VID_NAMES_EVENT_ID&&(event->type&0xF)==0xD){
                selectedVideo = event->data-1;
                //GlobalSelectedVideo = selectedVideo; 
                UpdateVideoInfo();
                return 0;
            }
            if (eventID==PLAY_EVENT_ID){
                if (selectedVideo < Video::numVideos){
                    struct Video::VideoInfo *vid = &Video::videos[selectedVideo];
                    loadvideo(vid);
                    return 0;
                }
                
            }
            return GUIDialog::OnEvent(dialog, event);
        }


        void UpdateVideoInfo(){
            if (selectedVideo >= Video::numVideos) return;
            struct Video::VideoInfo *vid = &Video::videos[selectedVideo];
            memcpy(vidInfoString, vid->description, sizeof(vidInfoString));
            vidInfo.SetText(vidInfoString);
            vidInfo.Refresh();
            Refresh();
        }


    private:
        const uint16_t VID_NAMES_EVENT_ID = 1;
        GUIDropDownMenu vidNames;
        GUILongLabel vidInfo;
        char vidInfoString[500];
        const uint16_t PLAY_EVENT_ID = DialogResultOK;
        GUIButton play;
        const uint16_t CLOSE_EVENT_ID = DialogResultCancel;
        GUIButton close;
};

void main(){
  //while (true){
	Select select;
	select.ShowDialog();
	//if (select.ShowDialog()==GUIDialog::DialogResultOK){
    //	if (select.selectedVideo < Video::numVideos){
    //        struct Video::VideoInfo *vid = &Video::videos[select.selectedVideo];
	//        loadvideo(vid);
	//    }
	//}
  //}
}


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

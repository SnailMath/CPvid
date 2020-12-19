#include <appdef.hpp>
#include <sdk/os/file.hpp>
#include <sdk/os/mem.hpp>
#include <sdk/os/lcd.hpp>
#include <sdk/os/debug.hpp>
#include <sdk/os/gui.hpp>

#include "video.hpp"

#define vidw 160
#define vidh  90

#define PIXEL(x, y) (vram[(x) + (y) * width])

#define count(str,nr) if(str[nr]>'9'){str[nr]-=10; str[nr-1]++;}
#define countUp(str,nr) str[nr+3]++; count(str,nr+3) count(str,nr+2) count(str,nr+1)

APP_NAME("Videoplayer")
APP_DESCRIPTION("A program to play videos on your calculator.")
APP_AUTHOR("SnailMath")
APP_VERSION("0.0.0")

void blackScreen();

uint16_t *vram;
int width, height;

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
            if (event->GetEventID()==VID_NAMES_EVENT_ID&&(event->type&0xF)==0xD){
                selectedVideo = event->data-1;
                UpdateVideoInfo();
                return 0;
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
	Select select;
	if (select.ShowDialog()==GUIDialog::DialogResultOK){
	}
}











/*
//extern "C"
void oldmain() {
	LCD_VRAMBackup();

	vram = LCD_GetVRAMAddress();
	LCD_GetSize(&width, &height);
    
	char filename[]="\\fls0\\mc\\IMG_0000.565"; //The name of the first file
	#define filenameno 13 //The first digit of the 4-digit number (15-18 are the digits.)

	while(1==1){
		int fd = open(filename,OPEN_READ);
		blackScreen();
		//Debug_Printf(0,40, true, 0, "File \"%s\" opened, fd is: %d ",filename,fd);
		if (fd <= 0){
			break;
		}
	
		uint8_t *image = (uint8_t*)vram + (((528-(vidh*2))/2)*320*2); //leafe 104 lines free at the top and bottom
		uint8_t *data  = image + (vidh*960); //153600 ; //This is 3/4 through the image. The last byte of the data should be the last byte of the image. (the data is 1/4 the size of the image.)
		int count = read(fd, (void*)data, (vidw*vidh*2)); //Read the whole frame
		//Debug_Printf(0,41, true, 0, "Read %d bytes.", count);

		close(fd);

		//resize the image
		int iy = 0;
		while (iy<=vidh){
			int ix = 0;
			while (ix<=vidw){

				uint16_t color = *((uint16_t*)( data + ((ix+(iy*160))*2) ));
				
				*((uint16_t*)( image + ( ((ix*2)+0) + (((iy*2)+0)*320) )*2 )) = color;
				*((uint16_t*)( image + ( ((ix*2)+1) + (((iy*2)+0)*320) )*2 )) = color;
				*((uint16_t*)( image + ( ((ix*2)+0) + (((iy*2)+1)*320) )*2 )) = color;
				*((uint16_t*)( image + ( ((ix*2)+1) + (((iy*2)+1)*320) )*2 )) = color;
	
				ix++;
			}
			iy++;
		}


		countUp(filename,filenameno)
	
	
		LCD_Refresh();
	}
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
*/
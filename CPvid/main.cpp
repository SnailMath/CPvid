#include <appdef.hpp>
#include <sdk/os/file.hpp>
#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/lcd.hpp>
#include <sdk/os/debug.hpp>
#include <sdk/os/gui.hpp>

#include "player.hpp"
#include "video.hpp"


//#define vidw 160
//#define vidh  90

#define PIXEL(x, y) (vram[(x) + (y) * width])


APP_NAME("Videoplayer")
APP_DESCRIPTION("A program to play videos on your calculator.")
APP_AUTHOR("SnailMath")
APP_VERSION("0.0.0")


class Select : public GUIDialog {
    public:
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
            int eventID = event->GetEventID();
            if (eventID==VID_NAMES_EVENT_ID&&(event->type&0xF)==0xD){
                selectedVideo = event->data-1;
                UpdateVideoInfo();
                return 0;
            }
            if (eventID==PLAY_EVENT_ID){
                if (selectedVideo < Video::numVideos){
                    struct Video::VideoInfo *vid = &Video::videos[selectedVideo];
                    Player::loadvideo(vid);
                    return 0;
                }
                
            }
            return GUIDialog::OnEvent(dialog, event);
        }


        void UpdateVideoInfo(){
            if (selectedVideo >= Video::numVideos) return;
            struct Video::VideoInfo *vid = &Video::videos[selectedVideo];
			memset(vidInfoString, 0, sizeof(vidInfoString));
            strcat(vidInfoString, vid->description);
            strcat(vidInfoString, "\nFolder: ");
            strcat(vidInfoString, vid->folder);
            vidInfo.SetText(vidInfoString);
            vidInfo.Refresh();
            Refresh();
        }


    private:
        const uint16_t VID_NAMES_EVENT_ID = 1;
        GUIDropDownMenu vidNames;
        GUILongLabel vidInfo;
        char vidInfoString[199+9+63+1]; // video.description + "\nFolder: " + video.folder + "\0"
        const uint16_t PLAY_EVENT_ID = DialogResultOK;
        GUIButton play;
        const uint16_t CLOSE_EVENT_ID = DialogResultCancel;
        GUIButton close;
};

void main(){
	Select select;
	select.ShowDialog();
}



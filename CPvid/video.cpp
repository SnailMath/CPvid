#include "video.hpp"

#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/file.hpp>

namespace Video{
    struct VideoInfo videos[MAX_VIDEOS];
    int numVideos;
    void LoadVideoInfo(){
        numVideos = 0;
        char folder[64]; //The name of the folder of the video
        char filename[64+10]; //The name of the video info file
        wchar_t Lfoundname[64]; //The name of the file found by 'find'. This is a wchar_t (16bit pert character)
        char     foundname[64];
        wchar_t Lpathname[]=L"\\fls0\\*"; //The name of the file found by 'find'. This is a wchar_t (16bit pert character)
        int findHandle;
	
        //Start the find
        struct findInfo bufFindInfo;
        if ( findFirst( Lpathname, &findHandle, Lfoundname, &bufFindInfo)>=0 ){
            do{
                memset(folder,   0, 12);
                memset(filename, 0, 21);
                
                //Convert the filename from wide char to normal char
                int i = 0;
                while (i<64){ 
                    const wchar_t c = Lfoundname[i];
                    foundname[i] = c;
                    if (c==0x0000) break;
                    i++;
                }
                
                strcat(folder, "\\fls0\\"); // "\fls0\"
                strcat(folder, foundname);  // "\fls0\vid0"
                
                strcat(filename, folder);       // "\fls0\vid0"
                strcat(filename,"\\video.txt"); // "\fls0\vid0\video.txt"
                
                int fd = open(filename,OPEN_READ);
            
                if (fd>0){ //Video gefunden*/
                    struct VideoInfo video;             //Create new VidInfo
                    memset(&video, 0, sizeof(video));   //Fill it with 0s
                    
                    //video.name[0] = fd>=0?'1':'0';
                    
                    //strcat(video.name, filename);
                    
                    if (VideoInfoParse(fd, &video)){     //Load info from file
                        strcat (video.folder, folder);
                        strcat(video.description,"\nFrom file ");
                        strcat(video.description,filename);
                        if(*((uint16_t*)video.mode)==256)
                            strcat(video.description,"\nmode=256");
                        videos[numVideos++]=video;
                    }
                    
                }
            
                close(fd);
                i++;
            }while( (findNext( findHandle, Lfoundname, &bufFindInfo)>=0) && (numVideos<MAX_VIDEOS) );
        }//endif
        //Close the find handle.
        findClose(findHandle);  
    }
    bool VideoInfoParse(int fd, VideoInfo *video){
        char *info;
        getAddr(fd, 0, (const void**)(&info));
        //Copy the Video name
        
        
        unsigned int i = 0;
        while((*info!='\r')&&(*info!='\n')&&(*info!='\0')&&i<sizeof(video->name)-1){
            video->name[i++]=*info++;
        }
        if(i==0) return false;
        video->name[i]=0; //Terminate the name
        while((*info=='\r')||(*info=='\n')) info++;

        //Parse the video dimensions
        uint16_t w = 0;
        while(*info >= '0' && *info <= '9')
            w = (w*10) + (*info++ - '0');
        if (w==0) return false;
        *((uint16_t*)(video->w)) = w;
        if (*info++ != 'x') return false;

        uint16_t h = 0;
        while(*info >= '0' && *info <= '9')
            h = (h*10) + (*info++ - '0');
        if (h==0) return false;
        *((uint16_t*)(video->h)) = h;

	//Parse the mode (either 565(color in 565) or 256(256 different colors)
        while((*info=='\r')||(*info=='\n')) info++;
        uint16_t mode = 0;
        while(*info >= '0' && *info <= '9')
            mode = (mode*10) + (*info++ - '0');
        //if (mode==0) return false;
        *((uint16_t*)(video->mode)) = mode;
        
        while((*info=='\r')||(*info=='\n')) info++;

        i=0; 
        while (i<sizeof(video->description)-1){
            video->description[i]=*info;
            if (*info==0) break;
            info++;
            i++;
        }              
        return true;
    }
}

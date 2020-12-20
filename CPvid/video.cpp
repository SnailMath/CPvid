#include "video.hpp"

#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/file.hpp>

namespace Video{
    struct VideoInfo videos[MAX_VIDEOS];
    int numVideos;
    void LoadVideoInfo(){
        numVideos = 0;
        char folder[12]; //The name of the folder of the video
        char filename[21]; //The name of the video info file
        char nr[2]; nr[1]=0; //The number of the current folder (vid0 - vid9)
        unsigned char i = 0;
        while (i<10) { //Test all 10 folders from vid0 to vid9
            memset(folder,   0, 12);
            memset(filename, 0, 21);
            strcat(folder,"\\fls0\\vid");
            nr[0]='0'+i;
            strcat(folder,nr); //append the number after the "vid"
            strcat(filename, folder);
            strcat(filename,"\\video.txt");
            int fd = open(filename,OPEN_READ);
            
            if (fd>0){ //Video gefunden
                struct VideoInfo video;             //Create new VidInfo
                memset(&video, 0, sizeof(video));   //Fill it with 0s
                if (VideoInfoParse(fd, &video)){     //Load info from file
                    strcat (video.folder, folder);
                    strcat(video.description,"\nFrom file ");
                    strcat(video.description,filename);
                    videos[numVideos++]=video;
                }
            }
            
            close(fd);
            i++;
        }
    
    }
    bool VideoInfoParse(int fd, VideoInfo *video){
        char *info;
        getAddr(fd, 0, (const void**)(&info));
        //Copy the Video name
        
        
        unsigned int i = 0;
        while((*info!='\r')&&(*info!='\n')&&(*info!='\0')&&i<10){
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

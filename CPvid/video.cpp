#include "video.hpp"

#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/file.hpp>

namespace Video{
    struct VideoInfo videos[MAX_VIDEOS];
    int numVideos;
    void LoadVideoInfo(){
        numVideos = 0;
        char filename[20]; //The name of the video info file
        char nr[2]; nr[1]=0; //The number of the current folder (vid0 - vid9)
        unsigned char i = 0;
        while (i<10) { //Test all 10 folders from vid0 to vid9
            strcpy(filename,"\\fls0\\vid");
            nr[0]='0'+i;
            strcat(filename,nr); //append the number after the "vid"
            strcat(filename,"\\video.txt");
            int fd = open(filename,OPEN_READ);
            
            if (fd>0){ //Video gefunden
                struct VideoInfo video;             //Create new VidInfo
                memset(&video, 0, sizeof(video));   //Fill it with 0s
                //if (VideoInfoParse(fd, video)){     //Load info from file
                    strcat (video.name, "Test");
                    video.name[4]='0'+i;
                    strcpy(video.description,filename);
                    //Add the video to videos
                    videos[numVideos++]=video;
                //}
            }
            
            close(fd);
            i++;
        }
    
    }
    bool VideoInfoParse(int fd, VideoInfo video){
        /*char *info;
        getAddr(fd, 0, (const void**)(&info));
        //Copy the Video name
        int i = 0;
        while((*info!='\r')&&(*info!='\r')&&(*info!='\r')&&i<10){
            video.name[i++]=*info++;
        }
        video.name[i]=0; //Terminate the name
    */
    }
}

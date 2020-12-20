#pragma once


namespace Video{
    struct VideoInfo {
        char folder[12]; // \fls0\vid0
        char name[64];
        char description[100];
        char w[2]; //This should be an uint16_t pointer, but that does not work...
        char h[2]; 
        //char director[100];
    };
    const int MAX_VIDEOS = 10;
    extern struct VideoInfo videos[MAX_VIDEOS];
    extern int numVideos;
    void LoadVideoInfo();
    bool VideoInfoParse(int fd, VideoInfo *video);
}

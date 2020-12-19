#pragma once


namespace Video{
    struct VideoInfo {
        char folder[12]; // \fls0\vid0
        //char folder;
        char name[64];
        char description[100];
        //char director[100];
    };
    const int MAX_VIDEOS = 10;
    extern struct VideoInfo videos[MAX_VIDEOS];
    extern int numVideos;
    void LoadVideoInfo();
    bool VideoInfoParse(int fd, VideoInfo *video);
}

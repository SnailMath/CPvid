#pragma once


namespace Video{
    struct VideoInfo {
        char folder[5];
        char name[100];
        char description[100];
        //char director[100];
    };
    const int MAX_VIDEOS = 10;
    extern struct VideoInfo videos[MAX_VIDEOS];
    extern int numVideos;
    void LoadVideoInfo();
    bool VideoInfoParse(int fd, VideoInfo video);
}

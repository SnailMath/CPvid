#pragma once

#include <sdk/os/mem.hpp>
#include <sdk/os/string.hpp>
#include <sdk/os/file.hpp>

namespace Video{
    struct VideoInfo {
        char folder[64]; // \fls0\vid0
        char name[32];
        char description[200];
        char w[2]; //This should be an uint16_t pointer, but that does not work...
        char h[2]; 
        char mode[2]; //The mode (either 565 [565-method] or 256 [256 colors] )
    };
    const int MAX_VIDEOS = 10;
    extern struct VideoInfo videos[MAX_VIDEOS];
    extern int numVideos;
    void LoadVideoInfo();
    bool VideoInfoParse(int fd, uint32_t fileSize, VideoInfo *video);
}

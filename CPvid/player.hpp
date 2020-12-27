#pragma once

#include <sdk/os/lcd.hpp>
#include <sdk/os/debug.hpp>

#include "video.hpp"

namespace Player{
	extern uint16_t *vram;
	extern int width, height;
	void blackScreen();
	void loadvideo(Video::VideoInfo *video);
	extern uint8_t status;
	extern uint8_t skipStatus;
	/*
	enum STATUS {
		PLAY = 0,		//Is currently playing
		PAUSE = 1,		//Is currently paused
		STOP = 2,		//Is stopped (finished playing)
		START = 3,		//Is stopped, but should start now ( go (back to) frame 0000 )
		STEP = 4,		//The video is stopped, but it should advance one image
		BACK = 5,		//The video is stopped, but it should reverse one image
		FORWARD = 6,	//The video is running, but it should skip 10 frames
		BACKWARD = 7	//The video is running, but it should go back 10 frames
	};*/
	enum STATUS {
		PLAY,		//Is currently playing
		PAUSE,		//Is currently paused
		PAUSE2,		//Is currently paused, but frame number changed (after STEP or BACK)
		STOP,		//Is stopped (finished playing)
		START,		//Is stopped, but should start now ( go (back to) frame 0000 )
		STEP,		//The video is stopped, but it should advance one image
		BACK,		//The video is stopped, but it should reverse one image
		FORWARD,	//The video is running, but it should skip 10 frames
		BACKWARD	//The video is running, but it should go back 10 frames
	};

}

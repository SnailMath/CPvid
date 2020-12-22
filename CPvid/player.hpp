#pragma once

#include <sdk/os/lcd.hpp>
#include <sdk/os/debug.hpp>

#include "video.hpp"

namespace Player{
	extern uint16_t *vram;
	extern int width, height;
	void blackScreen();
	void loadvideo(Video::VideoInfo *video);
}

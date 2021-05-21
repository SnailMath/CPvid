#include <stdint.h>
namespace Audio{
	//extern unsigned int soundblock; //current sound block number
	//extern unsigned int soundoffset;//offset in this sound block
	//extern bool sound_running; //if the sound is running or paused
	//extern int test;
	//extern uint32_t VBR_test;
	//extern volatile uint8_t* newExceptionVector;
	bool init(const char* folder);		//initialize playback
	void stop();				//stop playback, undo changes to the system, close serial
	void play(); 				//continue playback after pause
	void pause(); 				//pause playback
	//void play(uint32_t time); 		//continue playback after pause or rewind at specific position
	void pos(uint32_t time); 		//set position in audiofile
	int32_t getTime();			//get the current time int the audio, used to sync audio and video;
	//void waituntil(uint32_t time);		//wait until the time is reachd, used to sync audio and video;
	extern "C"
	void handleInterrupt();
};

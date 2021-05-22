#include "audio.hpp"
#include "scif.hpp"
#include <sdk/os/string.hpp>
#include <sdk/os/file.hpp>
#include <sdk/os/serial.hpp>
#include "exceptions.hpp" 

//#include "player.hpp" 

//Serial mode register			SCSMR	R/W	0000 16 
//Bit rate register				SCBRR	R/W	0004  8 
//Serial control register		SCSCR	R/W	0008 16 
//Transmit FIFO data register	SCFTDR	W	000C  8 
//Serial status register		SCFSR	R/W	0010 16        
//Receive FIFO data register	SCFRDR	R	0014  8 
//FIFO control register			SCFCR	R/W	0018 16 
//FIFO data count register		SCFDR	R	001C 16 
//Line status register			SCLSR	R/W	0024 16     


namespace Audio{
	int test = 0;

	//ram starts at 0x8c000000. The MCS area starts at 8cf80000 or so...
	//the hollyhock loader is at 8cfe0000 and our program starts at 8cff0000. We take (hopefully) less than f000 bytes (=61kb), so , sowe can take the area from 8cfff000 for the interrupt and exception vector.
	volatile uint8_t* newExceptionVector = (uint8_t*)(0x8cfff000);

	#define SOUNDLEN 1024
	unsigned char* soundaddr[SOUNDLEN]; // The array that contains the addresses of the audio pieces in rom. (addresses of the sound blocks)
	unsigned int   soundblock; //current sound block number
	unsigned int   soundoffset;//offset in this sound block
	bool sound_running; //if the sound is running or paused
	uint32_t VBR_backup;
	
	
	bool init(const char* folder){
		//build the filename
		char   audiopath[64];
		char   audioname[64];
		strcpy(audiopath, folder);
		audiopath[1]='f'; //change path from \drv0 to \fls0 so play always audio from the flash, even when video is on the sdcard.
		audiopath[2]='l';
		audiopath[3]='s';
		strcpy(audioname, audiopath);
		strcat(audioname, "\\audio.wav"); // \fls0\videoname\audio.wav
		//open the audio file
		int audiofile = open(audioname,OPEN_READ);
		if(audiofile<=0){ //if we don't have an audio file, try another name:
			strcpy(audioname, audiopath);
			strcat(audioname, ".wav"); // \fls0\videoname.wav
			audiofile = open(audioname,OPEN_READ);
			if(audiofile<=0) //if we don't have an audio file, just return false
				return false;
		}
		//get the addresses of each part of the audio file
		for(int i=0;i<SOUNDLEN;i++)
			getAddr(audiofile, i*1024*4, (const void**)&soundaddr[i]);
		
		soundoffset = 0;
		soundblock  = 0;
		sound_running = false;
		
		//close the audio file again (we have the addresses saved)
		close(audiofile);  

		//we made the last part of our memory to the new vector stuff... so we have to copy the functions from exceptions.s here.
		for(int i=0; i<12; i++) //copy the new general exception handler into our new vector at 0x100
			newExceptionVector[0x100 + i] = ((uint8_t*)v_generalException)[i];
		for(int i=0; i<12; i++) //copy the new miss exception handler into our new vector at 0x400
			newExceptionVector[0x400 + i] = ((uint8_t*)v_missException)[i];
		for(int i=0; i<52; i++) //copy the new interrupt handler into our new vector at 0x600
			newExceptionVector[0x600 + i] = ((uint8_t*)v_interrupt)[i];

		//back up original VBR (The pointer to the interrupt functions)
		__asm volatile (
			"stc VBR, %0\n"
			: "=r" (VBR_backup) /*output*/ 
			:  /*input*/ 
			:  /*used register*/ 
			"memory"
		);
		//put our own interrupt and exception vector in the VBR register (to hijack the exceptions)
		__asm volatile (
			"ldc %0, VBR\n"
			:  /*output*/ 
			: "r"  (newExceptionVector) /*input*/ 
			:  /*used register*/ 
			"memory"
		);
		
		//Open the serial port
		unsigned char mode[6] = {0, 9, 0, 0, 0, 0};    // 11520 bps 8n1, we'll change that to 922000bps
		Serial_Open(mode);

		//Make serial port faster
		SCIF::regWrite(SCIF::SCBRR,0);	//Make it the fastest. (922000bps)
		//SCIF::regSetBit(SCIF::CA);	//Change to synchronous mode (no start and stop bit)
		//SCIF::regClearBit(SCIF::CKE1);SCIF::regSetBit(SCIF::CKE0);	//internal clock

		//Enable TX interrupt
		SCIF::regSetBit(SCIF::TIE); //Enable TX interrupt

		return true;
	}

	void stop(){
		//restore original VBR
		asm (
			"ldc %0, VBR\n"
			: /*output*/ 
			: "r" (VBR_backup) /*input*/ 
			: /*used register*/ 
		);
		
		//Close the serial (stop transmission if active)
		Serial_Close(1); 
	}

	void pos(uint32_t time){
		soundblock  = time>>12;		//current sound block number
		soundoffset = time & 0xfff;	//offset in this sound block
	}
	/*void play(uint32_t time){
		pos(time);
		play();
	}*/
	void play(){
		sound_running = true;
		handleInterrupt();
		handleInterrupt();//fill the fifo so there is an interrupt if it is half empty again.
		SCIF::regSetBit(SCIF::TIE); //Enable TX interrupt
	}
	void pause(){
		sound_running = false;
	}
	int32_t getTime(){ //get the current time int the audio
		return (soundblock<<12) + soundoffset;
	}
	/*void waituntil(uint32_t time){ //wait until the time is reachd, used to sync audio and video;
		while(soundblock<(time>>12));
		while(soundoffset<(time&0xfff));
	}*/

	uint8_t pwm[9] = {
		0b00000000,
		0b00000001,
		0b00000011,
		0b00000111,
		0b00001111,
		0b00011111,
		0b00111111,
		0b01111111,
		0b11111111
	};

	extern "C"
	void handleInterrupt(){
		test++;
		//check fi TDFE is set (Transmit FIFO Data Empty)		
		if(SCIF::regTestBit(SCIF::TDFE) && sound_running){	//if we have space in the buffer AND are currently playing audio
			unsigned char* buf = soundaddr[soundblock];	//look up where this block is in rom
			unsigned char c = buf[soundoffset++]>>2;	//load first byte, aka 1 sample, use only 6 bit.
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++; //split into 8 pwm bytes with c bits set
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );  c++;
			SCIF::regWrite (SCIF::SCFTDR, pwm[c>>3] );//c+=;
			if(soundoffset>=1024*4){ //we are at the end of the current block
				soundoffset=0;
				soundblock++;
				if (soundblock==SOUNDLEN){//if there are no more blocks
					soundblock=0;
					sound_running = false;
					SCIF::regClearBit(SCIF::TIE); //Disable TX interrupt
				}
			}
			//SCIF::regWrite (SCIF::SCFTDR,'A');//fill the fifo
			//SCIF::regWrite (SCIF::SCFTDR,'B');
			//SCIF::regWrite (SCIF::SCFTDR,'C');
			//SCIF::regWrite (SCIF::SCFTDR,'D');
			//SCIF::regWrite (SCIF::SCFTDR,'E');
			//SCIF::regWrite (SCIF::SCFTDR,'F');
			//SCIF::regWrite (SCIF::SCFTDR,'G');
			//SCIF::regWrite (SCIF::SCFTDR,'H');
			SCIF::regClearBit(SCIF::TDFE);//clear transmit empty flag
			SCIF::regClearBit(SCIF::TEND);//clear transmission end flag (start transmission if not running)
		}
	
	}

};

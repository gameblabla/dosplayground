#ifndef SB_H
#define SB_H

struct WaveData {
  unsigned int SoundLength, Frequency;
  unsigned char *Sample;
};

typedef struct SoundDevice {
	/* * * */
	/* The name of this video driver */
	const char *name;

	void (*Sound_Init)();

	void (*Sound_Load)(const char* WaveFile, struct WaveData *Voice);
	void (*Sound_Play)(struct WaveData *Voice);
	void (*Sound_Unload)(struct WaveData *vc);
	
	void (*Sound_Close)();
} SoundDevice;

extern SoundDevice SB_device;
extern SoundDevice SBDMA_device;


#endif

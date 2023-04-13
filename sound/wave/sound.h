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

	unsigned char (*Sound_Init)();

	unsigned char (*Sound_Load)(const char* WaveFile, struct WaveData *Voice, unsigned char index);
	void (*Sound_Play)(struct WaveData *Voice, unsigned char index);
	void (*Sound_Unload)(struct WaveData *vc, unsigned char index);
	
	void (*Sound_Close)();
} SoundDevice;

extern SoundDevice SB_device;
extern SoundDevice SBDMA_device;
extern SoundDevice PMDPCM_device;

#endif

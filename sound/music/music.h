#ifndef SOUND_H
#define SOUND_H

typedef struct MusicDevice {
	/* * * */
	/* The name of this video driver */
	const char *name;

	void (*Init_Music)(unsigned short flags);

	unsigned char (*Load_Music)(const char* filename);
	void (*Play_Music)();
	void (*Stop_Music)();
	
	void (*Close_Music)();
} MusicDevice;

extern MusicDevice IMF_device;
extern MusicDevice MIDI_device;

#endif

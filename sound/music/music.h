#ifndef SOUND_H
#define SOUND_H

extern void Init_Music();

extern unsigned char Load_Music(const char* filename);

extern void Play_Music();

extern void Stop_Music();
extern void Close_Music();

#endif

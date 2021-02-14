#ifndef SB_H
#define SB_H

struct WaveData {
  unsigned int SoundLength, Frequency;
  unsigned char *Sample;
};

extern void SB_Init();
extern void SB_Load(const char* WaveFile, struct WaveData *Voice);
extern void SB_Play(struct WaveData *Voice);
extern void SB_Close();
extern void SB_Unload(struct WaveData *vc);

#endif

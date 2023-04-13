#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "music.h"

static void* imf;
static long imflen;

// Declare assembly routines in player.asm
extern void InstallPlayer(int rate);
extern void RemovePlayer();
extern void PlayIMF(void* imfdata, int imflen);
extern void StopIMF();
extern int GetPosIMF();
extern int GetLoopIMF();

void IMF_Init_Music(unsigned short flags)
{
	InstallPlayer(560);
}

unsigned char IMF_Load_Music(const char* filename, unsigned long size)
{
	int fd;
	
	if (imf)
	{
		free(imf);
		imf = NULL;
	}
	
	imf = malloc(size);
	if (!imf) return 0;

	_dos_open(filename, O_RDONLY, &fd);
	_dos_read(fd, (char*)&imf, size, &imflen);
	_dos_close(fd);
	
	return 1;
}

void IMF_Play_Music()
{
	PlayIMF(imf, (int)imflen);
}

void IMF_Stop_Music()
{
	// Stop IMF
	StopIMF();
}

void IMF_Close_Music()
{
	// Stop IMF
	StopIMF();
	// Remove player interrupt service
	RemovePlayer();	
}

MusicDevice IMF_device = {
	"IMF driver",
	IMF_Init_Music,
	IMF_Load_Music,
	IMF_Play_Music,
	IMF_Stop_Music,
	IMF_Close_Music
};


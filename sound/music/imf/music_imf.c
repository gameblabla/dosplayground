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

static void *imf_loadfile(const char* filename, long* length) 
{
	// Very simple file loader routine
	void* buff;
	long size;
	FILE* fp;
	
	//printf("Opening file %s...\n", filename);
	fp = fopen(filename, "rb");
	
	if (!fp) {
		//printf("Cannot load %s!\n", filename);
		return NULL;
	}
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	//printf("Loading %u bytes...\n", size);
	
	buff = malloc(size);
	fread(buff, 1, size, fp);

	fclose(fp);
	
	if ( length ) {
		*length = size;
	}		

	return buff;    
}

void IMF_Init_Music(unsigned short flags)
{
	InstallPlayer(560);
}

unsigned char IMF_Load_Music(const char* filename)
{
	imf = imf_loadfile(filename, &imflen);
	if (imf)
	{
		return 1;
	}
	return 0;
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


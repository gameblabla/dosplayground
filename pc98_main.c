
#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <dos.h>

//#include "snd.h"
#include "music.h"
#include "sound.h"
#include "generic.h"
#include "graph.h"


BITMAP bmp[2];
short y = 0, x = 0;

struct WaveData *Voice;

VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;
/*
	PC-98 version
	Run
	pmd86
	p86drv /16
	before running this
*/

int main(int argc,char **argv) 
{
	int done = 0;
	
	/*
	 * Normal_VGA
	 * Planar_VGA
	 * Normal_8514
	*/
	mainvideo = Normal_PC9821_PGC;
	
	/*
	 * IMF_device
	 * MIDI_device
	 * PMD_Device
	*/
	mainmusic = PMD_device;
	
	mainmusic.Init_Music(0);
	if (mainmusic.Load_Music("MUSIC.M", 4249) == 0)
	{
		printf("Can't load music\n");
		return 0;
	}
	mainmusic.Play_Music();
	
	printf("Play Music\n");
	
	/*
	 * SB_device
	 * SBDMA_device
	 * PMDPCM_device
	*/
	mainsound = PMDPCM_device;
	
	mainsound.Sound_Init();
	if (mainsound.Sound_Load("GAME.P86", Voice, 0) == 0)
	{
		printf("Can't load sound effects\n");
		return 0;
	}
	mainsound.Sound_Play(Voice, 0);
	
	mainvideo.SetVideoMode(640, 400, 0, argc, argv);
	mainvideo.LoadBMP("DEMO.SIF", &bmp[0], 0, 0, 1);
	mainvideo.SetPalette();
		
	//input = createInputManager();

	while (!done) 
	{
		mainvideo.DrawBMP_static(&bmp[0], 0, 0, 0);

		/*if (inputGetArrow(input, ArrowUp) == 1) 
		{
			y--;
		}
		else if (inputGetArrow(input, ArrowDown) == 1) 
		{
			y++;
		}
		
		if (inputGetArrow(input, ArrowLeft) == 1) 
		{
			x--;
		}
		else if (inputGetArrow(input, ArrowRight) == 1) 
		{
			x++;
		}
		
		if (inputGetKey(input, 0x1C) == 1)
		{
			mainsound.Sound_Play(&Voice);
		}
		
		if (inputGetKey(input, 0x1) == 1 || inputGetKey(input, 0xF) == 1)
		{
			done = 1;
		}*/

		mainvideo.FlipVideo();
		mainvideo.Vsyncwait();
	}
	
	mainvideo.FreeBMP(bmp[0]);
	mainvideo.FreeVideo();
	

	return 0;
}



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

#include "generic.h"
#include "keyb.h"
#include "graph.h"
#include "sound.h"


BITMAP bmp[2];
short y = 0, x = 0;
InputManager* input;

VideoDevice mainvideo;
MusicDevice mainmusic;
SoundDevice mainsound;

struct WaveData Voice;         //Pointer to wave file;

/*
	Z = 44
	X = 45
	C = 46
	Enter/Return = 0x1C
	Escape = 0x1
*/

int main(int argc,char **argv) 
{
	int done = 0;
	/*
	 * Normal_VGA
	 * Planar_VGA
	 * Normal_8514
	*/
	mainvideo = Normal_VGA;
	
	/*
	 * IMF_device
	 * MIDI_device
	*/
	mainmusic = IMF_device;
	
	/*
	 * SB_device
	 * SBDMA_device
	*/
	mainsound = SBDMA_device;
	
	mainvideo.SetVideoMode(320, 200, 0, argc, argv);
	mainvideo.LoadBMP("DEMO.SIF", &bmp[0], 0, 0, 1);
	printf("YES\n");
	//mainvideo.LoadBMP("CURS.BMP", &bmp[1], 0, 0, 0);
	mainvideo.SetPalette();
	
	mainmusic.Init_Music(0);
	mainmusic.Load_Music("code.imf");
	mainmusic.Play_Music();
	
	mainsound.Sound_Init();
	mainsound.Sound_Load("TEST.WAV", &Voice);
	mainsound.Sound_Play(&Voice);
		
	input = createInputManager();
	
	while (!done) 
	{
		mainvideo.DrawBMP_static(&bmp[0], 0, 0);
		//mainvideo.DrawBMP_sprite_notrans(&bmp[1], x, y, 0);

		if (inputGetArrow(input, ArrowUp) == 1) 
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
		}

		mainvideo.FlipVideo();
		mainvideo.Vsyncwait();
	}
	
	destroyInputManager(input);
	
	mainsound.Sound_Unload(&Voice);
	mainsound.Sound_Close();
	
	mainvideo.FreeBMP(bmp[0]);
	mainvideo.FreeBMP(bmp[1]);
	mainvideo.FreeVideo();
	
	mainmusic.Close_Music();

	return 0;
}


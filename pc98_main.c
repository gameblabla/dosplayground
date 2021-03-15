
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
#include "graph.h"


BITMAP bmp[2];
short y = 0, x = 0;

VideoDevice mainvideo;

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
	mainvideo = Normal_PC9821_PGC;
	
	/*
	 * IMF_device
	 * MIDI_device
	*/
	//mainmusic = IMF_device;
	
	/*
	 * SB_device
	 * SBDMA_device
	*/
	//mainsound = SBDMA_device;
	
	mainvideo.SetVideoMode(640, 400, 0, argc, argv);
	mainvideo.LoadBMP("DEMO.SIF", &bmp[0], 0, 0, 1);
	mainvideo.SetPalette();
		
	//input = createInputManager();
	

	while (!done) 
	{
		mainvideo.DrawBMP_static(&bmp[0], 0, 0);

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


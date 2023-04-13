/****************************************************************************
** WARNING: You need a COMPACT memory model to run this.                   **
*****************************************************************************
** Demonstration of playing a single wave file using DMA                   **
**  by Steven H Don                                                        **
**                                                                         **
** For questions, feel free to e-mail me.                                  **
**                                                                         **
**    shd@earthling.net                                                    **
**    http://shd.cjb.net                                                   **
**                                                                         **
****************************************************************************/
//Include files
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <sys/types.h>
#include "sound.h"

#define PCM86_VECTOR 0x65

#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();

static int check_pcm86()
{
    uint32_t pcm86vector;
    uint16_t pcm86vector_offset, pcm86vector_segment;
    char p, c, m;
    __dpmi_regs regs;

    /* Enable near pointer access to first 640 KB of memory */
	ENABLE_NEAR();

    /* Get the PCM86 vector address */
    regs.x.ax = 0x3500 | PCM86_VECTOR;
    __dpmi_int(0x21, &regs);
    pcm86vector_offset = regs.x.bx;
    pcm86vector_segment = regs.x.es;

    /* Calculate the linear address */
    pcm86vector = ((uint32_t)pcm86vector_segment << 4) + pcm86vector_offset + __djgpp_conventional_base;

    /* Read the characters at the PCM86 vector address */
    p = *((char *)(pcm86vector + 2));
    c = *((char *)(pcm86vector + 3));
    m = *((char *)(pcm86vector + 4));

    /* Disable near pointer access */
    DISABLE_NEAR();

    /* Check if the characters match "P86" */
    if (p == 'P' && c == '8' && m == '6') {
        return 0; /* PCM86 signature found */
    }
    return 1; /* PCM86 signature not found */
}

static int load_p86_file(const char *filename)
{
    unsigned long dos_buf = __tb;
    size_t filename_len = strlen(filename) + 1;
    __dpmi_regs dpmi_regs;

    // Enable near pointer access to conventional memory
    ENABLE_NEAR();
    
    // Copy the filename to the DOS buffer
    dosmemput(filename, filename_len, dos_buf);

    // Prepare input registers
    dpmi_regs.h.ah = 6; // Set AH=6 to load a P86 file

    // Pass the filename to DS:DX
    dpmi_regs.x.dx = dos_buf & 0xF;
    dpmi_regs.x.ds = dos_buf >> 4;

    // Call interrupt 0x65 with the prepared registers
    __dpmi_int(0x65, &dpmi_regs);
    
    // Disable near pointer access to conventional memory
    DISABLE_NEAR();

	return dpmi_regs.h.al; // Return AL register value with the result of the operation
}


static void pmd_play_pcm_sound_effect(unsigned char sound_effect_index, unsigned short frequency, char pan, unsigned char volume)
{
    __dpmi_regs inregs;
    
    // Enable near pointer access to conventional memory
    ENABLE_NEAR();

    // Set AH to 0x0F for PCM sound effect
    inregs.h.ah = 0x0F;

    // Set AL to the sound effect index
    inregs.h.al = sound_effect_index;

    // Set DX to the frequency
    inregs.x.dx = frequency;

    // Set CH to the pan
    inregs.h.ch = pan;

    // Set CL to the volume
    inregs.h.cl = volume;

    // Call the music driver
    __dpmi_int(0x60, &inregs);
    
    // Disable near pointer access to conventional memory
    DISABLE_NEAR();
}

static int initialize_p86()
{
    __dpmi_regs regs;

    regs.h.ah = 0; // Set AH=0 to initialize P86

    __dpmi_simulate_real_mode_interrupt(0x65, &regs); // Call interrupt 0x65

    if (regs.h.al == 0) {
        return 0; // Normal end
    } else {
        return -1; // Abnormal end
    }
}

unsigned char PMDPCM_Init()
{
	if (check_pcm86() == 0)
	{
		if (initialize_p86() == 0)
		{
			return 1;
		}
	}
	return 0;
}


unsigned char PMDPCM_Load(const char* file, struct WaveData *vc, unsigned char index)
{
	if (load_p86_file(file) == 0) return 1;
	return 0;
}

void PMDPCM_Unload(struct WaveData *vc, unsigned char index)
{

}

void PMDPCM_Play(struct WaveData *vc, unsigned char index)
{
	pmd_play_pcm_sound_effect(index, 0x2200, 0, 255);
}

void PMDPCM_Close()
{

}


SoundDevice PMDPCM_device = {
	"PMDPCM driver",
	PMDPCM_Init,
	PMDPCM_Load,
	PMDPCM_Play,
	PMDPCM_Unload,
	PMDPCM_Close
};

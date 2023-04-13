#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <pc.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>
#include <sys/types.h>

#include "music.h"
#define PMD_VECTOR 0x60

// Declare functions from playmidi.c
extern int LoadPMD (const char *fname);
extern void StopPMD ();
extern void PlayPMD ();
extern void InitPMD ();
extern char SetFM ();
extern void UnloadPMD ();
extern int Playing ();

//#define ENABLE_NEAR() if (__djgpp_nearptr_enable() == 0) { printf("Could not get access to first 640K of memory.\n"); exit(-1); }
#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();

static unsigned char* music_data;
static unsigned music_len;

static int check_pmd()
{
    uint32_t pmdvector;
    uint16_t pmdvector_offset, pmdvector_segment;
    char p, m, d;
    __dpmi_regs regs;

    /* Enable near pointer access to first 640 KB of memory */
    ENABLE_NEAR();

    /* Get the PMD vector address */
    regs.x.ax = 0x3500 | PMD_VECTOR;
    __dpmi_int(0x21, &regs);
    pmdvector_offset = regs.x.bx;
    pmdvector_segment = regs.x.es;

    /* Calculate the linear address */
    pmdvector = ((uint32_t)pmdvector_segment << 4) + pmdvector_offset + __djgpp_conventional_base;

    /* Read the characters at the PMD vector address */
    p = *((char *)(pmdvector + 2));
    m = *((char *)(pmdvector + 3));
    d = *((char *)(pmdvector + 4));

    /* Disable near pointer access */
    DISABLE_NEAR();

    /* Check if the characters match "PMD" */
    if (p == 'P' && m == 'M' && d == 'D') {
        return 0; /* PMD signature found */
    }
    return 1; /* PMD signature not found */
}


static void CallMusicDriver(unsigned char ah_value, unsigned int *segment, unsigned int *offset)
{
    __dpmi_regs regs;

    regs.h.ah = ah_value;
    __dpmi_int(0x60, &regs);

    if (segment) {
        *segment = regs.x.ds;
    }
    if (offset) {
        *offset = regs.x.dx;
    }
}

static  void PMD_MusicStart(const unsigned char *music_data, unsigned int music_data_size)
{
    unsigned int segment, offset;

    // Enable near pointer access to conventional memory
    ENABLE_NEAR();

    // API call 01 - Stop Song
    CallMusicDriver(0x01, NULL, NULL);

    // API call 06 - Get Song Buffer Address
    CallMusicDriver(0x06, &segment, &offset);

    // Load music data into song buffer
    unsigned long linear_address = (segment << 4) + offset + __djgpp_conventional_base;
    memcpy((void *)linear_address, music_data, music_data_size);

    // API call 00 - Play Song
    CallMusicDriver(0x00, NULL, NULL);

    // Disable near pointer access to conventional memory
    DISABLE_NEAR();
}


static unsigned char PMD_Init_Music(unsigned short flags)
{
	char ret;
	ret = check_pmd();
	if (ret != 0) return 0;
	
	return 1;
}

static unsigned char PMD_Load_Music(const char* filename, unsigned long size)
{
	int fd;
	unsigned totalsize;
	if (music_data)
	{
		free(music_data);
		music_data = NULL;
	}
	
	music_len = size;
	music_data = malloc(size);
	if (!music_data) return 0;

	_dos_open(filename, O_RDONLY, &fd);
	_dos_read(fd, music_data, size, &totalsize);
	_dos_close(fd);
	
	return 1;
}

static void PMD_Play_Music()
{
	PMD_MusicStart(music_data, music_len);
}

static void PMD_Stop_Music()
{
    // Enable near pointer access to conventional memory
    ENABLE_NEAR();

    // API call 01 - Stop Song
    CallMusicDriver(0x01, NULL, NULL);

    // Disable near pointer access to conventional memory
    DISABLE_NEAR()
}

static void PMD_Close_Music()
{
	if (music_data)
	{
		free(music_data);
		music_data = NULL;
	}
}

MusicDevice PMD_device =
{
	"PMD Music",
	PMD_Init_Music,
	PMD_Load_Music,
	PMD_Play_Music,
	PMD_Stop_Music,
	PMD_Close_Music
};

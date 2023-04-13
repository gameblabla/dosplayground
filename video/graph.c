#include <dos.h>
#include <stdio.h>  /* printf(), puts(), fopen()... */
#include <limits.h> /* ULONG_MAX */
#include <stdlib.h> /* malloc(), free(), rand() */
#include <string.h> /* strcmp() */
#include <time.h>

#include "generic.h"
#include "graph.h"

uint8_t VGA_8158_GAMEPAL[769];
unsigned char *hold_screen;

/**************************************************************************
 *  set_mode                                                              *
 *     Sets the video mode.                                               *
 **************************************************************************/

void set_mode(uint8_t mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  #if TARGET_MSDOS == 32
  int386(VIDEO_INT, &regs, &regs);
  #else
  int86(VIDEO_INT, &regs, &regs);
  #endif
}


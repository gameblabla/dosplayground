#include <dos.h>
#include <stdio.h>  /* printf(), puts(), fopen()... */
#include <limits.h> /* ULONG_MAX */
#include <stdlib.h> /* malloc(), free(), rand() */
#include <string.h> /* strcmp() */
#include <time.h>

#include "generic.h"
#include "graph.h"

byte VGA_8158_GAMEPAL[769];
unsigned char *hold_screen;

/**************************************************************************
 *  set_mode                                                              *
 *     Sets the video mode.                                               *
 **************************************************************************/

void set_mode(byte mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  #ifdef DJGPP
  int386(VIDEO_INT, &regs, &regs);
  #else
  int86(VIDEO_INT, &regs, &regs);
  #endif
}


void fskip(FILE *fp, int num_bytes)
{
   int i;
   for (i=0; i<num_bytes; i++)
      fgetc(fp);
}


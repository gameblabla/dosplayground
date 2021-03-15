/*
   Mouse routines

   Copyright (c) 2014, Mateusz Viste
   All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <dos.h>

#include "mouse.h"  /* include self for control */

/* init the mouse driver (and checks for presence of mouse support at same time)
 * returns 0 if no mouse is present, and the number of buttons otherwise */
int mouse_init(void) {
  union REGS regs;
  regs.x.ax = 0;
  int86(0x33, &regs, &regs);
  if (regs.x.ax == 0xFFFF) return(regs.x.bx);
  return(0);
}

/* shows the mouse pointer */
void mouse_show(void) {
  union REGS regs;
  regs.x.ax = 1;
  int86(0x33, &regs, &regs);
}

/* hides the mouse pointer */
void mouse_hide(void) {
  union REGS regs;
  regs.x.ax = 2;
  int86(0x33, &regs, &regs);
}

/* get x/y coordinates of the mouse, and returns a bitfield with state of buttons */
int mouse_getstate(int *x, int *y) {
  union REGS regs;
  regs.x.ax = 3;
  int86(0x33, &regs, &regs);
  if (x) *x = regs.x.cx;
  if (y) *y = regs.x.dx;
  return(regs.x.bx);
}

/* get x/y coordinates of the mouse when the last button release occured since last check.
   returns the id of the button pressed (1 or 2), or 0 if no event occured. */
int mouse_fetchrelease(int *x, int *y) {
  int buttonid;
  union REGS regs;
  /* check left and right buttons */
  for (buttonid = 0; buttonid < 2; buttonid++) {
    regs.x.ax = 6;         /* INT 33,6 = "get mouse button release data" */
    regs.x.bx = buttonid;  /* button id (left = 0 ; right = 1) */
    int86(0x33, &regs, &regs); /* make the INT call */
    if ((regs.x.bx != 0) && ((regs.x.ax & 3) == 0)) { /* if a click occured, report it,  */
      *x = regs.x.cx;                                 /* but only if all buttons were    */
      *y = regs.x.dx;                                 /* released at that time! This is  */
      return(buttonid + 1);                           /* to avoid false clicks from some */
    }                                                 /* mouses.                         */
  }
  return(0);
}

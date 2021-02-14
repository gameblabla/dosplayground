/*
 * Mouse routines
 *
 * Copyright (C) 2014 Mateusz Viste
 * All rights reserved.
 */

#ifndef mouse_h_sentinel
  #define mouse_h_sentinel

  /* init the mouse driver (and checks for presence of mouse support at same time)
   * returns 0 if no mouse is present, and the number of buttons otherwise */
  int mouse_init(void);

  /* shows the mouse pointer */
  void mouse_show(void);

  /* hides the mouse pointer */
  void mouse_hide(void);

  /* get x/y coordinates of the mouse, and returns a bitfield with state of buttons */
  int mouse_getstate(int *x, int *y);

  /* get x/y coordinates of the mouse when the last button release occured since last check.
     returns the id of the button pressed (1 or 2), or 0 if no event occured. */
  int mouse_fetchrelease(int *x, int *y);

#endif

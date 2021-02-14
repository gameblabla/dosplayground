#include <dos.h>
#include <stdio.h> 
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "generic.h"

unsigned short screen_width = 320, screen_height = 200;

#ifdef DJGPP
word *my_clock = (word *)0x046C;      /* this points to the 18.2hz system clock. */
#else
word *my_clock = (word *)0x0000046C;
#endif

/* returns the lower-case version of c char, if applicable */
static int lcase(char c) {
  if ((c >= 'A') && (c <= 'Z')) return(c + 32);
  return(c);
}

/* a case-insensitive version of strcmp() */
int strucmp(char *s1, char *s2) 
{
	for (;;) 
	{
		if (lcase(*s1) != lcase(*s2)) return(1);
		if (*s1 == 0) return(0);
		s1++;
		s2++;
	}
}

/* checks whether str starts with start or not. returns 0 if so, non-zero
 * otherwise - this function is case insensitive */
int stringstartswith(char *str, char *start) 
{
	if ((str == NULL) || (start == NULL)) return(-1);
	while (*start != 0) 
	{
		if (lcase(*start) != lcase(*str)) return(-1);
		str++;
		start++;
	}
	
	return(0);
}


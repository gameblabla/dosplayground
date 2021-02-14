#ifndef _H_GEN_
#define _H_GEN_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


#ifdef DJGPP
#include <sys/nearptr.h>
#else
#include <i86.h>
#endif

#ifdef DJGPP
#define _far 
#define far 
#define __far
#define interrupt 
#endif

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

extern int stringstartswith(char *str, char *start);
extern int strucmp(char *s1, char *s2);

extern unsigned short screen_width, screen_height;

#endif

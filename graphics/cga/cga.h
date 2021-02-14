#ifndef _CGA_GRAPH_
#define _CGA_GRAPH_

#ifdef DJGPP
extern byte *CGA;          /* this points to video memory. */
#else
extern byte far *CGA;        /* this points to video memory. */
#endif


#endif

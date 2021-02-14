#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "cga.h"

#ifdef DJGPP
byte *CGA = (byte *)0xB8000;          /* this points to video memory. */
#else
byte far *CGA=(byte *)0xB8000000L;        /* this points to video memory. */
#endif


/**************************************************************************
 *  fskip                                                                 *
 *     Skips bytes in a file.                                             *
 **************************************************************************/

void load_cga_rawpict(unsigned char* file, unsigned char *b, unsigned short size)
{
	FILE* fp;
	fp = fopen(file, "rb");
	fread(&b, 1, size, fp);
}


static void plot_cga(unsigned short x, unsigned char y, unsigned char color, word add_p)
{
	static const byte b_mask[]={6,4,2,0};
	static const byte m_mask[]={192,48,12,3};
	byte far *p = CGA;
	byte b, m, c;
	p += add_p;
	c = x & 0x3;
	b = b_mask[c];
	m = m_mask[c];
	p += ((y << 3) + (y << 5) + (x >> 2));
	*p = (*p & ~m) | (color << b);
}

inline void _internal_draw_pict_cga(unsigned char *data, unsigned short x, unsigned char y, unsigned short w, unsigned char h)
{
	word p;
	unsigned short i = w-1;
	unsigned short STEPS_PICT = w;
	unsigned char a = h;
	short x2, y2, color;
	
	do
	{
		i = STEPS_PICT;	
		y2 = a + y;
		p = 0;
		if (y2 & 0x0001) 
		{
			p = 0x2000;
			y2 &= 0xFFFE;
		}
		do
		{
			x2 = i + x;
			color = data[i+(a*w)];
			plot(x2, y2, color,p);
		} while (--i);
	} while (--a);
}

void _raw_draw_cga(unsigned char *data, unsigned short size)
{
	memcpy(CGA, data, size);
}


void Init_screen_CGA()
{
	set_mode(0x04);
	hold_screen = malloc(320*200);
}

void Kill_CGA()
{
	if (hold_screen != NULL) free(hold_screen);
	set_mode(TEXT_MODE);
}


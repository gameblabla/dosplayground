#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <malloc.h>
#include <unistd.h>

#include <stdint.h>

#include "generic.h"
#include "generic_io.h"
#include "graph.h"
#include "vga.h"
#include "registers.h"

static byte *VGA =
#ifdef DJGPP
(byte *)0xA0000;          /* this points to video memory. */
#else
(byte *)0xA0000000L;        /* this points to video memory. */
#endif

static unsigned long SCREEN_SIZE;
static unsigned short quarter_scr;
static word visual_page = 0;

// For non planar mode only
static byte* doublebuffer;

static void readyVgaRegs(void)
{
	int v;
	outp(0x3d4,0x11);
    v = inp(0x3d5) & 0x7f;
	outp(0x3d4,0x11);
	outp(0x3d5,v);
}

static void outReg(const Register r)
{
	switch (r.port)
	{
		/* First handle special cases: */
		case ATTRCON_ADDR:
			inp(STATUS_ADDR);  		/* reset read/write flip-flop */
			outp(ATTRCON_ADDR, r.index | 0x20);
										/* ensure VGA output is enabled */
			outp(ATTRCON_ADDR, r.value);
		break;
		case MISC_ADDR:
		case VGAENABLE_ADDR:
			outp(r.port, r.value);	/*	directly to the port */
		break;
		case SEQ_ADDR:
		case GRACON_ADDR:
		case CRTC_ADDR:
		default:						/* This is the default method: */
			outp(r.port, r.index);	/*	index to port			   */
			outp(r.port+1, r.value);/*	value to port+1 		   */
		break;
	}
}

static void outRegArray(const Register *r, int n)
{
    readyVgaRegs();
	while (n--)
	{
		outReg(*r++);
	}
}


static void VGA_SetVideo(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	unsigned char c;
	#ifdef DJGPP
	if (__djgpp_nearptr_enable() == 0)
	{
		printf("Could get access to first 640K of memory.\n");
		exit(-1);
	}

	VGA+=__djgpp_conventional_base;
	my_clock = (void *)my_clock + __djgpp_conventional_base;
	#endif
	
	screen_width = width;
	screen_height = height;
	SCREEN_SIZE = screen_width * screen_height;
	quarter_scr = SCREEN_SIZE / 4;
	
	set_mode(VGA_256_COLOR_MODE);
	
	switch(width)
	{
		case 256:
			switch(height)
			{
				case 240:
					outRegArray(vga_256x240_chained, sizeof(vga_256x240_chained)/sizeof(Register));
				break;
				case 224:
					outRegArray(vga_256x224_chained, sizeof(vga_256x224_chained)/sizeof(Register));
				break;
			}
		break;
	}
	
	if (!doublebuffer)
	{
		doublebuffer = malloc(SCREEN_SIZE);
	}

	outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
}

static void VGA_SetVideo_Planar(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	unsigned short i;
	dword *ptr=(dword *)VGA;
	
	#ifdef DJGPP
	if (__djgpp_nearptr_enable() == 0)
	{
		printf("Could get access to first 640K of memory.\n");
		exit(-1);
	}

	VGA+=__djgpp_conventional_base;
	my_clock = (void *)my_clock + __djgpp_conventional_base;
	#endif
	
	screen_width = width;
	screen_height = height;
	SCREEN_SIZE = screen_width * screen_height;
	quarter_scr = SCREEN_SIZE / 4;

	/* set mode 13 */
	set_mode(VGA_256_COLOR_MODE);

	/* clear all 256K of memory */
	for(i=0;i<0x4000;i++)
	{
		*ptr++ = 0;
	}

	switch(width)
	{
		case 640:
			switch(height)
			{
				case 400:
					outRegArray(vga_planar_640x400_special, sizeof(vga_planar_640x400_special)/sizeof(Register));
				break;
			}
		break;
		case 376:
			switch(height)
			{
				case 564:
					outRegArray(vga_planar_376x564, sizeof(vga_planar_376x564)/sizeof(Register));
				break;
			}
		break;
		case 360:
			switch(height)
			{
				case 400:
					outRegArray(vga_planar_360x400, sizeof(vga_planar_360x400)/sizeof(Register));
				break;
				case 270:
					outRegArray(vga_planar_360x270, sizeof(vga_planar_360x270)/sizeof(Register));
				break;
			}
		break;
		case 320:
			switch(height)
			{
				case 240:
					outRegArray(vga_planar_320x240, sizeof(vga_planar_320x240)/sizeof(Register));
				break;
			}
		break;
	}

	outpw(0x3c4, 0x0f02);	/* Enable all 4 planes */
}


static void VGA_Draw_static_bitmap_planar(BITMAP *bmp, short x, short y)
{
	int j,plane;
	word screen_offset;
	word bitmap_offset;

	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);          /* select plane */
		outp(SC_DATA,  1 << ((plane+x)&3) );
		bitmap_offset=0;
		screen_offset = ((dword)y*screen_width+x+plane) >> 2;
		for(j=0; j<bmp->height; j++)
		{
			memcpy(&VGA[quarter_scr+screen_offset], &bmp->pdata[plane][bitmap_offset], (bmp->sprite_width >> 2));
			bitmap_offset+=bmp->sprite_width>>2;
			screen_offset+=screen_width>>2;
		}
	}
}

static void VGA_Draw_static_bitmap_normal(BITMAP *bmp, short x, short y)
{
	memcpy(doublebuffer, (byte far*) bmp->data, (bmp->width*bmp->height));
}

static void VGA_Draw_sprite_planar_notrans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	int j,plane;
	word screen_offset;
	word bitmap_offset;
	word sprite_offset;
	
	sprite_offset=bmp->sprite_width*frame;
	if ((sprite_offset%bmp->width)==0)
		sprite_offset+=bmp->width*(bmp->sprite_height+1);

	for(plane=0; plane<4; plane++)
	{
		outp(SC_INDEX, MAP_MASK);          /* select plane */
		outp(SC_DATA,  1 << ((plane+x)&3) );
		screen_offset = ((dword)y*screen_width+x+plane) >> 2;
		for(j=0; j<bmp->height; j++)
		{
			memcpy(&VGA[quarter_scr+screen_offset], &bmp->pdata[plane][sprite_offset+bitmap_offset], (bmp->sprite_width >> 2));
			bitmap_offset+=bmp->sprite_width>>2;
			screen_offset+=screen_width>>2;
		}
	}
}

static void VGA_Draw_sprite_planar_trans(BITMAP *bmp, short x2, short y2, unsigned char frame)
{
	short x, y;	
	unsigned char plane;
	word screen_offset, bitmap_offset;

	bitmap_offset=bmp->sprite_width*frame;
	if ((bitmap_offset%bmp->width)==0)
		bitmap_offset+=bmp->width*(bmp->sprite_height+1);
		
	outp(SC_INDEX, MAP_MASK);
	
	screen_offset = (y2<<6) + (y2<<4) + (x2>>2);
	/* draw the object. */
	for(plane=0;plane<4;plane++)
	{
        /* select plane */
		outp(SC_DATA,  1 << ((plane+x2)&3) );
		for(y=0;y<bmp->sprite_height*bmp->width;y+=bmp->width)
		{
			for(x=plane;x<bmp->sprite_width;x+=4)
			{
				if (bmp->pdata[plane][bitmap_offset+y+x]!=0)
				{
					VGA[quarter_scr+screen_offset+(y>>2)+((x+(x2&3)) >> 2)]= bmp->pdata[plane][bitmap_offset+y+x];
				}
			}
		}
	}
}

static void VGA_Draw_sprite_normal_trans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	short i, j;
	//word screen_offset = (y<<8)+(y<<6) + x;	
	word screen_offset = x + (y * screen_width);
	word bitmap_offset = 0;
	byte data;
	unsigned short sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

	for(j=0;j<bmp->height;j++)
	{
		for(i=0;i<bmp->width;i++,bitmap_offset++)
		{
			data = bmp->data[bitmap_offset+sprite_offset];
			if (data) doublebuffer[screen_offset+x+i] = data;
		}
		screen_offset+=screen_width;
	}
}

static void VGA_Draw_sprite_normal_notrans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	short i, j;
	//word screen_offset = (y<<8)+(y<<6) + x;	
	word screen_offset = x + (y * screen_width);
	word bitmap_offset = 0;
	unsigned short sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

	for(j=0;j<bmp->height;j++)
	{
		for(i=0;i<bmp->width;i++,bitmap_offset++)
		{
			doublebuffer[screen_offset+x+i] = bmp->data[bitmap_offset+sprite_offset];
		}
		screen_offset+=screen_width;
	}
}

static void VGA_Load_static_bmp_planar(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height)
{
	FILE *fp;
	int32_t index, size;
	word num_colors;
	unsigned long x;
	unsigned char plane;

	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n", file);
		exit(1);
	}

  /* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}
  
	/* read in the width and height of the image, and the
	number of colors used; ignore the rest */
	fskip(fp,16);
	fread(&b->width, sizeof(word), 1, fp);
	fskip(fp,2);
	fread(&b->height,sizeof(word), 1, fp);
	fskip(fp,22);
	fread(&num_colors,sizeof(word), 1, fp);
	fskip(fp,6);
  
	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;

	size=b->width*b->height;
	
	/* try to allocate memory */
	for(plane=0;plane<4;plane++)
	{
		if ((b->pdata[plane] = (byte *) malloc((word)(size>>2))) == NULL)
		{
			fclose(fp);
			printf("Error allocating memory for file %s.\n", file);
			exit(1);
		}
	}

	fskip(fp, num_colors*4);

	/* read the bitmap */
	for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
	{
		for(x = 0; x < b->width; x++)
		{
			b->pdata[x&3][(int)((index+x)>>2)]=(byte)fgetc(fp);
		}
	}
	fclose(fp);
	
	if (s_width == 0) s_width = b->width;
	if (s_height == 0) s_height = b->height;
	
	b->sprite_width = s_width;
	b->sprite_height = s_height;
}

static void VGA_Load_bmp(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height)
{
	FILE *fp;
	long long index;
	word num_colors;
	long x;
  
	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}
  
	/* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}
	
	/* read in the width and height of the image, and the
	number of colors used; ignore the rest */
	fskip(fp,16);
	fread(&b->width, sizeof(word), 1, fp);
	fskip(fp,2);
	fread(&b->height,sizeof(word), 1, fp);
	fskip(fp,22);
	fread(&num_colors,sizeof(word), 1, fp);
	fskip(fp,6);

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;

	/* try to allocate memory */
	if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
	{
		fclose(fp);
		printf("Error allocating memory for file %s.\n",file);
		exit(1);
	}
  
	fskip(fp,num_colors*4);
	
	/* read the bitmap */
	for(index = (b->height-1)*b->width; index >= 0;index-=b->width)
	{
		for(x = 0; x < b->width; x++)
		{
			b->data[(int)((index+x))]=(byte)fgetc(fp);
		}
	}
	fclose(fp);
  
	if (s_width == 0) s_width = b->width;
	if (s_height == 0) s_height = b->height;
  
	b->sprite_width = s_width;
	b->sprite_height = s_height;
}

static void VGA_Load_palette(const char *file)
{
	FILE *fp;
	long index;
	word num_colors = 256;
  
	/* open the file */
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}
	/* check to see if it is a valid bitmap file */
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}

	fskip(fp,52);

	/* assume we are working with an 8-bit file */
	if (num_colors==0) num_colors=256;
  
  /* read the palette information */
	for(index=0;index<num_colors;index++)
	{
		VGA_8158_GAMEPAL[(int)(index*3+2)] = fgetc(fp) >> 2;
		VGA_8158_GAMEPAL[(int)(index*3+1)] = fgetc(fp) >> 2;
		VGA_8158_GAMEPAL[(int)(index*3+0)] = fgetc(fp) >> 2;
		fgetc(fp);
	}
	
	fclose(fp);
}

static void VGA_Set_palette()
{
  unsigned short i;
  outp(PALETTE_INDEX,0);	/* tell the VGA that palette data is coming. */
  for(i=0;i<256*3;i++)
	outp(PALETTE_DATA,VGA_8158_GAMEPAL[i]);    /* write the data */
}

static void VGA_Wait_for_retrace(void) 
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;
}

static void VGA_Wait_for_retrace_planar(void) 
{
}

static void VGA_Free_bmp_planar(BITMAP bmp)
{
	unsigned char plane;
	for(plane=0;plane<4;plane++)
	{
		free(bmp.pdata[plane]);
	}
}

static void VGA_Free_bmp(BITMAP bmp)
{
	if (bmp.data)
	{
		free(bmp.data);
		bmp.data = NULL;
	}
}

static void VGA_Clear_Screen_planar()
{
	/*unsigned char plane;
	outp(SC_INDEX, MAP_MASK);    
	for(plane=0;plane<4;plane++)
	{
		outp(SC_DATA,  1 << ((plane)&3) );
		memset(double_buffer[plane], 0, quarter_scr);
		memcpy(VGA,double_buffer[plane],quarter_scr);
	}*/
	outpw(SC_INDEX,ALL_PLANES);
	memset(&VGA[quarter_scr],0,quarter_scr);
}

static void VGA_Clear_Screen()
{
	memset(VGA, 0, (screen_width*screen_height));
}


static void page_flip(word *page1,word *page2)
{
	word high_address,low_address;
	word temp;

	temp=*page1;
	*page1=*page2;
	*page2=temp;

	high_address = HIGH_ADDRESS | (*page1 & 0xff00);
	low_address  = LOW_ADDRESS  | (*page1 << 8);

	//#ifdef VERTICAL_RETRACE
	while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
	//#endif
	outpw(CRTC_INDEX, high_address);
	outpw(CRTC_INDEX, low_address);
	//#ifdef VERTICAL_RETRACE
	while (!(inp(INPUT_STATUS_1) & VRETRACE));
	//#endif
}

static void VGA_Flip_planar()
{
	word active_page;
	active_page = quarter_scr;
	page_flip(&visual_page,&active_page);
}

static void VGA_Flip()
{
	memcpy(VGA, doublebuffer, SCREEN_SIZE);
}

static void VGA_Kill()
{
	unsigned char i;
	if (doublebuffer)
	{
		free(doublebuffer);
		doublebuffer = NULL;
	}
	set_mode(TEXT_MODE);
	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
}

static void VGA_Print_Text(char* text, short x, short y)
{
	
}


VideoDevice Planar_VGA = {
	"VGA Planar video",
	VGA_SetVideo_Planar,
	VGA_Draw_static_bitmap_planar,
	VGA_Draw_sprite_planar_trans,
	VGA_Draw_sprite_planar_notrans,
	VGA_Load_static_bmp_planar,
	VGA_Load_palette,
	VGA_Set_palette,
	VGA_Free_bmp_planar,
	VGA_Clear_Screen_planar,
	VGA_Flip_planar,
	VGA_Wait_for_retrace_planar,
	VGA_Kill,
	VGA_Print_Text
};

VideoDevice Normal_VGA = {
	"VGA Normal video",
	VGA_SetVideo,
	VGA_Draw_static_bitmap_normal,
	VGA_Draw_sprite_normal_trans,
	VGA_Draw_sprite_normal_notrans,
	VGA_Load_bmp,
	VGA_Load_palette,
	VGA_Set_palette,
	VGA_Free_bmp,
	VGA_Clear_Screen,
	VGA_Flip,
	VGA_Wait_for_retrace,
	VGA_Kill,
	VGA_Print_Text
};

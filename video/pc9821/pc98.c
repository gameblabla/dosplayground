//#ifdef DJGPP

#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>
#ifdef DJGPP
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
 #endif
 
#include "generic.h"
#include "graph.h"
#include "pc98.h"

static int pegc_fb_location = PEGC_FB_LOCATION_LOW;
#ifdef DJGPP
static __dpmi_meminfo	vram_dpmi;			// DPMI descriptor for far-memory location of framebuffer
#endif
static int vram_dpmi_selector;	// Memory region selector handle

static unsigned long SCREEN_SIZE;
static unsigned short quarter_scr;

static unsigned char doublebuffer[(GFX_ROW_SIZE * GFX_COL_SIZE)];

static char *removestr(char* myStr)
{
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

static void pal_Set(uint16_t idx, uint8_t r, uint8_t g, uint8_t b)
{
	outportb(PEGC_PALLETE_SEL_ADDR, idx);
	outportb(PEGC_RED_ADDR, r);
	outportb(PEGC_GREEN_ADDR, g);
	outportb(PEGC_BLUE_ADDR, b);
	return;
}

static int_fast8_t gfx_HasMemoryHole()
{
	// Checks if the memory hole is set at 16MB to enable us to set the VRAM framebuffer
	uint_fast8_t x;
	x = inportb(MEMORY_HOLE_CHECK_ADDR);

	if (x & 0x04)
	{
		// Memory hole not present
		return -1;
	}
	// If not, then Memory hole present
	return 0;	
}

static int_fast8_t gfx_DPMI()
{
	// Enable DPMI mapper to framebuffer far location
	if (gfx_HasMemoryHole() < 0){
		/* Memory hole not available, relocate VRAM address */
		pegc_fb_location = PEGC_FB_LOCATION_HIGH;
	} else {
		/* Memory hole available */
		pegc_fb_location = PEGC_FB_LOCATION_LOW;
	}
	
	#ifdef DJGPP
	if (!__djgpp_nearptr_enable()){
		return -1;
	}
	
	vram_dpmi.address = pegc_fb_location;
	vram_dpmi.size    = PEGC_FB_SIZE;
	
	if (__dpmi_physical_address_mapping(&vram_dpmi) != 0){
		return -1;
	}
	vram_dpmi_selector = __dpmi_allocate_ldt_descriptors(1);
	if (vram_dpmi_selector < 0){
		__dpmi_free_physical_address_mapping(&vram_dpmi);
		return -1;
	}
	__dpmi_set_segment_base_address(vram_dpmi_selector, vram_dpmi.address);
	__dpmi_set_segment_limit(vram_dpmi_selector, vram_dpmi.size - 1);
	#endif

	return 0;
}

void gfx_TextOn(){
	// Text mode on
	outportb(0x62, GDC_COMMAND_START);
}

void gfx_TextOff(){
	// Text mode off
	outportb(0x62, GDC_COMMAND_STOP1);
}

static void PC9821_SetVideo(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	// Initialise graphics to a set of configured defaults
	int_fast8_t status;
	
	// Text mode off
	outportb(0x62, GDC_COMMAND_STOP1);
	
	screen_width = width;
	screen_height = height;
	SCREEN_SIZE = screen_width * screen_height;
	quarter_scr = SCREEN_SIZE / 4;
		
	// HSYNC 24 KHz (640x400)
	outportb(PEGC_SCANFREQ_ADDR, PEGC_SCANFREQ_24);

	// 256 color mode
	outportb(PEGC_MODE_ADDR, 0x07);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_256c);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_SINGLE_PAGE);
	outportb(PEGC_MODE_ADDR, 0x06);

	// Enable Packed Pixel mode
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_PIXELMODE_ADDR, PEGC_PIXELMODE_PACKED);
	#endif

	// Set up DPMI mapper for vram framebuffer regionbui
	status = gfx_DPMI();
	if (status < 0){
		return;	
	}
	
	// Enable linear framebuffer at 16MB and 4095MB
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_ON);
	#endif

	// Set screen 0 to be active for drawing and active for display
	outportb(PEGC_DRAW_SCREEN_SEL_ADDR, 0x00);
	outportb(PEGC_DISP_SCREEN_SEL_ADDR, 0x00);
	
	// Graphics mode on
	outportb(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_START);
	
	// Set local vram_buffer to empty 
	//memset(vram_buffer, 0, (GFX_ROW_SIZE * GFX_COL_SIZE));
	//gfx_Clear();
	//gfx_Flip();
}


static void PC9821_Draw_static_bitmap_normal(BITMAP *bmp, short x, short y)
{
	memcpy(doublebuffer, (byte far*) bmp->data, (bmp->width*bmp->height));
}

static void PC9821_Draw_sprite_normal_trans(BITMAP *bmp, short x, short y, unsigned char frame)
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

static void PC9821_Draw_sprite_normal_notrans(BITMAP *bmp, short x, short y, unsigned char frame)
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


static void PC9821_Set_palette()
{
	unsigned short i;
	for(i=0;i<256;i++)
	{
		pal_Set(i, VGA_8158_GAMEPAL[(i*3)+0], VGA_8158_GAMEPAL[(i*3)+1], VGA_8158_GAMEPAL[(i*3)+2]);
	}
}

static void PC9821_Load_palette(const char *file)
{
	FILE *fp;
	uint16_t index;
	
	fp = fopen(file, "rb");
	for(index=0;index<256;index++)
	{
		VGA_8158_GAMEPAL[(index*3)+0] = fgetc(fp);
		VGA_8158_GAMEPAL[(index*3)+1] = fgetc(fp);
		VGA_8158_GAMEPAL[(index*3)+2] = fgetc(fp);
	}
	PC9821_Set_palette();
	fclose(fp);
}

static void PC9821_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char load_pal)
{
	FILE* fp;
	char str[128];

	fp = fopen(file, "rb");
	
	fread (&b->width, 1, sizeof(uint16_t), fp);
	fread (&b->height, 1, sizeof(uint16_t), fp);
	fread (&b->sprite_width, 1, sizeof(uint16_t), fp);
	fread (&b->sprite_height, 1, sizeof(uint16_t), fp);
	
	// Jump right after the end of the header
	fseek(fp, BITMAP_HEADER_SIZE, SEEK_SET);

	//switch(bmp.encoding)
	{
		//default:
			b->data = malloc((b->width * b->height));
			fread (b->data, 1, (b->width * b->height), fp);
		//break;
	}
	
	if (s_width == 0) s_width = b->width;
	if (s_height == 0) s_height = b->height;
  
	b->sprite_width = s_width;
	b->sprite_height = s_height;

	fclose(fp);
	
	if (load_pal == 1)
	{
		sprintf(str, "%s.PAL", removestr(basename(file)));
		PC9821_Load_palette(str);
	}
}

static void PC9821_Wait_for_retrace(void) 
{
	/* wait until done with vertical retrace */
	while ((inp(INPUT_STATUS) & VRETRACE)) ;
	/* wait until done refreshing */
	while (!(inp(INPUT_STATUS) & VRETRACE)) ;
}

static void PC9821_Free_bmp(BITMAP bmp)
{
	if (bmp.data)
	{
		free(bmp.data);
		bmp.data = NULL;
	}
}

static void PC9821_Clear_Screen()
{
	memset(doublebuffer, 0, (screen_width*screen_height));
}


static void PC9821_Flip()
{
	// Copy a buffer of GFX_ROWS * GFX_COLS bytes to
	// the active VRAM framebuffer for display.
	#ifdef DJGPP
	movedata(_my_ds(), doublebuffer, vram_dpmi_selector, 0, (GFX_ROWS * GFX_COLS));
	#else
	memcmy(pegc_fb_location, doublebuffer, GFX_ROWS * GFX_COLS);
	#endif
}

static void PC9821_Kill()
{
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_OFF);
	#endif

	// 16 Color mode
	outportb(PEGC_MODE_ADDR, 0x07);
	outportb(PEGC_MODE_ADDR, PEGC_BPPMODE_16c);
	outportb(PEGC_MODE_ADDR, 0x06);

	// Graphics mode off
	outportb(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_STOP1);
	
	#ifdef DJGPP
	// Free DPMI mapping
	__dpmi_free_physical_address_mapping(&vram_dpmi);
	#endif
	
	//  Clear anything we did to the screen
	PC9821_Clear_Screen();
	
	// Text mode on
	outportb(0x62, GDC_COMMAND_START);

	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
}

static void PC9821_Print_Text(char* text, short x, short y)
{
	
}


VideoDevice Normal_PC9821_PGC = {
	"PC9821 PGC video",
	PC9821_SetVideo,
	PC9821_Draw_static_bitmap_normal,
	PC9821_Draw_sprite_normal_trans,
	PC9821_Draw_sprite_normal_notrans,
	PC9821_Load_SIF,
	PC9821_Load_palette,
	PC9821_Set_palette,
	PC9821_Free_bmp,
	PC9821_Clear_Screen,
	PC9821_Flip,
	PC9821_Wait_for_retrace,
	PC9821_Kill,
	PC9821_Print_Text
};

//#endif

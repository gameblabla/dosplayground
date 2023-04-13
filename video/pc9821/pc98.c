#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#ifdef DJGPP
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#endif
 
#include "graph.h"
#include "pc98.h"
#include "gdc.h"

uint8_t palette_pc9821[769];
extern uint16_t screen_width, screen_height;

#ifdef DJGPP
// Doing so greatly reduces size
#include <crt0.h>
char **__crt0_glob_function (char *arg) { return 0; }
void   __crt0_load_environment_file (char *progname) { }
void   __crt0_setup_arguments (void) { }
#endif

static int pegc_fb_location = PEGC_FB_LOCATION_LOW;
#ifdef DJGPP
static __dpmi_meminfo	vram_dpmi;			// DPMI descriptor for far-memory location of framebuffer
#endif
static int vram_dpmi_selector;	// Memory region selector handle

static unsigned long SCREEN_SIZE;
static unsigned short quarter_scr;

static unsigned char* doublebuffer;

extern uint32_t apl_decompress(const void *Source, void *Destination);

#ifdef DJGPP
#define OUTPORT_DOS outportb
#define INPORT_DOS inportb
#else
#define OUTPORT_DOS _outp
#define INPORT_DOS _inp
#endif

#define ENABLE_NEAR() __djgpp_nearptr_enable();
#define DISABLE_NEAR() __djgpp_nearptr_disable();

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
	OUTPORT_DOS(PEGC_PALLETE_SEL_ADDR, idx);
	OUTPORT_DOS(PEGC_RED_ADDR, r);
	OUTPORT_DOS(PEGC_GREEN_ADDR, g);
	OUTPORT_DOS(PEGC_BLUE_ADDR, b);
	return;
}

static int_fast8_t gfx_HasMemoryHole()
{
	// Checks if the memory hole is set at 16MB to enable us to set the VRAM framebuffer
	uint_fast8_t x;
	x = INPORT_DOS(MEMORY_HOLE_CHECK_ADDR);

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
		__djgpp_nearptr_disable();
		return -1;
	}
	vram_dpmi_selector = __dpmi_allocate_ldt_descriptors(1);
	if (vram_dpmi_selector < 0){
		__dpmi_free_physical_address_mapping(&vram_dpmi);
		__djgpp_nearptr_disable();
		return -1;
	}
	__dpmi_set_segment_base_address(vram_dpmi_selector, vram_dpmi.address);
	__dpmi_set_segment_limit(vram_dpmi_selector, vram_dpmi.size - 1);
	
	__djgpp_nearptr_disable();
	#endif
	
	return 0;
}

void gfx_TextOn(){
	// Text mode on
	OUTPORT_DOS(0x62, GDC_COMMAND_START);
}

void gfx_TextOff(){
	// Text mode off
	OUTPORT_DOS(0x62, GDC_COMMAND_STOP1);
}


static void Print_text(const char *str)
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 0;
	while(*str) {
		regs.h.dl = *(str++);
		int86(0xDC, &regs, &regs);
	}
	outp(0x64, 0); // VSync interrupt trigger	
}

static void ClearTextVRAM()
{
    unsigned long base_address = 0xA0000;
    unsigned char buffer[16384] = {0};
    dosmemput(buffer, sizeof(buffer), base_address);
}


static void PC9821_SetVideo(unsigned short width, unsigned short height, unsigned short flags, int argc, char** argv)
{
	// Initialise graphics to a set of configured defaults
	
	// Text mode off
	//OUTPORT_DOS(0x62, GDC_COMMAND_STOP1);
	
	// Clear text layer
	ClearTextVRAM();
	
	// Print command to disable text cursor
	Print_text("\x1B[>5h");
	
	// Print command to disable system line
	Print_text("\x1B[>1h");

	screen_width = width;
	screen_height = height;
	SCREEN_SIZE = screen_width * screen_height;
	quarter_scr = SCREEN_SIZE / 4;
		
	// HSYNC 24 KHz (640x400)
	OUTPORT_DOS(PEGC_SCANFREQ_ADDR, PEGC_SCANFREQ_24);

	// 256 color mode
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x07);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_256c);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_SINGLE_PAGE);
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x06);
	
	doublebuffer = malloc (GFX_COL_SIZE * GFX_ROW_SIZE);

	// Enable Packed Pixel mode
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_PIXELMODE_ADDR, PEGC_PIXELMODE_PACKED);
	#endif

	// Set up DPMI mapper for vram framebuffer regionbui
	if (gfx_DPMI() < 0){
		return;	
	}
	
	// Enable linear framebuffer at 16MB and 4095MB
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_ON);
	#endif

	// Set screen 0 to be active for drawing and active for display
	OUTPORT_DOS(PEGC_DRAW_SCREEN_SEL_ADDR, 0x00);
	OUTPORT_DOS(PEGC_DISP_SCREEN_SEL_ADDR, 0x00);
	
	// Graphics mode on
	OUTPORT_DOS(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_START);
	
	// Set local vram_buffer to empty 
	//memset(vram_buffer, 0, (GFX_ROW_SIZE * GFX_COL_SIZE));
	//gfx_Clear();
	//gfx_Flip();
	
	return;
}


static void PC9821_Draw_static_bitmap_normal(BITMAP *bmp, short x, short y, unsigned long offset)
{
	memcpy(doublebuffer, (uint8_t far*) bmp->data + offset, (bmp->width*bmp->height));
}

static void PC9821_Draw_sprite_normal_trans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	unsigned short i, j;
	//uint16_t screen_offset = (y<<8)+(y<<6) + x;	
	uint16_t screen_offset = x + (y * screen_width);
	uint16_t bitmap_offset = 0;
	uint8_t data;
	unsigned long sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

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
	short j;
	//uint16_t screen_offset = (y<<8)+(y<<6) + x;	
	uint16_t screen_offset = x + (y * screen_width);
	uint16_t bitmap_offset = 0;
	unsigned long sprite_offset = frame*(bmp->sprite_width*bmp->sprite_height);

	for(j=0;j<bmp->height;j++)
	{
        memcpy(&doublebuffer[screen_offset], &bmp->data[bitmap_offset + sprite_offset], bmp->width * sizeof(unsigned char));
        bitmap_offset += bmp->width;
        screen_offset += screen_width;
	}
}


static void PC9821_Set_palette()
{
	unsigned short i;
	for(i=0;i<256;i++)
	{
		pal_Set(i, palette_pc9821[(i*3)+0], palette_pc9821[(i*3)+1], palette_pc9821[(i*3)+2]);
	}
}

static void PC9821_Load_palette(const char *file)
{
	int fd;
	unsigned totalsize;
	_dos_open(file, O_RDONLY, &fd);
	_dos_read(fd, (char*)&palette_pc9821, sizeof(palette_pc9821), &totalsize);
	_dos_close(fd);
}



static void PC9821_Load_SIF(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height, unsigned char load_pal)
{
    int fd;
    char str[32];
    unsigned char* temp_mem;
    unsigned totalsize;

    _dos_open(file, O_RDONLY, &fd);

    _dos_read(fd, &b->width, sizeof(uint16_t), &totalsize);
    _dos_read(fd, &b->height, sizeof(uint16_t), &totalsize);
    _dos_read(fd, &b->sprite_width, sizeof(uint16_t), &totalsize);
    _dos_read(fd, &b->sprite_height, sizeof(uint16_t), &totalsize);

    _dos_read(fd, (char*)&b->bytespp, sizeof(uint8_t), &totalsize);
    _dos_read(fd, (char*)&b->encoding, sizeof(uint8_t), &totalsize);

#ifdef DJGPP
   	lseek (fd, BITMAP_HEADER_SIZE, SEEK_SET);
#else
	_dos_seek (fd, BITMAP_HEADER_SIZE, SEEK_SET);
#endif

    switch(b->encoding)
    {
        default:
            b->data = malloc((b->width * b->height));
            _dos_read(fd, b->data, (b->width * b->height), &totalsize);
        break;
        case APLIB_COMPRESSION_ID:
            temp_mem = malloc((b->width * b->height));
            b->data = malloc((b->width * b->height));

            _dos_read(fd, temp_mem, (b->width * b->height), &totalsize);

            apl_decompress(temp_mem,b->data);

            free(temp_mem);
        break;
    }

    if (s_width == 0) s_width = b->width;
    if (s_height == 0) s_height = b->height;

    b->sprite_width = s_width;
    b->sprite_height = s_height;

    _dos_close(fd);

    if (load_pal == 1)
    {
        strcpy(str, removestr(basename(file)));
        strcat(str, ".PAL");
        PC9821_Load_palette(str);
    }
}

static void PC9821_Wait_for_retrace(void) 
{
	/* Wait until vsync occurs. */
	while (!(inp(GDC_GFX_STATUS) & GDC_STATUS_VSYNC)) {}
	
	/* Now wait until it's over. */
	while (inp(GDC_GFX_STATUS)   & GDC_STATUS_VSYNC) {}
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
	ClearTextVRAM();
}


static void PC9821_Flip()
{
	// Copy a buffer of GFX_ROWS * GFX_COLS bytes to
	// the active VRAM framebuffer for display.
	#ifdef DJGPP
	movedata(_my_ds(), (unsigned)doublebuffer, vram_dpmi_selector, 0, (GFX_ROWS * GFX_COLS));
	#else
	memcpy(pegc_fb_location, doublebuffer, GFX_ROWS * GFX_COLS);
	#endif
}

static void PC9821_Kill()
{
	#ifdef DJGPP
	_farpokeb(_dos_ds, PEGC_FB_CONTROL_ADDR, PEGC_FB_OFF);
	#endif

	// 16 Color mode
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x07);
	OUTPORT_DOS(PEGC_MODE_ADDR, PEGC_BPPMODE_16c);
	OUTPORT_DOS(PEGC_MODE_ADDR, 0x06);

	// Graphics mode off
	OUTPORT_DOS(PEGC_GDC_COMMAND_ADDR, GDC_COMMAND_STOP1);
	
	#ifdef DJGPP
	// Free DPMI mapping
	__dpmi_free_physical_address_mapping(&vram_dpmi);
	#endif
	
	//  Clear anything we did to the screen
	PC9821_Clear_Screen();
	
	// Text mode on
	OUTPORT_DOS(0x62, GDC_COMMAND_START);

	#ifdef DJGPP
	__djgpp_nearptr_disable();
	#endif
	
	free(doublebuffer);
	
	// Print command to enable text cursor
	Print_text("\x1B[>5l");
	// Print command to enable system line
	Print_text("\x1B[>1l");
}

static void Cursor_SetXY(unsigned char x, unsigned char y) 
{
	union REGS regs;
	regs.h.cl = 0x10;
	regs.h.ah = 3;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0xDC, &regs, &regs);
}


static void PC9821_Print_Text(char* text, short x, short y)
{
	Cursor_SetXY(x, y);
	Print_text(text);
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

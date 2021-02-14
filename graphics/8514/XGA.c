
    /********************************************************/
    /*                                                      */
    /*                                                      */
    /*            XGA Adapter Demonstration Program         */
    /*                                                      */
    /*     Compile this module, then link to CALLAFI.OBJ    */
    /*                                                      */
    /*               Version 1.01  7/30/91                  */
    /*                                                      */
    /********************************************************/



/**************************************************/
/*                                                */
/*          Include files                         */
/*                                                */
/**************************************************/

#include "generic.h"
#include "generic_io.h"
#include "ibmafi.h"
#include "graph.h"
#include "generic.h"

static char xga_path[ 63 ];
static char file_path[ 63 ];
static char far *state_buf;
static int cell_height;

#define   DEMO_LOGO_MSG     0
#define   NO_ADAPTER_MSG    1
#define   OPEN_FAILED_MSG   2
#define   NO_FONT_MSG       3

static const HQDPS_DATA    hqdps_data     = { 14 };             	/* Query state        */
static HSPAL_DATA   *save_pal_data;                      	/* save palette       */
static const HOPEN_DATA    hopen_data     = { 2, 0, 1, 0 };     	/* open adapter       */
static const HOPEN_DATA    hopen_data_1024     = { 2, 0, 0, 0 }; // 1024x768
static const HCLOSE_DATA   hclose_data    = { 1, 0 };           	/* close adapter      */
static HINIT_DATA    hinit_data     = { 2, 0 };           	/* initialise adapter */
static const HQMODE_DATA   hqmode_data    = { 18 };             	/* query mode         */
static const HEAR_DATA     hear_data      = { 1, 0 };           	/* end area           */
static HSCOL_DATA    hscol_data     = { 4, 0 };           	/* set colour         */
static const HINT_DATA     hint_data      = { 4, 0x80000000 };  	/* wait for event     */
static HSCS_DATA     hscs_data      = { 4, 0 };           	/* set character set  */
static HCHST_DATA( 128 ) hchst_data;                      	/* write string       */
static HSPAL_DATA  *real_pal;


static struct CharSetDef *  load_font( font_file )

    char  * font_file;
{
    int           font_fid;
    static char   font_path[ 120 ];
    word          font_file_len;
    struct FontFileDefn * ffd_ptr;
    struct CharSetDef   * csd_ptr = NULL;

    /*----------------------------------------------------------*/

    /* Open font file as binary, read only                      */

    font_fid = open( font_file,  O_RDONLY | O_BINARY );


    if ( font_fid == -1 )
    {
        /* Can't find file in current directory so try xga path. */

        strcpy( font_path, xga_path );
        strcat( font_path, font_file );

        font_fid = open( font_path, O_RDONLY | O_BINARY );

        if ( font_fid == -1 )
        {
            /* Can't find file in xga path, so try \XGAPCDOS.    */

            strcpy( font_path, "\\XGAPCDOS\\" );
            strcat( font_path, font_file );

            font_fid = open( font_path, O_RDONLY | O_BINARY );
        }
    }

    if ( font_fid != -1 )
    {
        /* Calculate length of font file                            */

        font_file_len = ( word ) lseek( font_fid, 0L, SEEK_END );

        ffd_ptr = ( struct FontFileDefn * ) calloc( font_file_len, 1 );


        /* read font file into memory                               */

        lseek( font_fid, 0L, SEEK_SET );

        read( font_fid, ( char * ) ffd_ptr, font_file_len );


        /* Set up pointer to character set definition.        */

        csd_ptr = ( struct CharSetDef * ) ( ( char * ) ffd_ptr  +
                            ffd_ptr->page_array[ ffd_ptr->def_page ].csd_offset );


        /* Set up internal csd pointers                       */

        csd_ptr->chardef1 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef1;
        csd_ptr->chardef2 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef2;
        csd_ptr->chardef3 = ( byte far * ) ffd_ptr +  ( long )csd_ptr->chardef3;

        csd_ptr->indextbl = ( word far * ) ( ( byte far * )ffd_ptr  +
                                                ( long )csd_ptr->indextbl );

        csd_ptr->enveltbl = ( ( byte far * )ffd_ptr  + ( long )csd_ptr->enveltbl );


        /* Finished with font file. */

        close( font_fid );

        cell_height = csd_ptr->cellheight;
    }

    return csd_ptr;
}

static void error_exit( int msg_id, int error_num )
{
    HCLOSE( &hclose_data );
    HRPAL( save_pal_data );
    exit(1);
}

static void XGA_Load_Font()
{
	char font_file[13];
	/* Select a font depending on character size for mode       */
	switch ( hqmode_data.ac_h )
	{
		case 14:
			sprintf(font_file, "stan0814.fnt");
		break;
		case 15:
			sprintf(font_file, "stan0715.fnt");
		break;
		case 20:
			sprintf(font_file, "stan1220.fnt");
		break;
		case 23:
			sprintf(font_file, "stan1223.fnt");
		break;
	}
     /* read font definition from file, then load adapter character set */
	if ( hscs_data.address = load_font( font_file ) )
	{
		HSCS( &hscs_data );
	}
}

static void XGA_Load_bmp(const char *file, BITMAP *b, unsigned short s_width, unsigned short s_height)
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
  
	b->sprite_width = s_width;
	b->sprite_height = s_height;
}


static void XGA_Load_palette(const char *file)
{
	FILE *fp;
	long index;
	int i;
	word num_colors = 256;
	unsigned char x;
  
	if ((fp = fopen(file,"rb")) == NULL)
	{
		printf("Error opening file %s.\n",file);
		exit(1);
	}
	
	if (fgetc(fp)!='B' || fgetc(fp)!='M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n",file);
		exit(1);
	}

	fskip(fp,52);
	
	for(index=0;index<256;index++)
	{
		VGA_8158_GAMEPAL[(int)(3+index*3+2)] = fgetc(fp) >> 2;
		VGA_8158_GAMEPAL[(int)(3+index*3+1)] = fgetc(fp) >> 2;
		VGA_8158_GAMEPAL[(int)(3+index*3+0)] = fgetc(fp) >> 2;
		x = fgetc(fp);
	}
	
	fclose(fp);
}

static void error_exit( int msg_id, int error_num );

static inline uint8_t getpixel(BITMAP *bmp, unsigned short x, unsigned short y)
{
    uint8_t *p = (uint8_t *)bmp->data + y * bmp->width + x;
	return *p;
}

static void XGA_Draw_static(BITMAP *bmp, short x, short y)
{
	//HRECT_DATA quad;
	
	HBBW_DATA img;
	HBBCHN_DATA img2;
	HBBC_DATA img3;
	
	HBAR();
	img.length = 10;
	img.format = 8;
	img.width = bmp->width;
	img.height = bmp->height;
	img.coord.x_coord = x;
	img.coord.y_coord = y;
	
	img2.length = 6;
	img2.address = bmp->data;
	img2.len = bmp->width * bmp->height;
	
	img3.length = 16;
	img3.format = 8;
	img3.width = bmp->width;
	img3.height = bmp->height;
	img3.plane = 0;
	img3.source.x_coord = 0;
	img3.source.y_coord = 0;
	img3.dest.x_coord = 0;
	img3.dest.y_coord = 0;
	
	HBBW(&img);
	HBBCHN(&img2);
	HBBC(&img3);
	
	HEAR(&hear_data);
}

/* Based upon my RLE decompressor :
 * https://github.com/gameblabla/rle_xga_sdl
 * 
 * In this case, it is done on the fly.
 * Reason as to why i'm doing this is because pixel drawing on the 8514
 * can be a bottleneck so we're using RLE as to reduce the GPU overhead.
 * 
*/
static void XGA_Draw_Sprite_RLE_trans(BITMAP* bmp, short x, short y, unsigned char frame)
{
	unsigned short i,a;
	unsigned char color_hold = 0;
	unsigned short inc = 0;
	unsigned short w, h;
	
	HRECT_DATA quad;
	HBAR();
	
	w = 0;
	h = 0;
	
	for(i=0;i<bmp->height;i++)
	{
		color_hold = getpixel(bmp, 0, i);
		inc = 1;
		// a is set to 1 since we are already reading the pixel before entering the loop
		// Also we use < instead of <= to avoid overdrawing especially after we write out the rest.
		for(a=1;a<bmp->width;a++)
		{
			if (color_hold != getpixel(bmp, a, i))
			{
				quad.coord.x_coord = x+w;
				quad.coord.y_coord = y+h;
				quad.width = inc;
				quad.height = 1;
				if (color_hold)
				{
					hscol_data.index = color_hold;
					HSCOL(&hscol_data);
					HRECT(&quad);
				}
				w += inc;
				if (w >= bmp->width)
				{
					h++;
					w = 0;
				}
				inc = 1;
			}
			else
			{
				inc++;
			}
			color_hold = getpixel(bmp, a, i);
		}
		
		quad.coord.x_coord = x+w;
		quad.coord.y_coord = y+h;
		quad.width = inc;
		quad.height = 1;
				
		if (color_hold)
		{
			hscol_data.index = color_hold;
			HSCOL(&hscol_data);
			HRECT(&quad);
		}
				
		w += inc;
		if (w >= bmp->width)
		{
			h++;
			w = 0;
		}
	}
}

/*
static void XGA_Draw_sprite_trans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	unsigned short a, i;
	unsigned short sprite_offset = frame * (bmp->sprite_width * bmp->sprite_height);
	HRECT_DATA quad;
	
	HBAR();
	for(i=0;i<bmp->width;i++)
	{
		for(a=0;a<bmp->height;a++)
		{
			quad.coord.x_coord = i+x;
			quad.coord.y_coord = a+y;
			quad.width = 1;
			quad.height = 1;
			if (bmp->data[sprite_offset + (i+(a*bmp->width))])
			{
				hscol_data.index = bmp->data[sprite_offset + (i+(a*bmp->width))];
				HSCOL(&hscol_data);
				HRECT(&quad);
			}
		}
	}
	HEAR(&hear_data);
}
*/

// This needs to be checked
static void XGA_Draw_sprite_notrans(BITMAP *bmp, short x, short y, unsigned char frame)
{
	//HRECT_DATA quad;
	
	HBBW_DATA img;
	HBBCHN_DATA img2;
	HBBC_DATA img3;
	
	HBAR();
	img.length = 10;
	img.format = 8;
	img.width = bmp->width;
	img.height = bmp->height;
	img.coord.x_coord = x;
	img.coord.y_coord = y;
	
	img2.length = 6;
	img2.address = bmp->data;
	img2.len = bmp->width * bmp->height;
	
	img3.length = 16;
	img3.format = 8;
	img3.width = bmp->width;
	img3.height = bmp->height;
	img3.plane = 0;
	img3.source.x_coord = 0;
	img3.source.y_coord = frame * bmp->sprite_width;
	img3.dest.x_coord = bmp->sprite_width;
	img3.dest.y_coord = bmp->sprite_height;
	
	HBBW(&img);
	HBBCHN(&img2);
	HBBC(&img3);
	
	HEAR(&hear_data);
}

static void XGA_Kill()
{
    HCLOSE( &hclose_data );
    HRPAL( save_pal_data );
	
    if (save_pal_data != NULL) free(save_pal_data);
    if (real_pal != NULL) free(real_pal);
    if (state_buf != NULL) free(state_buf);
}

static void XGA_Free_bitmap(BITMAP bmp)
{
	if (bmp.data)
	{
		free(bmp.data);
		bmp.data = NULL;
	}
}

static void get_xga_path( int argc, char ** argv )
{
    /* If a parameter is passed to this program, it should      */
    /* represent the directory where the XGA Adapter software   */
    /* was installed.                                           */
    if ( argc >= 2 )
    {
        strcpy( xga_path, argv[ 1 ] );
        if ( xga_path[ strlen( xga_path ) - 1 ] != '\\' )
            strcat( xga_path, "\\" );
    }
    else
    {
        strcpy( xga_path, argv[ 0 ] );
        *( strrchr( xga_path, '\\' ) + 1 ) = '\0';
    }
}

static void XGA_SetVideo(unsigned short width, unsigned short height, unsigned short planar, int argc, char** argv)
{
	real_pal = malloc (768);
	real_pal->length = 768;

	memcpy(real_pal, VGA_8158_GAMEPAL, 768);

    get_xga_path( argc, argv );
	
    if ( getafi() == NULL )
    {
		printf("No AFI Adapter detected !\n");
        error_exit( NO_ADAPTER_MSG, 0 );
	}
	else
	{
		printf("An AFI Adapter was detected !\n");
	}

    /* Allocate Adapter interface task dependent buffer         */
    HQDPS( &hqdps_data );
    state_buf =  malloc( hqdps_data.size + 15 );

    /* Allocate palette save restore buffer                     */
    save_pal_data =  malloc( hqdps_data.palbufsize + 2 );
    save_pal_data->length = hqdps_data.palbufsize;
    
    /* save palette                                             */
    HSPAL( save_pal_data );

    /* Attempt to open XGA adapter in mode 0 ( 1024 * 768 ). If the attached */
    /* monitor does not support this mode, or there is insufficient vram,    */
    /* then the XGA adapter may open in another mode.                        */
    
    if (width == 1024 && height == 768)
    {
		HOPEN( &hopen_data_1024 );
	}
	else
	{
		HOPEN( &hopen_data );
	}

    if ( hopen_data.iflags )
		error_exit( OPEN_FAILED_MSG, ( byte )hopen_data.iflags );

    /* Make sure that task dependent buffer is situated on a    */
    /*                                       16 byte boundary.  */

    hinit_data.segment = FP_SEG( state_buf ) + ( ( FP_OFF( state_buf ) + 15) >> 4 );

    /* initialise XGA adapter                                   */
    HINIT( &hinit_data );
    /* load palette for demo                                    */
	HRPAL( real_pal );
    /* Find out mode and associated data                        */
    HQMODE( &hqmode_data );
}

static void XGA_Set_Palette()
{
	memcpy(real_pal, VGA_8158_GAMEPAL, 768);
	HRPAL( real_pal );
}

static void XGA_Clear_Screen()
{
	HRECT_DATA quad;
	
	HBAR();
	quad.coord.x_coord = 0;
	quad.coord.y_coord = 0;
	quad.width = screen_width;
	quad.height = screen_height;
	hscol_data.index = 0;
	HSCOL(&hscol_data);
	HRECT(&quad);
	HEAR(&hear_data);
}

static void XGA_Flip()
{
	
}

static void XGA_Wait_for_retrace()
{
	
}

static void XGA_Print_Text(char* text, short x, short y)
{
	
}

VideoDevice Normal_8514 = {
	"8154/XGA video",
	XGA_SetVideo,
	XGA_Draw_static,
	XGA_Draw_Sprite_RLE_trans,
	XGA_Draw_sprite_notrans,
	XGA_Load_bmp,
	XGA_Load_palette,
	XGA_Set_Palette,
	XGA_Free_bitmap,
	XGA_Clear_Screen,
	XGA_Flip,
	XGA_Wait_for_retrace,
	XGA_Kill,
	XGA_Print_Text
};

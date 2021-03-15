/* (C) Copyright IBM Corporation 1986,1987,1988,1989,1990 ********************/
/*                                                                           */
/*              Program:      Definitions and declarations relevant to the   */
/*                            Adapter Interface                              */
/*                                                                           */
/*              File/module:  AFIDATA.H                                      */
/*                                                                           */
/*                            AI Base Function                               */
/*                            + Extended Function Sets (1) and (2)           */
/*                                                                           */
/*****************************************************************************/

#define HLINE_DATA(S) struct { word length; coord_pr coords[S]; }
/* usage: HLINE_DATA(size) will give the following structure            */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coords[size];                                              */
/*  }                                                                   */
/*                                                                      */

#define HCLINE_DATA(S) struct { word length; coord_pr coords[S]; }
/* usage: HCLINE_DATA(size) will give the following structure           */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coords[size];                                              */
/*  }                                                                   */
/*                                                                      */

#define HRLINE_DATA(S) struct { word length; coord_pr coord1; rcoord_pr rcoords[S]; }
/* usage: HRLINE_DATA(size) will give the following structure           */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord1;                                                    */
/*  rcoord_pr rcoords[size];                                            */
/*  }                                                                   */
/*                                                                      */

#define HCRLINE_DATA(S) struct { word length; rcoord_pr rcoords[S]; }
/* usage: HCRLINE_DATA(size) will give the following structure          */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  rcoord_pr rcoords[size];                                            */
/*  }                                                                   */
/*                                                                      */

#define HSCP_DATA struct { word length; coord_pr coord1; }
/* usage: HSCP_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord1;                                                    */
/*  }                                                                   */
/*                                                                      */

/* There is no HBAR data                                                */
/*                                                                      */

#define HEAR_DATA struct { word length; byte flags; }
/* usage: HEAR_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte flags;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HSCOL_DATA struct { word length; long index; }
/* usage: HSCOL_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  long index;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HOPEN_DATA struct { word length; byte oflags,mode,iflags; }
/* usage: HOPEN_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte oflags,                                                        */
/*       mode,                                                          */
/*       iflags;                                                        */
/*  }                                                                   */
/*                                                                      */
/*                                                                      */

#define HSMX_DATA struct { word length; byte foremix; byte backmix; }
/* usage: HSMX_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte foremix;                                                       */
/*  byte backmix;                                                       */
/*  }                                                                   */
/*                                                                      */
#define MIX_NOC       0x00      /* No change to mix     */
#define MIX_OR        0x01      /* OR                   */
#define MIX_OVER      0x02      /* Overpaint             */
#define MIX_XOR       0x04      /* Exclusive OR          */
#define MIX_LEAVE     0x05      /* Leave alone           */
#define MIX_ADD       0x08      /* Add colour to screen  */
#define MIX_SUB       0x09      /* screen - colour      */
#define MIX_MEAN      0x0B      /* average of screen & colour   */

#define HSBCOL_DATA struct { word length; long index; }
/* usage: HSBCOL_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  long index;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HSLT_DATA struct { word length; byte index, reserved; byte far *def; }
/* usage: HSLT_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte index;                                                         */
/*  byte reserved;                                                      */
/*  byte far *def;                                                      */
/*  }                                                                   */
/*                                                                      */

#define HSLW_DATA struct { word length; byte index; }
/* usage: HSLW_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte index;                                                         */
/*  }                                                                   */
/*                                                                      */

/* There is no HEGS data                                                */
/*                                                                      */


#define HSGQ_DATA struct { word length; word index; }
/* usage: HSGQ_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word index;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HSCMP_DATA struct { word length; long col; byte function; }
/* usage: HSCMP_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  long col;                                                           */
/*  byte function;                                                      */
/*  }                                                                   */
/*                                                                      */

#define HINT_DATA struct { word length; long eventid; }
/* usage: HINT_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  long eventid;                                                       */
/*  }                                                                   */
/*                                                                      */

#define HSPATTO_DATA struct { word length; coord_pr coord1; }
/* usage: HSPATTO_DATA will give the following structure                */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length,                                                        */
/*  coord_pr coord1;                                                    */
/*  }                                                                   */
/*                                                                      */

#define HSPATT_DATA struct { word length; byte width,height,flags,reserved; word img_length; byte far *image, far *colour; }
/* usage: HSPATT_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte width,                                                         */
/*       height,                                                        */
/*       flags,                                                         */
/*       reserved;                                                      */
/*  word img_length;                                                    */
/*  byte far *image,                                                    */
/*       far *colour;                                                   */
/*  }                                                                   */
/*                                                                      */

#define HLDPAL_DATA struct { word length; byte pal_id,res1;word first,count;byte far *address; }
/* usage: HLDPAL_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte pal_id,                                                        */
/*       res1;                                                          */
/*  word first,                                                         */
/*       count;                                                         */
/*  byte far *address;                                                  */
/*  }                                                                   */
/*                                                                      */

#define HSHS_DATA struct { word length;word l,r,b,t;byte far *address;byte flags;}
/* usage: HSHS_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word l,                                                             */
/*       r,                                                             */
/*       b,                                                             */
/*       t;                                                             */
/*  byte far *address;                                                  */
/*  byte flags;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HBBW_DATA struct { word length,format,width,height; coord_pr coord; word l,t,w,h; }
/* usage: HBBW_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length,                                                        */
/*      format,                                                         */
/*      width,                                                          */
/*      height,                                                         */
/*  coord_pr coord;                                                     */
/*  word l,                                                             */
/*       t,                                                             */
/*       w,                                                             */
/*       h;                                                             */
/*  }                                                                   */
/*                                                                      */

#define HCBBW_DATA struct { word length,format,width,height,l,t,w,h; }
/* usage: HCBBW_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length,                                                        */
/*      format,                                                         */
/*      width,                                                          */
/*      height,                                                         */
/*      l,                                                              */
/*      t,                                                              */
/*      w,                                                              */
/*      h;                                                              */
/*  }                                                                   */
/*                                                                      */

#define HBBR_DATA struct { word length,format,width,height; byte plane,reserved; coord_pr coord; word l,t,w,h;}
/* usage: HBBR_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length,                                                        */
/*      format,                                                         */
/*      width,                                                          */
/*      height,                                                         */
/*  byte plane,                                                         */
/*       reserved;                                                      */
/*  coord_pr coord;                                                     */
/*  word l,                                                             */
/*       t,                                                             */
/*       w,                                                             */
/*       h,                                                             */
/*  }                                                                   */
/*                                                                      */

#define HBBCHN_DATA struct { word length; byte far *address; word len; }
/* usage: HBBCHN_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte far *address;                                                  */
/*  word len;                                                           */
/*  }                                                                   */
/*                                                                      */

#define HBBC_DATA struct { word length,format,width,height; byte plane,reserved; coord_pr source,dest; }
/* usage: HBBC_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length,                                                        */
/*      format,                                                         */
/*      width,                                                          */
/*      height,                                                         */
/*  byte plane,                                                         */
/*       reserved;                                                      */
/*  coord_pr source,                                                    */
/*           dest;                                                      */
/*  }                                                                   */
/*                                                                      */

#define HSCOORD_DATA struct { word length; byte acformat,rcformat,ndimens; }
/* usage: HSCOORD_DATA will give the following structure                */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte acformat,                                                      */
/*       rcformat,                                                      */
/*       ndimens;                                                       */
/*  }                                                                   */
/*                                                                      */

#define HQCOORD_DATA struct { word length; byte acformat,rcformat,ndimens,result; }
/* usage: HQCOORD_DATA will give the following structure                */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte acformat,                                                      */
/*       rcformat,                                                      */
/*       ndimens,                                                       */
/*       result;                                                        */
/*  }                                                                   */
/*                                                                      */

#define HSMODE_DATA struct { word length; byte scformat,flags; }
/* usage: HSMODE_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte scformat,                                                      */
/*       flags;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HQMODE_DATA struct { word length; byte mode; byte clvl[2]; byte atype,dtype,ac_w,ac_h,nplanes;word width,height,h_pitch,v_pitch;byte moco,n_i_lvls,areafp,vga; }
/* usage: HQMODE_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte mode;                                                          */
/*  byte clvl[2];                                                       */
/*  byte atype,                                                         */
/*       dtype,                                                         */
/*       ac_w,                                                          */
/*       ac_h,                                                          */
/*       nplanes;                                                       */
/*  word width,                                                         */
/*       height,                                                        */
/*       h_pitch,                                                       */
/*       v_pitch;                                                       */
/*  byte moco,                                                          */
/*       n_i_lvls,                                                      */
/*       areafp,                                                        */
/*       vga;                                                           */
/*  }                                                                   */
/*                                                                      */

#define HQMODES_DATA struct { word length; byte atype,modes[32]; }
/* usage: HQMODES_DATA will give the following structure                */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte atype,                                                         */
/*       modes[32];                                                     */
/*  }                                                                   */
/*                                                                      */

#define HQDPS_DATA struct { word length; word size, stack, palbufsize; longword vramsize,areafpsize; }
/* usage: HQDPS_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word size,                                                          */
/*       stack,                                                         */
/*       palbufsize;                                                    */
/*  longword vramsize,                                                  */
/*           areafpsize,                                                */
/*  }                                                                   */
/*                                                                      */

#define HRECT_DATA struct { word length; coord_pr coord; word width,height; }
/* usage: HRECT_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  word width,                                                         */
/*       height;                                                        */
/*  }                                                                   */
/*                                                                      */

#define HRECTS_DATA(S) struct { word length; rect_def rect[S]; }
/* usage: HRECTS_DATA(size) will give the following structure           */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  rect_def rect[size];                                                */
/*  }                                                                   */
/*                                                                      */

#define HSBP_DATA struct { word length; longword grupdate, alupdate, display; byte flags, reserved; longword green_msk, red_msk, blue_msk; }
/* usage: HSBP_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  longword grupdate,                                                  */
/*           alupdate,                                                  */
/*           display;                                                   */
/*  byte flags,                                                         */
/*       reserved;                                                      */
/*  longword green_msk,                                                 */
/*           red_msk,                                                   */
/*           blue_msk;                                                  */
/*  }                                                                   */
/*                                                                      */

#define HCLOSE_DATA struct { word length; word flags; }
/* usage: HCLOSE_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word flags;                                                         */
/*  }                                                                   */
/*                                                                      */
/*                                                                      */


/* There is no HESC data                                                */
/*                                                                      */

#define HXLATE_DATA struct { word length; longword table[8]; }
/* usage: HXLATE_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  longword table[8];                                                  */
/*  }                                                                   */
/*                                                                      */

struct CharSetDef   /* Character Set Definition block                   */
{
  byte reserve1;        /* reserved                                     */
  byte csettype;        /* Char set type  0 - image/triplane image      */
                        /*                3 - short stroke vector       */
  long reserve2;        /* reserved                                     */
  byte reserve3;        /* reserved                                     */
  byte cellwidth;       /* character cell width (pels)                  */
  byte cellheight;      /* character cell height (pels)                 */
  byte reserve4;        /* reserved                                     */
  word cellnbytes;      /* no. bytes per cell definition (ceil(cx*cy/8))*/
  word flags;           /* Flags as follows                             */
#define MULTIPLANE 0x4000       /* single or multi plane                */
#define PROPSPACE  0x2000       /* single or multi plane                */
  word far *indextbl;   /* address of index table                       */
  byte far *enveltbl;   /* address of envelope table                    */
  byte cdpt1st;         /* initial code point                           */
  byte cdptlast;        /* final code point                             */
  byte far *chardef1;   /* address of character definition 1            */
  word reserve5;        /* reserved                                     */
  byte far *chardef2;   /* address of character definition 2            */
  word reserve6;        /* reserved                                     */
  byte far *chardef3;   /* address of character definition 3            */
};

#define HSCS_DATA struct { word length; struct CharSetDef far *address; }
/* usage: HSCS_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  struct CharSetDef far *address;                                     */
/*  }                                                                   */
/*                                                                      */

#define HCHST_DATA(L) struct { word length; coord_pr coord; char string[L]; }
/* usage: HCHST_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  char string[L];                                                     */
/*  }                                                                   */
/*                                                                      */

#define HCCHST_DATA(L) struct { word length; char string[L]; }
/* usage: HCCHST_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  char string[L];                                                     */
/*  }                                                                   */
/*                                                                      */

#define ABLKMFI_DATA struct { word length; byte colm,row,across,down; byte far *chars; byte width; }
/* usage: ABLKMFI_DATA will give the following structure                */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte colm,                                                           */
/*      row,                                                            */
/*      across,                                                         */
/*      down;                                                           */
/* byte far *chars;                                                     */
/* byte width;                                                          */
/* }                                                                    */
/*                                                                      */

#define ABLKCGA_DATA struct { word length; byte colm,row,across,down; byte far *chars; byte width,hilite; }
/* usage: ABLKCGA_DATA will give the following structure                */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte colm,                                                           */
/*      row,                                                            */
/*      across,                                                         */
/*      down;                                                           */
/* byte far *chars;                                                     */
/* byte width,                                                          */
/*      hilite;                                                         */
/* }                                                                    */
/*                                                                      */

#define AERASE_DATA struct { word length; byte colm,row,across,down,colour; }
/* usage: AERASE_DATA will give the following structure                 */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte colm,                                                           */
/*      row,                                                            */
/*      across,                                                         */
/*      down,                                                           */
/*      colour;                                                         */
/* }                                                                    */
/*                                                                      */

#define ASCROLL_DATA struct { word length; byte sor_colm,sor_row,across,down,des_colm,des_row; }
/* usage: ASCROLL_DATA will give the following structure                */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte sor_colm,                                                       */
/*      sor_row,                                                        */
/*      across,                                                         */
/*      down,                                                           */
/*      des_colm;                                                       */
/*      des_row;                                                        */
/* }                                                                    */
/*                                                                      */

#define ACURSOR_DATA struct { word length; byte colm,row; }
/* usage: ACURSOR_DATA will give the following structure                */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte colm,                                                           */
/*      row;                                                            */
/* }                                                                    */
/*                                                                      */

#define ASCUR_DATA struct { word length; byte start,stop,attr; }
/* usage: ASCUR_DATA will give the following structure                  */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte start,                                                          */
/*      stop,                                                           */
/*      attr;                                                           */
/* }                                                                    */
/*                                                                      */

#define ASFONT_DATA struct { word length; byte font,res; struct CharSetDef far *address; }
/* usage: ASFONT_DATA will give the following structure                 */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte font,                                                           */
/*      res;                                                            */
/* struct CharSetDef far *address;                                      */
/* }                                                                    */
/*                                                                      */

#define AXLATE_DATA struct { word length; longword foretable[16],backtable[16]; }
/* usage: AXLATE_DATA will give the following structure                 */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* longword foretable[16],                                              */
/*          backtable[16];                                              */
/* }                                                                    */
/*                                                                      */

#define HINIT_DATA struct { word length; word segment; }
/* usage: HINIT_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word segment;                                                       */
/*  }                                                                   */
/*                                                                      */

#define HSYNC_DATA struct { word length; word segment; }
/* usage: HSYNC_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word segment;                                                       */
/*  }                                                                   */
/*                                                                      */

#define HMRK_DATA(S) struct { word length; coord_pr coords[S];}
/* usage: HMRK_DATA will give the following structure                   */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* coord_pr coords[S]                                                   */
/* }                                                                    */
/*                                                                      */

#define HCMRK_DATA(S) struct { word length; coord_pr coords[S];}
/* usage: HCMRK_DATA will give the following structure                  */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* coord_pr coords[S]                                                   */
/* }                                                                    */
/*                                                                      */

#define HSMARK_DATA struct {word length; byte width, height, flags, res; word imlen; byte far *image; byte far *colour; }
/* usage: HSMARK_DATA will give the following structure                 */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte width,                                                          */
/*      height,                                                         */
/*      flags,                                                          */
/*      res;                                                            */
/* word imlen;                                                          */
/* byte far *image;                                                     */
/* byte far *colour;                                                    */
/* }                                                                    */
/*                                                                      */

#define HSLPC_DATA struct { word length; word count; }
/* usage: HSLPC_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word count;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HRLPC_DATA struct { word length; word count; }
/* usage: HRLPC_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word count;                                                         */
/*  }                                                                   */
/*                                                                      */

#define HQCP_DATA struct { word length; coord_pr coord1; }
/* usage: HQCP_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord1;                                                    */
/*  }                                                                   */
/*                                                                      */

#define HQDFPAL_DATA struct {word length; long table[16]; }
/* usage: HQDFPAL_DATA will give the following structure                */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* long table[16];                                                      */
/* }                                                                    */
/*                                                                      */

#define HSPAL_DATA struct { word length; byte* paldata[769]; }
/* usage: HSPAL_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte paldata[769];                                                  */
/*  }                                                                   */
/*                                                                      */

#define HRPAL_DATA HSPAL_DATA
/* usage: HRPAL uses an HSPAL data block                                */
/*                                                                      */

#define HSFPAL_DATA struct { word length; word format; byte paldata[769]; }
/* usage: HSFPAL_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word format;                                                        */
/*  byte paldata[769];                                                  */
/*  }                                                                   */
/*                                                                      */

#define HRFPAL_DATA HSFPAL_DATA
/* usage: HRFPAL uses an HSFPAL data block                              */
/*                                                                      */

#define HSAFP_DATA struct { word length; byte far *address; byte flags; }
/* usage: HSAFP will give the following structure                       */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte far *address;                                                  */
/*  byte flags;                                                         */
/*  }                                                                   */
/*                                                                      */

#define ASCELL_DATA struct { word length; byte width,height; }
/* usage: ASCELL will give the following structure                      */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte width,                                                         */
/*       height;                                                        */
/*  }                                                                   */
/*                                                                      */

#define ASGO_DATA struct { word length; coord_pr coord; }
/* usage: ASGO_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  }                                                                   */
/*                                                                      */

#define HDLINE_DATA(S) struct { word length; dcoord  dcoords[S]; }
/* usage: HDLINE_DATA(size) will give the following structure           */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*      word    length;                                                 */
/*      dcoord  dcoords[size];                                          */
/*  }                                                                   */
/*                                                                      */

#define HPEL_DATA(S) struct { word length; pel_pr coord[S];}
/* usage: HPEL_DATA(size) will give the following structure             */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  pel_pr coord[size];                                                 */
/*  }                                                                   */
/*                                                                      */

#define HRPEL_DATA struct { word length; byte far *address; coord_pr coord; word pel_count; }
/* usage: HRPEL_DATA will give the following structure                  */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte far *address;                                                  */
/*  coord_pr coord;                                                     */
/*  word pel_count;                                                     */
/*  }                                                                   */
/*                                                                      */

#define HPSTEP_DATA struct { word length; coord_pr coord; byte far *def_address,*src_address; }
/* usage: HPSTEP_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  byte far *def_address,                                              */
/*           *src_address;                                              */
/*  }                                                                   */
/*                                                                      */

#define HCPSTEP_DATA struct { word length; byte far *def_address,*src_address; }
/* usage: HCPSTEP_DATA will give the following structure                */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte far *def_address,                                              */
/*           *src_address;                                              */
/*  }                                                                   */
/*                                                                      */

#define HRSTEP_DATA struct { word length; coord_pr coord; byte far *def_address,*tgt_address; }
/* usage: HRSTEP_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  byte far *def_address,                                              */
/*           *tgt_address;                                              */
/*  }                                                                   */
/*                                                                      */

#define HSBMAP_DATA struct { word length; byte flags, format; byte far *bmap_addr; word width, height; }
/* usage: HSBMAP_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte flags,                                                         */
/*       format;                                                        */
/*  byte far *bmap_addr;                                                */
/*  word width,                                                         */
/*       height;                                                        */
/*  }                                                                   */
/*                                                                      */

#define HQBMAP_DATA struct { word length; byte flags, format; byte far *bmap_addr; word width, height; coord_pr coord; word wind_width, wind_height;}
/* usage: HQBMAP_DATA will give the following structure                 */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte flags,                                                         */
/*       format;                                                        */
/*  byte far *bmap_addr;                                                */
/*  word width,                                                         */
/*       height;                                                        */
/*  coord_pr coord;                                                     */
/*  word wind_width,                                                    */
/*       wind_height;                                                   */
/*  }                                                                   */
/*                                                                      */

#define HBMC_DATA struct {word length;word flags,blk_width,blk_height; byte dst_format,reserved1;byte far *dst_addr;word dst_width,dst_height;coord_pr dst_coord; byte src_format,reserved2;byte far *src_addr;word src_width,src_height;coord_pr src_coord;\
byte pat_format,reserved3;byte far *pat_addr;word pat_width,pat_height;coord_pr pat_coord;}
/* usage: HBMC_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  word flags,                                                         */
/*       blk_width,                                                     */
/*       blk_height;                                                    */
/*                                                                      */
/*  byte      dst_format,                                               */
/*            reserved1;                                                */
/*  byte far *dst_addr;                                                 */
/*  word      dst_width,                                                */
/*            dst_height;                                               */
/*  coord_pr  dst_coord;                                                */
/*                                                                      */
/*  byte      src_format,                                               */
/*            reserved2;                                                */
/*  byte far *src_addr;                                                 */
/*  word      src_width,                                                */
/*            src_height;                                               */
/*  coord_pr  src_coord;                                                */
/*                                                                      */
/*  byte      pat_format,                                               */
/*            reserved3;                                                */
/*  byte far *pat_addr;                                                 */
/*  word      pat_width,                                                */
/*            pat_height;                                               */
/*  coord_pr  pat_coord;                                                */
/*                                                                      */
/*  }                                                                   */

#define HSPRITE_DATA struct { word length; coord_pr coord; }
/* usage: HSPRITE will give the following structure                     */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  }                                                                   */
/*                                                                      */

#define HSSPRITE_DATA struct { word length; byte flags,res,hotx,hoty; byte far *image;word width,height;\
                               word col1_green, col1_red, col1_blue, col2_green, col2_red, col2_blue; }
/* usage: HSSPRITE_DATA will give the following structure               */
/*                                                                      */
/* struct                                                               */
/* {                                                                    */
/* word length;                                                         */
/* byte flags,                                                          */
/*      res,                                                            */
/*      hotx,                                                           */
/*      hoty;                                                           */
/* byte far *image;                                                     */
/* word width,                                                          */
/*      height;                                                         */
/* word col1_green,                                                     */
/*      col1_red,                                                       */
/*      col1_blue,                                                      */
/*      col2_green,                                                     */
/*      col2_red,                                                       */
/*      col2_blue;                                                      */
/* }                                                                    */

#define HSDW_DATA struct { word length; coord_pr coord; word width,height;}
/* usage: HSDW_DATA will give the following structure                   */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  coord_pr coord;                                                     */
/*  word width,                                                         */
/*       height;                                                        */
/*  }                                                                   */
/*                                                                      */

#define HRWVEC_DATA(S) struct { word length; byte flags, reserved; byte far *buffer; coord_pr coords[S]; }
/* usage: HRWVEC_DATA(size) will give the following structure           */
/*                                                                      */
/*  struct                                                              */
/*  {                                                                   */
/*  word length;                                                        */
/*  byte flags,                                                         */
/*       reserved;                                                      */
/*  byte far *buffer;                                                   */
/*  coord_pr coords[size];                                              */
/*  }                                                                   */
/*                                                                      */


/* Font file definition                                                 */
/* This is the layout of the font file header                           */
/* All the address fields within the CSD's (Character Set Definition)   */
/* are offsets within the file. To these should be added the address    */
/* at which the font is loaded to give the correct values.              */
/*                                                                      */
struct FontFileDefn
{
  word no_pages; /* no of code pages in the file (size of page_array)   */
  word def_page; /* default page index (into page_array)                */
  word alt_page; /* alternate default page index (into page_array)      */
  struct
  {
    byte code_page_id[4];       /* code page id                         */
    word csd_offset;            /* offset within file of CSD block      */
  } page_array[1];      /* array of code page ID's and CSD offsets      */
};

/* (C) Copyright IBM Corporation 1986,1987,1988,1989,1990 ***************/
/*                                                                      */
/* Definitions & declarations relevant to the Adapter Interface         */
/* For use in conjunction with the Adapter linked List                  */
/*                                                                      */
/* This file also includes the compiler independent file afidata.h      */
/*                                                                      */
/************************************************************************/

#ifndef NULL
#define NULL 0x0L
#endif

#ifndef byte
#define byte unsigned char
#endif

#ifndef word
#define word unsigned int
#endif

#ifndef longword
#define longword unsigned long
#endif

typedef struct
{
   int x_coord;
   int y_coord;
} coord_pr;

typedef struct
{
   char rx_coord;
   char ry_coord;
} rcoord_pr;

typedef struct
{
   int x_coord;
   int y_coord;
   int pel_count;
} pel_pr;

typedef struct
{
   int   x_start;
   int   y_start;
   int   x_end;
   int   y_end;
} dcoord;

typedef struct
{
   int   x_coord;
   int   y_coord;
   word  width;
   word  height;
} rect_def;

struct afi_entries
{
  void  ( far pascal * hline    )( char far * );
  void  ( far pascal * hcline   )( char far * );
  void  ( far pascal * hrline   )( char far * );
  void  ( far pascal * hcrline  )( char far * );
  void  ( far pascal * hscp     )( char far * );
  void  ( far pascal * hbar     )( char far * );
  void  ( far pascal * hear     )( char far * );
  void  ( far pascal * hscol    )( char far * );
  void  ( far pascal * hopen    )( char far * );
  void  ( far pascal * hsmx     )( char far * );
  void  ( far pascal * hsbcol   )( char far * );
  void  ( far pascal * hslt     )( char far * );
  void  ( far pascal * hslw     )( char far * );
  void  ( far pascal * hegs     )( char far * );
  void  ( far pascal * hsgq     )( char far * );
  void  ( far pascal * hscmp    )( char far * );
  void  ( far pascal * hint     )( char far * );
  void  ( far pascal * hspatto  )( char far * );
  void  ( far pascal * hspatt   )( char far * );
  void  ( far pascal * hldpal   )( char far * );
  void  ( far pascal * hshs     )( char far * );
  void  ( far pascal * hbbw     )( char far * );
  void  ( far pascal * hcbbw    )( char far * );
  void  ( far pascal * hbbr     )( char far * );
  void  ( far pascal * hbbchn   )( char far * );
  void  ( far pascal * hbbc     )( char far * );
  void  ( far pascal * hscoord  )( char far * );
  void  ( far pascal * hqcoord  )( char far * );
  void  ( far pascal * hsmode   )( char far * );
  void  ( far pascal * hqmode   )( char far * );
  void  ( far pascal * hqmodes  )( char far * );
  void  ( far pascal * hqdps    )( char far * );
  void  ( far pascal * hrect    )( char far * );
  void  ( far pascal * hsbp     )( char far * );
  void  ( far pascal * hclose   )( char far * );
  void  ( far pascal * hesc     )( char far * );
  void  ( far pascal * hxlate   )( char far * );
  void  ( far pascal * hscs     )( char far * );
  void  ( far pascal * hchst    )( char far * );
  void  ( far pascal * hcchst   )( char far * );
  void  ( far pascal * ablockmfi)( char far * );
  void  ( far pascal * ablockcga)( char far * );
  void  ( far pascal * aerase   )( char far * );
  void  ( far pascal * ascroll  )( char far * );
  void  ( far pascal * acursor  )( char far * );
  void  ( far pascal * ascur    )( char far * );
  void  ( far pascal * asfont   )( char far * );
  void  ( far pascal * axlate   )( char far * );
  void  ( far pascal * hinit    )( char far * );
  void  ( far pascal * hsync    )( char far * );
  void  ( far pascal * hmrk     )( char far * );
  void  ( far pascal * hcmrk    )( char far * );
  void  ( far pascal * hsmark   )( char far * );
  void  ( far pascal * hslpc    )( char far * );
  void  ( far pascal * hrlpc    )( char far * );
  void  ( far pascal * hqcp     )( char far * );
  void  ( far pascal * hqdfpal  )( char far * );
  void  ( far pascal * hspal    )( char far * );
  void  ( far pascal * hrpal    )( char far * );
  void  ( far pascal * hsafp    )( char far * );
  void  ( far pascal * ascell   )( char far * );
  void  ( far pascal * asgo     )( char far * );
  void  ( far pascal * hdline   )( char far * );
  void  ( far pascal * hreserve1)( char far * );     /* reserved call */
  void  ( far pascal * hpel     )( char far * );
  void  ( far pascal * hrpel    )( char far * );
  void  ( far pascal * hpstep   )( char far * );
  void  ( far pascal * hcpstep  )( char far * );
  void  ( far pascal * hrstep   )( char far * );
  void  ( far pascal * hsbmap   )( char far * );
  void  ( far pascal * hqbmap   )( char far * );
  void  ( far pascal * hbmc     )( char far * );
  void  ( far pascal * hsdw     )( char far * );
  void  ( far pascal * hsprite  )( char far * );
  void  ( far pascal * hssprite )( char far * );
  void  ( far pascal * hrwvec   )( char far * );
  void  ( far pascal * hreserve2)( char far * );     /* reserved call */
  void  ( far pascal * hreserve3)( char far * );     /* reserved call */
  void  ( far pascal * hsfpal   )( char far * );
  void  ( far pascal * hrfpal   )( char far * );
  void  ( far pascal * hqdevice )( char far * );
  void  ( far pascal * hreserve6)( char far * );     /* reserved call */
  void  ( far pascal * hreserve7)( char far * );     /* reserved call */
  void  ( far pascal * hreserve8)( char far * );     /* reserved call */
};





/* Structure definition for an AI linked list entry */

struct afi_list_entry
{
    struct  afi_entries    far * afi_tbl_ptr ;
    byte                         adapter_type ;
    byte                         instance ;
    struct  afi_list_entry far * link_ptr ;
};


/* External declaration of adapter linked list address returning function */

extern struct afi_list_entry far * ( far getafil( void ) );

#define MHLINE(W,X)     (*W->hline)((char far *) X)
#define MHCLINE(W,X)    (*W->hcline)((char far *) X)
#define MHRLINE(W,X)    (*W->hrline)((char far *) X)
#define MHCRLINE(W,X)   (*W->hcrline)((char far *) X)
#define MHSCP(W,X)      (*W->hscp)((char far *) X)
#define MHBAR(W)        (*W->hbar)((char far *) 0L)
#define MHEAR(W,X)      (*W->hear)((char far *) X)
#define MHSCOL(W,X)     (*W->hscol)((char far *) X)
#define MHOPEN(W,X)     (*W->hopen)((char far *) X)
#define MHSMX(W,X)      (*W->hsmx)((char far *) X)
#define MHSBCOL(W,X)    (*W->hsbcol)((char far *) X)
#define MHSLT(W,X)      (*W->hslt)((char far *) X)
#define MHSLW(W,X)      (*W->hslw)((char far *) X)
#define MHEGS(W)        (*W->hegs)((char far *) 0L)
#define MHSGQ(W,X)      (*W->hsgq)((char far *) X)
#define MHSCMP(W,X)     (*W->hscmp)((char far *) X)
#define MHINT(W,X)      (*W->hint)((char far *) X)
#define MHSPATTO(W,X)   (*W->hspatto)((char far *) X)
#define MHSPATT(W,X)    (*W->hspatt)((char far *) X)
#define MHLDPAL(W,X)    (*W->hldpal)((char far *) X)
#define MHSHS(W,X)      (*W->hshs)((char far *) X)
#define MHBBW(W,X)      (*W->hbbw)((char far *) X)
#define MHCBBW(W,X)     (*W->hcbbw)((char far *) X)
#define MHBBR(W,X)      (*W->hbbr)((char far *) X)
#define MHBBCHN(W,X)    (*W->hbbchn)((char far *) X)
#define MHBBC(W,X)      (*W->hbbc)((char far *) X)
#define MHSCOORD(W,X)   (*W->hscoord)((char far *) X)
#define MHQCOORD(W,X)   (*W->hqcoord)((char far *) X)
#define MHSMODE(W,X)    (*W->hsmode )((char far *) X)
#define MHQMODE(W,X)    (*W->hqmode )((char far *) X)
#define MHQMODES(W,X)   (*W->hqmodes)((char far *) X)
#define MHQDPS(W,X)     (*W->hqdps  )((char far *) X)
#define MHRECT(W,X)     (*W->hrect  )((char far *) X)
#define MHSBP(W,X)      (*W->hsbp   )((char far *) X)
#define MHCLOSE(W,X)    (*W->hclose )((char far *) X)
#define MHESC(W)        (*W->hesc   )((char far *) 0L)
#define MHXLATE(W,X)    (*W->hxlate )((char far *) X)
#define MHSCS(W,X)      (*W->hscs)((char far *) X)
#define MHCHST(W,X)     (*W->hchst)((char far *) X)
#define MHCCHST(W,X)    (*W->hcchst)((char far *) X)
#define MABLKMFI(W,X)   (*W->ablockmfi)((char far *) X)
#define MABLKCGA(W,X)   (*W->ablockcga)((char far *) X)
#define MAERASE(W,X)    (*W->aerase )((char far *) X)
#define MASCROLL(W,X)   (*W->ascroll)((char far *) X)
#define MACURSOR(W,X)   (*W->acursor)((char far *) X)
#define MASCUR(W,X)     (*W->ascur  )((char far *) X)
#define MASFONT(W,X)    (*W->asfont )((char far *) X)
#define MAXLATE(W,X)    (*W->axlate )((char far *) X)
#define MHINIT(W,X)     (*W->hinit)((char far *) X)
#define MHSYNC(W,X)     (*W->hsync)((char far *) X)
#define MHMRK(W,X)      (*W->hmrk   )((char far *) X)
#define MHCMRK(W,X)     (*W->hcmrk  )((char far *) X)
#define MHSMARK(W,X)    (*W->hsmark )((char far *) X)
#define MHSLPC(W,X)     (*W->hslpc  )((char far *) X)
#define MHRLPC(W,X)     (*W->hrlpc  )((char far *) X)
#define MHQCP(W,X)      (*W->hqcp )((char far *) X)
#define MHQDFPAL(W,X)   (*W->hqdfpal )((char far *) X)
#define MHSPAL(W,X)     (*W->hspal )((char far *) X)
#define MHRPAL(W,X)     (*W->hrpal )((char far *) X)
#define MHSAFP(W,X)     (*W->hsafp )((char far *) X)
#define MASCELL(W,X)    (*W->ascell )((char far *) X)
#define MASGO(W,X)      (*W->asgo   )((char far *) X)
#define MHDLINE(W,X)    (*W->hdline )((char far *) X)
        /* blank entry */
#define MHPEL(W,X)      (*W->hpel )((char far *) X)
#define MHRPEL(W,X)     (*W->hrpel )((char far *) X)
#define MHPSTEP(W,X)    (*W->hpstep )((char far *) X)
#define MHCPSTEP(W,X)   (*W->hcpstep )((char far *) X)
#define MHRSTEP(W,X)    (*W->hrstep )((char far *) X)
#define MHSBMAP(W,X)    (*W->hsbmap )((char far *) X)
#define MHQBMAP(W,X)    (*W->hqbmap )((char far *) X)
#define MHBMC(W,X)      (*W->hbmc )((char far *) X)
#define MHSDW(W,X)      (*W->hsdw )((char far *) X)
#define MHSPRITE(W,X)   (*W->hsprite )((char far *) X)
#define MHSSPRITE(W,X)  (*W->hssprite )((char far *) X)
#define MHRWVEC(W,X)    (*W->hrwvec )((char far *) X)
        /* blank entry */
        /* blank entry */
#define MHSFPAL(W,X)    (*W->hsfpal )((char far *) X)
#define MHRFPAL(W,X)    (*W->hrfpal )((char far *) X)
#define MHQDEVICE(W,X)  (*W->hqdevice )((char far *) X)
        /* blank entry */
        /* blank entry */
        /* blank entry */



extern struct afi_entries far *afi_ptr,far *(far getafi(void));



#define HLINE(X)        (*afi_ptr->hline)((char far *) X)
#define HCLINE(X)       (*afi_ptr->hcline)((char far *) X)
#define HRLINE(X)       (*afi_ptr->hrline)((char far *) X)
#define HCRLINE(X)      (*afi_ptr->hcrline)((char far *) X)
#define HSCP(X)         (*afi_ptr->hscp)((char far *) X)
#define HBAR()          (*afi_ptr->hbar)((char far *) 0L)
#define HEAR(X)         (*afi_ptr->hear)((char far *) X)
#define HSCOL(X)        (*afi_ptr->hscol)((char far *) X)
#define HOPEN(X)        (*afi_ptr->hopen)((char far *) X)
#define HSMX(X)         (*afi_ptr->hsmx)((char far *) X)
#define HSBCOL(X)       (*afi_ptr->hsbcol)((char far *) X)
#define HSLT(X)         (*afi_ptr->hslt)((char far *) X)
#define HSLW(X)         (*afi_ptr->hslw)((char far *) X)
#define HEGS()          (*afi_ptr->hegs)((char far *) 0L)
#define HSGQ(X)         (*afi_ptr->hsgq)((char far *) X)
#define HSCMP(X)        (*afi_ptr->hscmp)((char far *) X)
#define HINT(X)         (*afi_ptr->hint)((char far *) X)
#define HSPATTO(X)      (*afi_ptr->hspatto)((char far *) X)
#define HSPATT(X)       (*afi_ptr->hspatt)((char far *) X)
#define HLDPAL(X)       (*afi_ptr->hldpal)((char far *) X)
#define HSHS(X)         (*afi_ptr->hshs)((char far *) X)
#define HBBW(X)         (*afi_ptr->hbbw)((char far *) X)
#define HCBBW(X)        (*afi_ptr->hcbbw)((char far *) X)
#define HBBR(X)         (*afi_ptr->hbbr)((char far *) X)
#define HBBCHN(X)       (*afi_ptr->hbbchn)((char far *) X)
#define HBBC(X)         (*afi_ptr->hbbc)((char far *) X)
#define HSCOORD(X)      (*afi_ptr->hscoord)((char far *) X)
#define HQCOORD(X)      (*afi_ptr->hqcoord)((char far *) X)
#define HSMODE(X)       (*afi_ptr->hsmode )((char far *) X)
#define HQMODE(X)       (*afi_ptr->hqmode )((char far *) X)
#define HQMODES(X)      (*afi_ptr->hqmodes)((char far *) X)
#define HQDPS(X)        (*afi_ptr->hqdps  )((char far *) X)
#define HRECT(X)        (*afi_ptr->hrect  )((char far *) X)
#define HSBP(X)         (*afi_ptr->hsbp   )((char far *) X)
#define HCLOSE(X)       (*afi_ptr->hclose )((char far *) X)
#define HESC()          (*afi_ptr->hesc   )((char far *) 0L)
#define HXLATE(X)       (*afi_ptr->hxlate )((char far *) X)
#define HSCS(X)         (*afi_ptr->hscs)((char far *) X)
#define HCHST(X)        (*afi_ptr->hchst)((char far *) X)
#define HCCHST(X)       (*afi_ptr->hcchst)((char far *) X)
#define ABLKMFI(X)      (*afi_ptr->ablockmfi)((char far *) X)
#define ABLKCGA(X)      (*afi_ptr->ablockcga)((char far *) X)
#define AERASE(X)       (*afi_ptr->aerase )((char far *) X)
#define ASCROLL(X)      (*afi_ptr->ascroll)((char far *) X)
#define ACURSOR(X)      (*afi_ptr->acursor)((char far *) X)
#define ASCUR(X)        (*afi_ptr->ascur  )((char far *) X)
#define ASFONT(X)       (*afi_ptr->asfont )((char far *) X)
#define AXLATE(X)       (*afi_ptr->axlate )((char far *) X)
#define HINIT(X)        (*afi_ptr->hinit)((char far *) X)
#define HSYNC(X)        (*afi_ptr->hsync)((char far *) X)
#define HMRK(X)         (*afi_ptr->hmrk   )((char far *) X)
#define HCMRK(X)        (*afi_ptr->hcmrk  )((char far *) X)
#define HSMARK(X)       (*afi_ptr->hsmark )((char far *) X)
#define HSLPC(X)        (*afi_ptr->hslpc  )((char far *) X)
#define HRLPC(X)        (*afi_ptr->hrlpc  )((char far *) X)
#define HQCP(X)         (*afi_ptr->hqcp )((char far *) X)
#define HQDFPAL(X)      (*afi_ptr->hqdfpal )((char far *) X)
#define HSPAL(X)        (*afi_ptr->hspal )((char far *) X)
#define HRPAL(X)        (*afi_ptr->hrpal )((char far *) X)
#define HSAFP(X)        (*afi_ptr->hsafp )((char far *) X)
#define ASCELL(X)       (*afi_ptr->ascell )((char far *) X)
#define ASGO(X)         (*afi_ptr->asgo )((char far *) X)
#define HDLINE(X)       (*afi_ptr->hdline )((char far *) X)
        /* blank entry no. 63 */
#define HPEL(X)         (*afi_ptr->hpel )((char far *) X)
#define HRPEL(X)        (*afi_ptr->hrpel )((char far *) X)
#define HPSTEP(X)       (*afi_ptr->hpstep )((char far *) X)
#define HCPSTEP(X)      (*afi_ptr->hcpstep )((char far *) X)
#define HRSTEP(X)       (*afi_ptr->hrstep )((char far *) X)
#define HSBMAP(X)       (*afi_ptr->hsbmap )((char far *) X)
#define HQBMAP(X)       (*afi_ptr->hqbmap )((char far *) X)
#define HBMC(X)         (*afi_ptr->hbmc )((char far *) X)
#define HSDW(X)         (*afi_ptr->hsdw )((char far *) X)
#define HSPRITE(X)      (*afi_ptr->hsprite )((char far *) X)
#define HSSPRITE(X)     (*afi_ptr->hssprite )((char far *) X)
#define HRWVEC(X)       (*afi_ptr->hrwvec )((char far *) X)
        /* blank entry */
        /* blank entry */
#define HSFPAL(X)       (*afi_ptr->hsfpal )((char far *) X)
#define HRFPAL(X)       (*afi_ptr->hrfpal )((char far *) X)
#define HQDEVICE(X)     (*afi_ptr->hqdevice )((char far *) X)
        /* blank entry */
        /* blank entry */
        /* blank entry */

#include "afidata.h"

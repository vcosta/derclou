/*
**	$Filename: gfx/gfx.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	gfx functions for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_GFX
#define MODULE_GFX

#include "theclou.h"
#include "SDL.h"

#include "list/list.h"

#define GFX_NO_MEM_HANDLE	((uword) -1)

/* defines for gfxPrint */
#define GFX_PRINT_CENTER        (1)
#define GFX_PRINT_RIGHT         (1<<1)
#define GFX_PRINT_LEFT          (1<<2)
#define GFX_PRINT_SHADOW        (1<<3)

typedef enum {
    GFX_SCALE_NORMAL,

    GFX_SCALE_2X,
    GFX_SCALE_LINEAR2X
} GfxScaleE;

/* defines for SetDrMd */

typedef enum {
    GFX_JAM_1               = 0,    /* Hintergrund wird gleichbelassen */
    GFX_JAM_2               = 1	    /* Hintergrund wird mit BackPen bermalt */
} GfxDrawModeE;

/* defines for Show */
#define GFX_NO_REFRESH             1
#define GFX_BLEND_UP               2
#define GFX_FADE_OUT               4
#define GFX_CLEAR_FIRST            8
#define GFX_OVERLAY               16
#define GFX_ONE_STEP              32	/* "plain" blit */
#define GFX_DONT_SHOW_FIRST_PIC   64

/* defines for gfxSetPens */
#define GFX_SAME_PEN						((ubyte)255)

#define GFX_NO_COLL_IN_MEM               UINT16_MAX

#define GFX_PALETTE_SIZE			768	/* 256 Farben * 3 Bytes */
#define GFX_SIZE_OF_COLLECTION(c) 	(gfxGetILBMSize(c) + GFX_COLORTABLE_SIZE)

#define  gfxWaitBOF						wfd

#define GFX_VIDEO_MCGA					1
#define GFX_VIDEO_NCH4					2
#define GFX_VIDEO_TEXT					3

typedef struct _MemRastPort MemRastPort;
typedef struct _Rect        Rect;
typedef struct Font         Font;

struct _MemRastPort {
    U16 w;
    U16 h;

    U8  palette [GFX_PALETTE_SIZE];
    U8 *pixels;

    U16 collId;                 /* Collection, die sich gerade hier befindet! */
};

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200
#define SCREEN_SIZE     (SCREEN_WIDTH * SCREEN_HEIGHT)

void gfxInitMemRastPort(MemRastPort *rp, U16 width, U16 height);
void gfxDoneMemRastPort(MemRastPort *rp);

void gfxScratchFromMem(MemRastPort *src);
void gfxScratchToMem(MemRastPort *dst);

void gfxCollFromMem(U16 collId);
void gfxCollToMem(U16 collId, MemRastPort *rp);

struct _Rect {
    U16 x;
    U16 y;

    U16 w;
    U16 h;
};

struct Collection {
    NODE Link;

    U16 us_CollId;

    char *puch_Filename;

    MemRastPort *prepared;

    void *p_ColorTable;		/* not always correct (only as long as nothing
  				   else modified the buffer!) */

    uword us_TotalWidth;
    uword us_TotalHeight;

    ubyte uch_ColorRangeStart;
    ubyte uch_ColorRangeEnd;
};

typedef struct _GC GC;

struct Picture {
    NODE Link;

    U16 us_PictId;
    U16 us_CollId;		/* in welcher Collection sich dieses Bild befindet */

    U16 us_XOffset;		/* innerhalb der Collection */
    U16 us_YOffset;

    U16 us_Width;
    U16 us_Height;

    U16 us_DestX;
    U16 us_DestY;
};

struct Font {
    SDL_Surface *bmp;

    U16 w;	                /* width of one character */
    U16 h;	                /* height of one character */

    unsigned char first;
    unsigned char last;
};

extern GC *l_gc;
extern GC *u_gc;
extern GC *m_gc;

extern MemRastPort ScratchRP;

extern MemRastPort StdRP0InMem;
extern MemRastPort StdRP1InMem;
extern MemRastPort AnimRPInMem;
extern MemRastPort AddRPInMem;
extern MemRastPort LSFloorRPInMem;
extern MemRastPort LSObjectRPInMem;

extern MemRastPort BobRPInMem;

extern MemRastPort LSRPInMem;

extern struct Font *bubbleFont;
extern struct Font *menuFont;

extern void gfxInit(void);
extern void gfxDone(void);

extern void gfxSetVideoMode(ubyte uch_NewMode);

extern void wfd(void);
extern void wfr(void);

extern S32 gfxGetILBMSize(struct Collection *coll);

extern void gfxCorrectUpperRPBitmap(void);

void gfxSetRGB(GC *gc, U8 color, U8 r, U8 g, U8 b);

void gfxMoveCursor(GC *gc, U16 x, U16 y);
void gfxSetPens(GC *gc, U8 foreground, U8 background, U8 outline);
void gfxRectFill(GC *gc, U16 sx, U16 sy, U16 ex, U16 ey);
void gfxDraw(GC *gc, U16 x, U16 y);
void gfxSetDrMd(GC *rp, GfxDrawModeE mode);
void gfxSetFont(GC *gc, Font *font);

extern void gfxPrepareRefresh(void);
extern void gfxRefresh(void);

extern void gfxClearArea(GC *gc);
extern void gfxSetRect(uword us_X, uword us_Width);

extern U16 gfxTextWidth(GC *gc, const char *txt, size_t len);

extern void gfxPrint(GC *gc, const char *txt, U16 y, U32 mode);
extern void gfxPrintExact(GC *gc, const char *txt, U16 x, U16 y);

extern void gfxSetColorRange(ubyte uch_ColorStart, ubyte uch_ColorEnd);

extern void gfxChangeColors(GC *gc, U32 delay, U32 mode, U8 *palette);
extern void gfxShow(uword us_PictId, U32 ul_Mode, S32 l_Delay, S32 l_XPos,
		    S32 l_YPos);

extern void gfxSetGC(GC *gc);

extern void gfxGetPaletteFromReg(U8 *palette);
extern void gfxGetPalette(U16 collId, U8 *palette);

void gfxPrepareColl(U16 collId);
void gfxUnPrepareColl(U16 collId);

extern struct Collection *gfxGetCollection(uword us_CollId);
extern struct Picture *gfxGetPicture(uword us_PictId);

void gfxLoadILBM(char *fileName);

void gfxWaitTOF(void);
void gfxWaitTOR(void);
void gfxWaitTOS(void);

void gfxRAWBlit(U8 * sp, U8 * dp, const int x1, const int y1, const int x2,
		const int y2, const int w, const int h, const int sw,
		const int dw);

typedef enum {
    GFX_ROP_BLIT      = 0,
    GFX_ROP_MASK_BLIT = 1,
    GFX_ROP_CLR       = 2,
    GFX_ROP_SET       = 3
} ROpE;

void gfxBlit(GC *gc, MemRastPort *src, U16 sx, U16 sy, U16 dx, U16 dy,
             U16 w, U16 h, bool has_mask);

void MemBlit(MemRastPort *src, Rect *src_rect,
             MemRastPort *dst, Rect *dst_rect, ROpE op);

void gfxRefreshArea(U16 x, U16 y, U16 w, U16 h);

void gfxScreenFreeze(void);
void gfxScreenThaw(GC *gc, U16 x, U16 y, U16 w, U16 h);
void gfxScreenUnFreeze(void);

void gfxGetMouseXY(GC *gc, U16 *pMouseX, U16 *pMouseY);

void ShowIntro(void);

#include "gfx/gfxnch4.h"
#endif

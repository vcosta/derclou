/*
**	$Filename: gfx\gfx.ph
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	private gfx includes
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

#include "SDL.h"

#define GFX_SEG_ADDR						(0xA0000000L)
#define GFX_PAGE_SIZE					(64000L)	/* 320*200 = 64000 Bytes gro· */

#define GFX_LOAD_BUFFER					StdBuffer0
#define GFX_DECR_BUFFER					DecrBuffer

#define GFX_DECR_BUFFER_SIZE			(320*192)

#define GFX_BUBBLE_FONT_NAME			("bubble.fnt")
#define GFX_MENU_FONT_NAME				("menu.fnt")

#define GFX_CMAP_OFFSET					61440	// -> Maximalgrî·e 320 * 192

struct PrintRect {
    uword us_X;
    uword us_Width;
};

struct ColorRange {
    ubyte uch_Start;
    ubyte uch_End;
};

struct GfxBase {
    GC *gc;

    ubyte uch_VideoMode;
};

GC LowerGC;
GC MenuGC;
GC LSUpperGC;
GC LSMenuGC;

/* wird verwendet fÅr Double Buffering wÑhrend der Vorbereitung */
MemRastPort ScratchRP;
/* Da man von Mem nur ganze Blîcke kopieren kann (ohne Modulo)    */
/* mÅssen einige Grafiken, bevor sie ins Video RAM kopiert werden */
/* hier an dieser Stelle gepuffert werden. */
/* ist als extern definfiert, da Animmodul darauf zugreifen mu· */


MemRastPort StdRP0InMem;
/* wird benutzt fÅr Objekte, die immer im Speicher sind */
MemRastPort StdRP1InMem;
/* wird benutzt fÅr Objekte, die immer im Speicher sind */
MemRastPort RefreshRPInMem;
/* Bild, das im Moment gerade gezeigt wird (fÅr Refresh) */
MemRastPort AnimRPInMem;
/* beinhaltet die Animphasen des gerade aktuellen Bildes */
/* um Inkonsistenzen zu vermeiden, wird jedesmal, bevor eine */
/* Animphase gezeigt wird, das gesamte Bild in den PrepareRp */
/* kopiert, der benîtigte Ausschnitt wird dann von dort */
/* ins Video-RAM kopiert */
/* ist als extern definfiert, da Animmodul darauf zugreifen mu· */
MemRastPort AddRPInMem;
/* in diesem Rastport befinden sich diverse zusÑtzliche Grafiken */
/* die in das Bild hinzugefÅgt werden (Gesichter, Werkzeuge)     */
/* je nach Situation... 													  */
/* dieser RP wird je nach Situation ausgewechselt					  */
MemRastPort LSFloorRPInMem;
/* in diesem RastPort befindet sich wÑhrend der Planungs-Einbruchs */
/* phasen das Bild der Bîden */
MemRastPort LSObjectRPInMem;
/* in diesem RastPort befinden sich einige Objekte wÑhrend der Planung */


MemRastPort BobRPInMem;

MemRastPort LSRPInMem;

SDL_Surface *GfxBoardBase;
SDL_Surface *Screen;

ubyte DecrBuffer[GFX_DECR_BUFFER_SIZE];

struct PrintRect GlobalPrintRect;
struct ColorRange GlobalColorRange;

struct GfxBase GfxBase;

LIST *CollectionList = NULL;
LIST *PictureList = NULL;

GC *l_gc;
GC *m_gc;
GC *u_gc;

struct Font *bubbleFont;
struct Font *menuFont;

static void gfxInitGC(GC *gc, U16 x, U16 y, U16 w, U16 h,
                      U8 colorStart, U8 colorEnd, Font *font);

static void gfxInitCollList(void);
static void gfxInitPictList(void);

static void gfxSetCMAP(const Uint8 *src);

static Font *gfxOpenFont(char *fileName, U16 w, U16 h,
                         unsigned char first, unsigned char last,
                         U16 sw, U16 sh);
static void gfxCloseFont(Font *font);

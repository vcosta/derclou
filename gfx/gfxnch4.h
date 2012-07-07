/*
**	$Filename: gfx/gfxnch4.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     20-04-94
**
**	basic functions for "Der Clou!"
**
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_GFXNCH4
#define MODULE_GFXNCH4

#include "theclou.h"

#include "gfx/gfx.h"

#define GFX_NCH4_SCROLLOFFSET 	(160 * 72)	/* memory used by the fixed display */

#define ScrLEFT 0
#define ScrRIGHT 639
#define ScrTOP 0
#define ScrBOTTOM 255

struct RastPort;

extern U32 gfxNCH4GetCurrScrollOffset(void);
void gfxSetDarkness(ubyte value);

void gfxNCH4Refresh(void);

extern void gfxNCH4SetViewPort(int x, int y);
extern void gfxNCH4Scroll(int x, int y);
extern void gfxNCH4SetSplit(uword line);
void gfxLSInit(void);

void gfxLSPut(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutMsk(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutClr(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);
void gfxLSPutSet(MemRastPort *sp, U16 sx, U16 sy, U16 dx, U16 dy, U16 w, U16 h);

void gfxLSRectFill(U16 sx, U16 sy, U16 ex, U16 ey, U8 color);
U8 gfxLSReadPixel(U16 x, U16 y);
#endif

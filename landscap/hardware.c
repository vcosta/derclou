/*
**	$Filename: landscap/hardware.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**
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

#include <assert.h>

#include "base/base.h"

#include "landscap/landscap.h"
#include "landscap/landscap.ph"

static MemRastPort *lsPrepareFromMemBySize(ubyte uch_Size);
static MemRastPort *lsPrepareFromMem(LSObject lso);
void lsBlitOneObject(MemRastPort *rp, U16 offsetFact, U16 dx, U16 dy, U16 size);

uword us_ScrollX, us_ScrollY;

void lsShowEscapeCar(void)
{
    Building b = dbGetObject(ls->ul_BuildingID);

    livPrepareAnims();

    BobSet(ls->us_EscapeCarBobId, b->CarXPos, b->CarYPos,
	   LS_ESCAPE_CAR_X_OFFSET, LS_ESCAPE_CAR_Y_OFFSET);
    BobVis(ls->us_EscapeCarBobId);
}

static void lsSetAlarmPict(LSObject lso)
{
    U16 x0, x1, y0, y1, destx, desty;

    lsCalcExactSize(lso, &x0, &y0, &x1, &y1);

    if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM) {
        MemRastPort *rp;

	rp = lsPrepareFromMemBySize(16);

	destx = ((word) x1 - (word) x0 - 6) / 2 + x0 - 1;
	desty = ((word) y1 - (word) y0 - 7) / 2 + y0 + 2;

	lsBlitOneObject(rp, 24, destx, desty, 16);
    }
}

/* repaints all visible doors in a certain area */
static void lsRefreshClosedDoors(uword us_X0, uword us_Y0,
                                 uword us_X1, uword us_Y1)
{
    NODE *node;

    ls->uch_ShowObjectMask = 0x40;

    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsIsInside(lso, us_X0, us_Y0, us_X1, us_Y1))
	    if (lsIsObjectADoor(lso))
		if (lso->uch_Visible == LS_OBJECT_VISIBLE)
		    lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS,
				    LS_SHOW_DOOR);
    }

    ls->uch_ShowObjectMask = 0x0;
}

void lsRefreshStatue(LSObject lso)
/* dirty, dirty, violates pretty much all conventions for DerClou!
   and, unbelievably, should work nevertheless.
   This lso represents the statue, it is not displayed, we only
   put a pedestal at lso->us_DestX, lso->us_DestY.
   The source coordinates are not taken from the data (eek!) but
   directly from the image; to get the correct source image, the
   function lsPrepareFromMem is called with a dummy object */
{
    uword srcX, srcY, destX, destY, size;
    struct _LSObject dummy;	/* only passed to lsPrepareFromMem */
    MemRastPort *rp;

    ls->uch_ShowObjectMask = 0x40;

    size = 16;			/* is the size correct ?? */

    dummy.uch_Size = (ubyte) size;

    rp = lsPrepareFromMem(&dummy);

    srcX = 16;			/* take from the image and put here! */
    srcY = 0;

    destX = lso->us_DestX + 7;
    destY = lso->us_DestY + 10;

    gfxLSPutMsk(rp, srcX, srcY, destX, destY, size, size);

    ls->uch_ShowObjectMask = 0x0;
}

void lsFastRefresh(LSObject lso)
{
    U16 x0, x1, y0, y1;

    ls->uch_ShowObjectMask = 0x40;	/* ignore bit 6 */

    switch (lso->Type) {
    case Item_Holztuer:
    case Item_Stahltuer:
    case Item_Tresorraum:
    case Item_Mauertor:
	if (lso->ul_Status & (1 << Const_tcOPEN_CLOSE_BIT)) {
	    word x0, y0;

	    lsDoDoorRefresh(lso);

	    x0 = max((word) lso->us_DestX - 32, 0);
	    y0 = max((word) lso->us_DestY - 32, 0);

	    lsRefreshClosedDoors(x0, y0, lso->us_DestX + 32,
				 lso->us_DestY + 32);
	} else
	    lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
	break;

    case Item_Gemaelde:
    case Item_Bild:
	/* paintings and pictures are simply painted over with a gray RectFill
           (shadow colour) when they are taken */

	if (lso->uch_Visible == LS_OBJECT_VISIBLE)
	    lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
	else {
	    LSArea area = dbGetObject(lsGetActivAreaID());
	    ubyte color = LS_REFRESH_SHADOW_COLOR1;

	    lsCalcExactSize(lso, &x0, &y0, &x1, &y1);

	    if (area->uch_Darkness == LS_DARKNESS)
		color = LS_REFRESH_SHADOW_COLOR0;

            gfxLSRectFill(x0, y0, x1, y1, color);

	    lsSetAlarmPict(lso);
	}
	break;
    case Item_Statue:
	if (lso->uch_Visible == LS_OBJECT_VISIBLE)
	    lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
	else
	    lsRefreshStatue(lso);
	break;
    default:
        if (setup.Profidisk) {
            switch (lso->Type) {
            case Item_Heiligenstatue:
            case Item_Hottentotten_Figur:
            case Item_Batman_Figur:
            case Item_Dicker_Man:
            case Item_Unbekannter:
            case Item_Jack_the_Ripper_Figur:
            case Item_Koenigs_Figur:
            case Item_Wache_Figur:
            case Item_Miss_World_1952:
    	        if (lso->uch_Visible == LS_OBJECT_VISIBLE)
	            lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS,
			            LS_SHOW_ALL);
	        else
	            lsDoDoorRefresh(lso);
	        break;
            case Item_Postsack:
	        break;
            default:
    	        lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
                break;
            }
        } else {
	    lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_ALL);
        }
	break;
    }

    switch (lso->Type) {
    case Item_Fenster:
	lsSetAlarmPict(lso);
	break;

    default:
	break;
    }

    ls->uch_ShowObjectMask = 0;	/* copy all bits */
}

static MemRastPort *lsPrepareFromMemBySize(ubyte uch_Size)
{
    MemRastPort *rp = NULL;

    switch (uch_Size) {
    case 16:
        rp = &LS_COLL16_MEM_RP;
	break;
    case 32:
        rp = &LS_COLL32_MEM_RP;
	break;
    case 48:
        rp = &LS_COLL48_MEM_RP;
	break;
    }

    assert(rp);

    return rp;
}

/* put the collection with the image of an lso into the LS_PREPARE_BUFFER */
static MemRastPort *lsPrepareFromMem(LSObject lso)
{
    return lsPrepareFromMemBySize(lso->uch_Size);
}

void lsBlitOneObject(MemRastPort *rp, U16 offsetFact, U16 dx, U16 dy, U16 size)
{
    uword srcWidth = 288;	/* source screen is usually 288 wide */
    uword srcX, srcY, perRow;

    /* 32er objects are on a screen that is 320 pixels wide */
    if (size == 32)
	srcWidth = 320;

    perRow = srcWidth / size;

    srcY = (offsetFact / perRow) * size;
    srcX = (offsetFact % perRow) * size;

    if (ls->uch_ShowObjectMask)
        gfxLSPutMsk(rp, srcX, srcY, dx, dy, size, size);
    else
        gfxLSPutMsk(rp, srcX, srcY, dx, dy, size, size);

}

S32 lsShowOneObject(LSObject lso, word destx, word desty, U32 ul_Mode)
{
    Item item = dbGetObject(lso->Type);
    S32 show = 0;
    uword offsetFact;

    switch (lso->Type) {
    case Item_Sockel:		/* pedestal should not be displayed */
	break;

    default:
	if (ul_Mode & LS_SHOW_ALL)
	    show = 1;

	if ((!show) && (ul_Mode & LS_SHOW_WALL))
	    if (lsIsObjectAWall(lso))
		show = 1;

	/* only display doors if they are visible */

	if ((!show) && (ul_Mode & LS_SHOW_DOOR))
	    if (lsIsObjectADoor(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
		show = 1;

	if ((!show) && (ul_Mode & LS_SHOW_SPECIAL))
	    if (lsIsObjectSpecial(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
		show = 1;

	if ((!show) && (ul_Mode & LS_SHOW_OTHER_0))
	    if (lsIsObjectAStdObj(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
		show = 1;

	if ((!show) && (ul_Mode & LS_SHOW_OTHER_1))
	    if (lsIsObjectAnAddOn(lso) && lso->uch_Visible == LS_OBJECT_VISIBLE)
		show = 1;

	if (show) {
            MemRastPort *rp;
        
            rp = lsPrepareFromMem(lso);

	    offsetFact = item->OffsetFact + (lso->ul_Status & 3);

	    if (destx == LS_STD_COORDS)
		destx = lso->us_DestX;

	    if (desty == LS_STD_COORDS)
		desty = lso->us_DestY;

	    lsBlitOneObject(rp, offsetFact, destx, desty, item->Size);
	}
	break;
    }

    if (show)
	lsSetAlarmPict(lso);

    return show;
}

void lsBlitFloor(uword floorIndex, uword destx, uword desty)
{
    MemRastPort *rp = &LS_FLOOR_MEM_RP;
    uword srcX;

    srcX =
	((ls->p_CurrFloor[floorIndex].uch_FloorType) & 0xf) * LS_FLOOR_X_SIZE;

    gfxLSPut(rp, srcX, 0, destx, desty, LS_FLOOR_X_SIZE, LS_FLOOR_Y_SIZE);
}

void lsSetViewPort(uword x, uword y)
{
    gfxNCH4SetViewPort(x, y);
}

void lsInitGfx(void)
{
    gfxSetVideoMode(GFX_VIDEO_NCH4);

    lsSetViewPort(0, 0);
    gfxNCH4SetSplit(256);
}

void lsCloseGfx(void)
{
    gfxSetVideoMode(GFX_VIDEO_MCGA);

    tcSetPermanentColors();

    inpMousePtrOn();
}

void lsScrollCorrectData(S32 dx, S32 dy)
{
    ls->us_WindowXPos += dx;
    ls->us_WindowYPos += dy;

    ls->us_RasInfoScrollX = (word) dx;
    ls->us_RasInfoScrollY = (word) dy;

    livSetVisLScape(ls->us_WindowXPos, ls->us_WindowYPos);
}

void lsDoScroll(void)
{
    gfxNCH4Scroll(ls->us_RasInfoScrollX, ls->us_RasInfoScrollY);

    ls->us_RasInfoScrollX = 0;
    ls->us_RasInfoScrollY = 0;
}

/*
**	$Filename: landscap/raster.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	landscape raster functions for "Der Clou!"
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

#include "landscap/raster.h"

void lsShowRaster(U32 areaID, ubyte perc)
{
    LSArea area = dbGetObject(areaID);
    struct ObjectNode *node;
    S32 count, i;
    LIST *objects;

    gfxShow(154, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

    SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
    AskAll(area, ConsistOfRelationID, BuildObjectList);
    objects = ObjectListPrivate;

    /*lsSortObjectList(&objects);*/

    if (!(LIST_EMPTY(objects))) {
	count = (GetNrOfNodes(objects) * perc) / 255;

	for (node = (struct ObjectNode *) LIST_HEAD(objects), i = 0;
	     (NODE_SUCC((NODE *) node)) && (i < count);
	     node = (struct ObjectNode *) NODE_SUCC((NODE *) node), i++) {
	    LSObject lso = OL_DATA(node);

	    switch (lso->Type) {
	    case Item_Mauer:
	    case Item_Mauerecke:
	    case Item_Steinmauer:
		lsFadeRasterObject(areaID, lso, 1);
		break;
	    default:
		break;
	    }
	}

	for (node = (struct ObjectNode *) LIST_HEAD(objects), i = 0;
	     (NODE_SUCC((NODE *) node)) && (i < count);
	     node = (struct ObjectNode *) NODE_SUCC((NODE *) node), i++) {
	    LSObject lso = OL_DATA(node);

	    switch (lso->Type) {
	    case Item_Mauer:
	    case Item_Mauerecke:
	    case Item_Steinmauer:
		break;
	    default:
		lsFadeRasterObject(areaID, lso, 1);
		break;
	    }
	}
    } else
	Say(THECLOU_TXT, 0, MATT_PICTID, "KEIN_GRUNDRISS");

    RemoveList(objects);
}

NODE *lsGetSuccObject(NODE * start)
{
    NODE *n;
    LSObject lso;

    n = (NODE *) NODE_SUCC(start);

    while (NODE_SUCC(n)) {
	lso = (LSObject) OL_DATA(n);

	if (lso->ul_Status & (1L << Const_tcACCESS_BIT))
	    return (n);

	n = (NODE *) NODE_SUCC(n);
    }

    return start;
}

NODE *lsGetPredObject(NODE * start)
{
    NODE *n;
    LSObject lso;

    n = (NODE *) NODE_PRED(start);

    while (NODE_PRED(n)) {
	lso = (LSObject) OL_DATA(n);

	if (lso->ul_Status & (1L << Const_tcACCESS_BIT))
	    return (n);

	n = (NODE *) NODE_PRED(n);
    }

    return start;
}


void lsFadeRasterObject(U32 areaID, LSObject lso, ubyte status)
{
    U32 rasterXSize, rasterYSize, rasterSize, col;
    uword xStart, yStart, xEnd, yEnd;

    rasterXSize = lsGetRasterXSize(areaID);
    rasterYSize = lsGetRasterYSize(areaID);

    rasterSize = min(rasterXSize, rasterYSize);

    lsCalcExactSize(lso, &xStart, &yStart, &xEnd, &yEnd);

    xStart = ((xStart) * rasterSize) / LS_RASTER_X_SIZE;
    yStart = ((yStart) * rasterSize) / LS_RASTER_Y_SIZE;

    xEnd = ((xEnd) * rasterSize) / LS_RASTER_X_SIZE;
    yEnd = ((yEnd) * rasterSize) / LS_RASTER_Y_SIZE;

    xEnd = max(xStart + 3, xEnd);
    yEnd = max(yStart + 3, yEnd);

    if (status)
	col = (((Item) dbGetObject(lso->Type))->ColorNr);
    else
	col = 10;

    gfxSetPens(l_gc, col, col, col);

    gfxRectFill(l_gc, xStart, yStart, xEnd, yEnd);
}

void lsShowAllConnections(U32 areaID, NODE * node, ubyte perc)
{
    NODE *n;
    LSObject lso1, lso2;
    U32 relID = 0, col, destX, destY, srcX, srcY;
    U32 rasterXSize, rasterYSize, rasterSize;
    static ubyte Alarm_Power;

    lso1 = OL_DATA(node);

    rasterXSize = lsGetRasterXSize(areaID);
    rasterYSize = lsGetRasterYSize(areaID);

    rasterSize = min(rasterXSize, rasterYSize);

    switch (lso1->Type) {
    case Item_Alarmanlage_Z3:
    case Item_Alarmanlage_X3:
    case Item_Alarmanlage_Top:
	relID = hasAlarmRelationID;
	if (Alarm_Power & 1)
	    lsShowRaster(areaID, perc);
	Alarm_Power |= 1;
	break;
    case Item_Steuerkasten:
	relID = hasPowerRelationID;
	if (Alarm_Power & 2)
	    lsShowRaster(areaID, perc);
	Alarm_Power |= 2;
	break;
    default:
	break;
    }

    if (relID) {
	col = ((Item) dbGetObject(lso1->Type))->ColorNr;

	SetObjectListAttr(OLF_NORMAL, Object_LSObject);
	AskAll(lso1, relID, BuildObjectList);

	srcX =
	    lso1->us_DestX + lso1->uch_ExactX + (lso1->uch_ExactX1 -
						 lso1->uch_ExactX) / 2;
	srcY =
	    lso1->us_DestY + lso1->uch_ExactY + (lso1->uch_ExactY1 -
						 lso1->uch_ExactY) / 2;

	srcX = (srcX * rasterSize) / LS_RASTER_X_SIZE;
	srcY = (srcY * rasterSize) / LS_RASTER_Y_SIZE;

	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    uword x0, y0, x1, y1;

	    gfxSetPens(l_gc, col, GFX_SAME_PEN, GFX_SAME_PEN);

	    lso2 = OL_DATA(n);

	    lsCalcExactSize(lso2, &x0, &y0, &x1, &y1);

	    destX = x0 + (x1 - x0) / 2;
	    destY = y0 + (y1 - y0) / 2;

	    destX = (destX * rasterSize) / LS_RASTER_X_SIZE;
	    destY = (destY * rasterSize) / LS_RASTER_Y_SIZE;

	    gfxMoveCursor(l_gc, srcX, srcY);
	    gfxDraw(l_gc, destX, srcY);
	    gfxDraw(l_gc, destX, destY);

	    gfxSetPens(l_gc, 0, 0, col);
	    gfxRectFill(l_gc, destX - 1, destY - 1, destX + 2, destY + 2);
	}
    }
}

uword lsGetRasterXSize(U32 areaID)
{
    LSArea area = dbGetObject(areaID);

    return (uword) (LS_RASTER_DISP_WIDTH /
		    ((area->us_Width) / LS_RASTER_X_SIZE));
}

uword lsGetRasterYSize(U32 areaID)
{
    LSArea area = dbGetObject(areaID);

    return (uword) (LS_RASTER_DISP_HEIGHT /
		    ((area->us_Height) / LS_RASTER_Y_SIZE));
}

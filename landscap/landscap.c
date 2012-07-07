/*
**	$Filename: landscap/landscap.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-04-94
**
**	landscap functions for "Der Clou!"
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

#include "base/base.h"

#include "landscap/landscap.h"
#include "landscap/landscap.ph"

U32 ConsistOfRelationID = 0;
U32 hasLockRelationID = 0;
U32 hasAlarmRelationID = 0;
U32 hasPowerRelationID = 0;
U32 hasLootRelationID = 0;
U32 hasRoomRelationID = 0;
U32 FloorLinkRelationID = 0;

struct LandScape *ls = NULL;

void lsSafeRectFill(U16 x0, U16 y0, U16 x1, U16 y1, U8 color)
{
    x0 = min(x0, 638);
    x1 = min(x1, 639);

    y0 = min(y0, 254);
    y1 = min(y1, 255);

    if (x1 > x0 && y1 > y0)
        gfxLSRectFill(x0, y0, x1, y1, color);
}

void lsSetVisibleWindow(uword x, uword y)
{
    word halfX, halfY, wX, wY;

    halfX = LS_VISIBLE_X_SIZE / 2;
    halfY = LS_VISIBLE_Y_SIZE / 2;

    if ((wX = x - halfX) < 0)
	wX = 0;

    if ((wY = y - halfY) < 0)
	wY = 0;

    /* -1 is important, so that no position 320, 128 is possible     */
    /* (window from (320, 128) - (640, 256) instead of (639, 255) !!! */
    wX = min(wX, LS_MAX_AREA_WIDTH - LS_VISIBLE_X_SIZE - 1);
    wY = min(wY, LS_MAX_AREA_HEIGHT - LS_VISIBLE_Y_SIZE - 1);

    ls->us_WindowXPos = (uword) wX;
    ls->us_WindowYPos = (uword) wY;

    gfxNCH4SetViewPort(ls->us_WindowXPos, ls->us_WindowYPos);

    livSetVisLScape(ls->us_WindowXPos, ls->us_WindowYPos);
}

#ifdef THECLOU_DEBUG
static void lsShowRooms(void)
{
    if (GamePlayMode & GP_SHOW_ROOMS) {
	LIST *rooms = lsGetRoomsOfArea(ls->ul_AreaID);
	NODE *room;

	for (room = LIST_HEAD(rooms); NODE_SUCC(room); room = NODE_SUCC(room)) {
	    LSRoom myroom = OL_DATA(room);

            gfxLSRectFill(myroom->us_LeftEdge, myroom->us_TopEdge,
                          myroom->us_LeftEdge + myroom->us_Width - 1,
                          myroom->us_TopEdge + myroom->us_Height - 1,
                          249);

	    /* pcErrNewDebugMsg(ERR_WARNING, "tst", "ROOM X %d Y %d W %d H %d", myroom->us_LeftEdge, myroom->us_TopEdge, myroom->us_Width, myroom->us_Height); */
	}

	RemoveList(rooms);
    }
}
#endif

void lsBuildScrollWindow(void)
{
    S32 i, j;
    NODE *node;
    LSArea area = (LSArea) dbGetObject(ls->ul_AreaID);
    U8 palette[GFX_PALETTE_SIZE];

    gfxSetColorRange(0, 255);
    gfxChangeColors(u_gc, 0, GFX_FADE_OUT, 0);

    /* Boden aufbauen */
    for (i = 0; i < LS_FLOORS_PER_COLUMN; i++) {
	for (j = 0; j < LS_FLOORS_PER_LINE; j++) {
	    /* if no floor is available fill with collision colour */

	    if (LS_NO_FLOOR
		((ls->p_CurrFloor[i * LS_FLOORS_PER_LINE + j].uch_FloorType))) {
		lsSafeRectFill(j * 32, i * 32, j * 32 + 31, i * 32 + 31,
                    LS_COLLIS_COLOR_2);
	    } else
		lsBlitFloor((i * LS_FLOORS_PER_LINE + j), j * 32, i * 32);
	}
    }

    /* Objekte setzen - zuerst W„nde */
    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_WALL))
	    lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

    }

    /* dann andere */
    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_0))
	    lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
    }

    /* jetzt noch ein paar Sondef„lle (Kassa, Vase, ...) */
    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_1))
	    lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

	/* because of the dirty hack statues need to be refreshed specially */
	if (lso->Type == Item_Statue)
	    if (lso->uch_Visible != LS_OBJECT_VISIBLE)
		lsRefreshStatue(lso);
    }

    /* now refresh all doors and special objects */
    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsIsObjectADoor(lso))
	    lsInitDoorRefresh(OL_NR(node));
	else if (lsIsObjectSpecial(lso))
	    lsInitDoorRefresh(OL_NR(node));
    }

    /* finally build the doors and specials on the PC */
    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_SPECIAL))
	    lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
    }

    for (node = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	LSObject lso = OL_DATA(node);

	if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_DOOR)) {
	    if (!(lso->ul_Status & 1 << Const_tcOPEN_CLOSE_BIT))
		lsTurnObject(OL_DATA(node), lso->uch_Visible, LS_COLLISION);
	    else
		lsTurnObject(OL_DATA(node), lso->uch_Visible, LS_NO_COLLISION);
	}
    }

    /* und weiter gehts mit der Musi... */
    BobSetDarkness(area->uch_Darkness);
    lsSetDarkness(area->uch_Darkness);

    lsShowAllSpots(0, LS_ALL_VISIBLE_SPOTS);

    /* 32er Collection einblenden */
    gfxPrepareColl(area->us_Coll32ID);
    gfxGetPalette(area->us_Coll32ID, palette);
    gfxSetColorRange(0, 127);

    /* only copy the background ! */
    for (i = 64 * 3; i < 65 * 3; i++)
	palette[i] = palette[i - 64 * 3];

    /* calculate other colours (darken!) */
    for (i = 65 * 3; i < 128 * 3; i++) {
	palette[i] = palette[i - 64 * 3];	/* light */

	palette[i - 64 * 3] /= 2;	/* dark */
    }

    gfxChangeColors(NULL, 5, GFX_BLEND_UP, palette);

    /* now show colours of the maxis */
    gfxPrepareColl(137);
    gfxGetPalette(137, palette);
    gfxSetColorRange(128, 191);
    gfxChangeColors(NULL, 0, GFX_BLEND_UP, palette);

    /* and just to be safe also set the menu colours */
    tcSetPermanentColors();

#ifdef THECLOU_DEBUG
    lsShowRooms();
#endif
}

void lsSetDarkness(ubyte value)
{
    gfxSetDarkness(value);
}


void lsTurnObject(LSObject lso, ubyte status, ubyte Collis)
{
    uword floorIndex;

    floorIndex = lsGetFloorIndex(lso->us_DestX, lso->us_DestY);

    lso->uch_Visible = status;

    if (status == LS_OBJECT_VISIBLE)
	LS_SET_OBJECT(ls->p_CurrFloor[floorIndex].uch_FloorType);

    if (lso->Type == Item_Mikrophon)
	LS_SET_MICRO_ON_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType);
}

ubyte lsIsInside(LSObject lso, uword x, uword y, uword x1, uword y1)
{
    uword lsoX, lsoY, lsoX1, lsoY1;

    lsCalcExactSize(lso, &lsoX, &lsoY, &lsoX1, &lsoY1);

    if ((lsoX <= x1) && (lsoX1 >= x) && (lsoY <= y1) && (lsoY1 >= y))
	return (1);
    else
	return (0);
}

void lsSetActivLiving(char *Name, uword x, uword y)
{
    if (Name) {
	strcpy(ls->uch_ActivLiving, Name);

	if (x == (uword) - 1)
	    x = livGetXPos(Name);
	if (y == (uword) - 1)
	    y = livGetYPos(Name);

	lsSetVisibleWindow(x, y);
	livRefreshAll();

	ls->us_PersonXPos = (uword) (x - ls->us_WindowXPos);
	ls->us_PersonYPos = (uword) (y - ls->us_WindowYPos);
    }
}

void lsSetObjectState(U32 objID, ubyte bitNr, ubyte value)
{
    LSObject object = (LSObject) dbGetObject(objID);

    /* for a time clock the status must not change */
    if (object->Type != Item_Stechuhr) {
	if (value == 0)
	    object->ul_Status &= (0xffffffff - (1L << bitNr));

	if (value == 1)
	    object->ul_Status |= (1L << bitNr);
    }
}

static word lsSortByXCoord(struct ObjectNode *n1, struct ObjectNode *n2)
{
    LSObject lso1, lso2;

    lso1 = OL_DATA(n1);
    lso2 = OL_DATA(n2);

    if (lso1->us_DestX > lso2->us_DestX)
	return (1);
    else
	return ((word) - 1);
}

static word lsSortByYCoord(struct ObjectNode * n1, struct ObjectNode * n2)
{
    LSObject lso1, lso2;

    lso1 = OL_DATA(n1);
    lso2 = OL_DATA(n2);

    if (lso1->us_DestY < lso2->us_DestY)
	return (1);
    else
	return ((word) - 1);
}

static void lsSortObjectList(LIST ** l)
{
    LSObject lso1, lso2;
    NODE *node, *node1, *next;
    ubyte lastNode = 0;

    if (!(LIST_EMPTY((*l)))) {
	dbSortObjectList(l, lsSortByYCoord);

	for (node = (NODE *) LIST_HEAD((*l));
	     (!(lastNode)) && NODE_SUCC(NODE_SUCC(node));) {
	    node1 = node;

	    do {
		node1 = (NODE *) NODE_SUCC(node1);
		lso1 = OL_DATA(node);
		lso2 = OL_DATA(node1);
	    }
	    while ((lso1->us_DestY == lso2->us_DestY)
		   && NODE_SUCC(NODE_SUCC(node1)));

	    next = node1;

	    /* wenn Abbruch wegen NODE_SUCC(NODE_SUCC(.. erflogte, darf
	     * nicht der NODE_PRED(node1) genomen werden!
	     * nach dem Sortieren ist auáerdem Schluá!
	     */

	    if ((lso1->us_DestY != lso2->us_DestY))
		node1 = (NODE *) NODE_PRED(node1);
	    else
		lastNode = 1;

	    if (node != node1) {
		dbSortPartOfList((*l), (struct ObjectNode *) node,
				 (struct ObjectNode *) node1, lsSortByXCoord);
		node = next;
	    } else
		node = (NODE *) NODE_SUCC(node);
	}
    }
}

void lsRefreshObjectList(U32 areaID)
{
    SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
    AskAll(dbGetObject(areaID), ConsistOfRelationID, BuildObjectList);
    ls->p_ObjectRetrieval = ObjectListPrivate;

    lsSortObjectList(&ls->p_ObjectRetrieval);
}

U32 lsAddLootBag(uword x, uword y, ubyte bagNr)
{				/* bagNr : 1 - 8! */
    LSObject lso = dbGetObject(9700 + bagNr);

    lso->uch_Visible = LS_OBJECT_VISIBLE;

    /* add loot bag */
    if (!hasLootBag(ls->ul_AreaID, (U32) (9700 + bagNr))) {
	lso->us_DestX = x;
	lso->us_DestY = y;

	BobSet(lso->us_OffsetFact, x, y, LS_LOOTBAG_X_OFFSET,
	       LS_LOOTBAG_Y_OFFSET);
	BobVis(lso->us_OffsetFact);

	hasLootBagSet(ls->ul_AreaID, (U32) (9700 + bagNr));
    }

    return ((U32) (9700 + bagNr));
}

void lsRemLootBag(U32 bagId)
{
    LSObject lso = dbGetObject(bagId);

    lso->uch_Visible = LS_OBJECT_INVISIBLE;

    BobInVis(lso->us_OffsetFact);

    hasLootBagUnSet(ls->ul_AreaID, bagId);
}

void lsRefreshAllLootBags(void)
{
    U32 i;

    livPrepareAnims();

    for (i = 1; i < 9; i++) {
	LSObject lso = dbGetObject(9700 + i);

	if ((lso->uch_Visible == LS_OBJECT_VISIBLE)
	    && (hasLootBag(ls->ul_AreaID, (U32) (9700 + i)))) {
	    BobSet(lso->us_OffsetFact, lso->us_DestX, lso->us_DestY,
		   LS_LOOTBAG_X_OFFSET, LS_LOOTBAG_Y_OFFSET);
	    BobVis(lso->us_OffsetFact);
	}
    }
}

void lsGuyInsideSpot(uword * us_XPos, uword * us_YPos, U32 * areaId)
{
    LIST *spots = lsGetSpotList();
    struct Spot *s;
    S32 i;

    for (s = (struct Spot *) LIST_HEAD(spots); NODE_SUCC(s);
	 s = (struct Spot *) NODE_SUCC(s)) {
	if (s->uch_Status & LS_SPOT_ON) {
	    for (i = 0; i < 4; i++) {
		if ((us_XPos[i] != (uword) - 1) && (us_YPos[i] != (uword) - 1)) {
		    if (areaId[i] == s->ul_AreaId) {
			if (((struct Spot *) s)->p_CurrPos) {
			    S32 x = s->p_CurrPos->us_XPos;	/* linke, obere */
			    S32 y = s->p_CurrPos->us_YPos;	/* Ecke des Spot! */
			    S32 size = s->us_Size - 4;

			    if ((us_XPos[i] < x + size - 2)
				&& (us_XPos[i] > x + 2))
				if ((us_YPos[i] < y + size - 2)
				    && (us_YPos[i] > y + 2))
				    Search.SpotTouchCount[i]++;
			}
		    }
		}
	    }
	}
    }
}

void lsWalkThroughWindow(LSObject lso, uword us_LivXPos, uword us_LivYPos,
			 uword * us_XPos, uword * us_YPos)
{
    word deltaX, deltaY;

    (*us_XPos) = us_LivXPos;
    (*us_YPos) = us_LivYPos;

    if (us_LivXPos < lso->us_DestX)
	deltaX = 25;
    else
	deltaX = -25;

    if (us_LivYPos < lso->us_DestY)
	deltaY = 25;
    else
	deltaY = -25;

    if (lso->ul_Status & (1 << Const_tcHORIZ_VERT_BIT))
	(*us_XPos) += deltaX;	/* vertikal */
    else
	(*us_YPos) += deltaY;	/* horizontal */
}

void lsPatchObjects(void)
	/* for some objects the LDesigner does not set the status bits correctly,
           so we need to patch them here at runtime */
{
    NODE *n;
    LSObject lso;
    Item item;

    ((Item) dbGetObject(Item_Fenster))->OffsetFact = 16;

    for (n = (NODE *) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	lso = OL_DATA(n);

	item = dbGetObject(lso->Type);

	lso->uch_Size = item->Size;	/* sizes are the same everywhere! */

	switch (lso->Type) {
	case Item_Statue:
	    lso->ul_Status |= (1L << Const_tcACCESS_BIT);
	    break;
	case Item_WC:
	case Item_Kuehlschrank:
	case Item_Nachtkaestchen:
	    lso->ul_Status |= (1L << Const_tcLOCK_UNLOCK_BIT);	/* unlocked! */
	    break;
	case Item_Steinmauer:
	case Item_Tresen:
	case Item_Vase:
	case Item_Sockel:
	    lso->ul_Status &= (~(1L << Const_tcACCESS_BIT));	/* Steinmauern kein Access */
	    break;
	default:
	    break;
	}

        if (setup.Profidisk) {
	    switch (lso->Type) {
	    case Item_Beichtstuhl:
	    case Item_Postsack:
	        lso->ul_Status |= (1L << Const_tcLOCK_UNLOCK_BIT);
                /* unlocked! */
	        break;
	    case Item_Leiter:
	        lso->ul_Status &= (~(1L << Const_tcACCESS_BIT));
                /* Steinmauern kein Access */
	        break;
	    default:
	        break;
	    }
        }

	if (OL_NR(n) == tcLAST_BURGLARY_LEFT_CTRL_OBJ)
	    lso->ul_Status |= (1 << Const_tcON_OFF);

	lsSetOldState(lso);	/* muá sein! */

	/* change for correcting amiga plans (corrects walls) before using pc level desinger
	   lso->us_DestX -= 9;
	   lso->us_DestY += 17;
	 */
    }
}

void lsCalcExactSize(LSObject lso, uword * x0, uword * y0, uword * x1,
		     uword * y1)
{
    Item item = dbGetObject(lso->Type);
    ubyte vertical = 0;

    (*x0) = lso->us_DestX;
    (*y0) = lso->us_DestY;

    /* no idea why OPEN_CLOSE_BIT & HORIZ_VERT_BIT are swapped
       for painting and image, but they are */
    if ((lso->Type == Item_Bild) || (lso->Type == Item_Gemaelde))
	vertical = lso->ul_Status & 3;
    else
	vertical = lso->ul_Status & 1;

    if (vertical)		/* vertical */
    {
	(*x0) += item->VExactXOffset;
	(*y0) += item->VExactYOffset;

	(*x1) = (*x0) + item->VExactWidth;
	(*y1) = (*y0) + item->VExactHeight;
    } else {
	(*x0) += item->HExactXOffset;
	(*y0) += item->HExactYOffset;

	(*x1) = (*x0) + item->HExactWidth;
	(*y1) = (*y0) + item->HExactHeight;
    }
}

void lsInitDoorRefresh(U32 ObjId)
	/* copies a background that is covered by a door into a mem buffer */
{
    LSObject lso = dbGetObject(ObjId);
    uword width, height, destX, destY;
    struct LSDoorRefreshNode *drn;
    ubyte found = 0;

    for (drn = (struct LSDoorRefreshNode *) LIST_HEAD(ls->p_DoorRefreshList);
	 NODE_SUCC(drn); drn = (struct LSDoorRefreshNode *) NODE_SUCC(drn))
	if (drn->lso == lso)
	    found = 1;

    if (!found) {
	width = lso->uch_Size;
	height = lso->uch_Size;

	destX = ls->us_DoorXOffset;
	destY = ls->us_DoorYOffset;

        {
            Rect srcR, dstR;

            srcR.x = lso->us_DestX;
            srcR.y = lso->us_DestY;
            srcR.w = width;
            srcR.h = height;

            dstR.x = destX;
            dstR.y = destY;
            dstR.w = width;
            dstR.h = height;

            MemBlit(&LSRPInMem, &srcR, &LS_DOOR_REFRESH_MEM_RP, &dstR,
                GFX_ROP_BLIT);
        }

	drn = (struct LSDoorRefreshNode *)
	    CreateNode(ls->p_DoorRefreshList, sizeof(*drn), NULL);

	drn->lso = lso;

	drn->us_XOffset = ls->us_DoorXOffset;
	drn->us_YOffset = ls->us_DoorYOffset;

	if ((ls->us_DoorXOffset + width) >= 320) {
	    if (ls->us_DoorYOffset <= 128)	/* prevent overflow */
	    {
		ls->us_DoorXOffset = 0;
		ls->us_DoorYOffset += height;
	    }
	} else
	    ls->us_DoorXOffset += width;
    }
}

void lsDoDoorRefresh(LSObject lso)
	/* restore the background of a door from a mem buffer */
{
    struct LSDoorRefreshNode *drn;
    uword width, height;

    for (drn = (struct LSDoorRefreshNode *) LIST_HEAD(ls->p_DoorRefreshList);
	 NODE_SUCC(drn); drn = (struct LSDoorRefreshNode *) NODE_SUCC(drn))
	if (drn->lso == lso)
	    break;

    width = lso->uch_Size;
    height = lso->uch_Size;

    gfxLSPut(&LS_DOOR_REFRESH_MEM_RP, drn->us_XOffset, drn->us_YOffset,
        lso->us_DestX, lso->us_DestY, width, height);
}

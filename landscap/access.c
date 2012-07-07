/*
**	$Filename: landscap/access.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-04-94
**
**	landscap access functions for "Der Clou!"
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

bool lsIsLSObjectInActivArea(LSObject lso)
{
    if (AskP
	(dbGetObject(ls->ul_AreaID), ConsistOfRelationID, lso, NO_PARAMETER,
	 CMP_NO))
	return true;

    return false;
}

void lsSetObjectRetrievalList(U32 ul_AreaId)
{
    S32 i;

    for (i = 0; i < 3; i++)
	if (ul_AreaId == ls->ul_ObjectRetrievalAreaId[i])
	    ls->p_ObjectRetrieval = ls->p_ObjectRetrievalLists[i];
}

U32 lsGetCurrObjectRetrieval(void)
{
    S32 i;

    for (i = 0; i < 3; i++)
	if (ls->p_ObjectRetrieval == ls->p_ObjectRetrievalLists[i])
	    return ls->ul_ObjectRetrievalAreaId[i];

    return 0;
}

bool lsIsObjectAStdObj(LSObject lso)
{
    if (lsIsObjectADoor(lso))
	return false;

    if (lsIsObjectAWall(lso))
	return false;

    if (lsIsObjectAnAddOn(lso))
	return false;

    if (lsIsObjectSpecial(lso))
	return false;

    return true;
}

bool lsIsObjectADoor(LSObject lso)
{
    switch (lso->Type) {
    case Item_Holztuer:
    case Item_Stahltuer:
    case Item_Mauertor:
    case Item_Tresorraum:
	return true;
    }

    return false;
}

bool lsIsObjectAWall(LSObject lso)
{
    switch (lso->Type) {
    case Item_Mauer:
    case Item_Mauerecke:
    case Item_Steinmauer:
    case Item_Sockel:
	return true;
    }

    if (setup.Profidisk) {
        switch (lso->Type) {
        case Item_verzierte_Saeule:
        case Item_Gelaender:
        case Item_Absperrung:
        case Item_Postsack:
        case Item_Lokomotive_vorne_rechts:
        case Item_Lokomotive_vorne_links:
        case Item_Lokomotive_oben:
        case Item_Lokomotive_seitlich:
        case Item_Lokomotive_Kabine:
        case Item_Lokomotive_Tuer:
	    return true;
        }
    }

    return false;
}

/* objects which need the same refresh as doors */
bool lsIsObjectSpecial(LSObject lso)
{
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
	    return true;
        }
    }

    return false;
}


bool lsIsObjectAnAddOn(LSObject lso)
{
    switch (lso->Type) {
    case Item_Kasse:
    case Item_Vase:
    case Item_Statue:
    case Item_Kreuz:
    case Item_Kranz:
	return true;
    }

    return false;
}

uword lsGetObjectCount(void)
{
    return ((uword) GetNrOfNodes(ls->p_ObjectRetrieval));
}

ubyte lsGetLoudness(uword x, uword y)
{
    word floorIndex = lsGetFloorIndex(x, y), i, j, k;

    /* Ursprnglich wurde loudness hier mit MaxVolume initialisiert    */
    /* dadurch waren in der Anzeige der Loudness auch die Nachbarn     */
    /* inbegriffen waren. Nebenwirkung: Es gab aber pl”tzlich, sobald  */
    /* man die Lautst„rke berschritt Alarm durch Mikrophone ->        */
    /* tcAlarmByMicro, bei einer Žnderung der Initialiserung, muá auch */
    /* TcAlarmByMicro ge„ndert werden.   (hg, 14-02-94)                */
    ubyte loudness = 255;

    if ((!LS_NO_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType)) &&
	LS_IS_MICRO_ON_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType))
	loudness = 15;
    else {
	for (i = -1; (i < 2) && (loudness == 255); i++) {
	    for (j = -1; (j < 2) && (loudness == 255); j++) {
		k = floorIndex + i * LS_FLOORS_PER_LINE + j;

		if ((k >= 0) && (k < LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN)) {
		    if ((!LS_NO_FLOOR(ls->p_CurrFloor[k].uch_FloorType)) &&
			LS_IS_MICRO_ON_FLOOR(ls->p_CurrFloor[k].uch_FloorType))
			loudness = 50;
		}
	    }
	}
    }

    return loudness;
}

U32 lsGetObjectState(U32 objID)
{
    LSObject obj = dbGetObject(objID);

    return (lsGetNewState(obj));
}

U32 lsGetStartArea(void)
{
    U32 areaID;			/* attention, planing has to be changed to! */

    startsWithAll(ls->ul_BuildingID, OLF_NORMAL, Object_LSArea);

    areaID = OL_NR(LIST_HEAD(ObjectList));

    return (areaID);
}

uword lsGetFloorIndex(uword x, uword y)
{
    register uword fpl, row, line;

    fpl = LS_FLOORS_PER_LINE;

    row = x / LS_FLOOR_X_SIZE;
    line = y / LS_FLOOR_Y_SIZE;

    return (uword) (line * fpl + row);
}

static void lsExtendGetList(LIST * list, U32 nr, U32 type, void *data)
{
    struct ObjectNode *new =
	dbAddObjectNode(list, type, OLF_INCLUDE_NAME | OLF_INSERT_STAR);

    new->nr = nr;
    new->type = type;
    new->data = data;
}

LIST *lsGetObjectsByList(uword x, uword y, uword width, uword height,
			 ubyte showInvisible, ubyte addLootBags)
{
    struct ObjectNode *node;
    LIST *list = CreateList();
    U32 i;

    /* diverse Objekte eintragen */
    for (node = (struct ObjectNode *) LIST_HEAD(ls->p_ObjectRetrieval);
	 NODE_SUCC((NODE *) node);
	 node = (struct ObjectNode *) NODE_SUCC((NODE *) node)) {
	LSObject lso = (LSObject) OL_DATA(node);

	if ((lso->ul_Status & (1L << Const_tcACCESS_BIT))
	    || (GamePlayMode & GP_LEVEL_DESIGN))
	    if (showInvisible || lso->uch_Visible)
		if (lsIsInside(lso, x, y, x + width, y + height))
		    lsExtendGetList(list, OL_NR(node), lso->Type, lso);
    }

    /* Ausnahme: Beutesack eintragen! */
    if (addLootBags) {
	for (i = 9701; i <= 9708; i++) {
	    LSObject lso = dbGetObject(i);

	    if (lso->uch_Visible == LS_OBJECT_VISIBLE)
		if (lsIsInside(lso, x, y, x + width, y + height))
		    lsExtendGetList(list, i, Item_Beutesack, lso);
	}
    }

    return (list);
}

uword lsGetWindowXPos(void)
{
    return (ls->us_WindowXPos);
}

uword lsGetWindowYPos(void)
{
    return (ls->us_WindowYPos);
}

uword lsGetTotalXPos(void)
{
    return (uword) (ls->us_WindowXPos + ls->us_PersonXPos);
}

uword lsGetTotalYPos(void)
{
    return (uword) (ls->us_WindowYPos + ls->us_PersonYPos);
}

void lsSetCollMode(ubyte collMode)
{
    ls->uch_CollMode = collMode;
}

U32 lsGetCurrBuildingID(void)
{
    return ls->ul_BuildingID;
}

U32 lsGetActivAreaID(void)
{
    return (ls->ul_AreaID);
}

LIST *lsGetRoomsOfArea(U32 ul_AreaId)
{
    LSArea area = (LSArea) dbGetObject(ul_AreaId);
    U32 roomRelId = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;
    NODE *room;

    SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSRoom);
    AskAll(area, roomRelId, BuildObjectList);


    for (room = LIST_HEAD(ObjectListPrivate); NODE_SUCC(room);
	 room = NODE_SUCC(room)) {
	LSRoom myroom = OL_DATA(room);

	if ((word) myroom->us_LeftEdge < 0)
	    myroom->us_LeftEdge = 0;

	if ((word) myroom->us_TopEdge < 0)
	    myroom->us_TopEdge = 0;
    }

    return ObjectListPrivate;
}

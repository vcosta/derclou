/*
**	$Filename: scenes/inside.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	 functions for inside of houses for "Der Clou!"
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

#include "scenes/scenes.h"

U32 CurrAreaId;
LSObject CurrLSO;		/* for FadeObjectInside */

static void FadeInsideObject(void)
{
    static ubyte status = 1;

    lsFadeRasterObject(CurrAreaId, CurrLSO, (status++) % 2);
}

U32 tcGoInsideOfHouse(U32 buildingID)
{
    LIST *menu = txtGoKey(MENU_TXT, "INSIDE_MENU"), *areas;
    U32 count, i, areaID = 0;

    consistsOfAll(buildingID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME,
		  Object_LSArea);
    areas = ObjectListPrivate;
    dbSortObjectList(&areas, dbStdCompareObjects);

    count = GetNrOfNodes(areas) + 1;

    if (count > 2) {
	for (i = count; i < 5; i++)
	    RemoveNode(menu, NODE_NAME(GetNthNode(menu, count)));

	ShowMenuBackground();

	inpTurnFunctionKey(0);
	inpTurnESC(0);
	i = (U32) Menu(menu, (1L << count) - 1, 0, 0L, 0L);
	inpTurnESC(1);
	inpTurnFunctionKey(1);

	consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

	if (i)
	    areaID = OL_NR(GetNthNode(areas, i - 1));
    }

    ShowMenuBackground();
    ShowTime(0);

    RemoveList(areas);
    RemoveList(menu);

    return (areaID);
}

void tcInsideOfHouse(U32 buildingID, U32 areaID, ubyte perc)
{
    LIST *objects;
    NODE *node, *n;
    U32 action = 0, count;
    char name[TXT_KEY_LENGTH];
    char alarm[TXT_KEY_LENGTH], power[TXT_KEY_LENGTH];
    LSObject lso;
    LSArea area = dbGetObject(areaID);
    LIST *menu = txtGoKey(MENU_TXT, "LookMenu");

    txtGetFirstLine(BUSINESS_TXT, "PROTECTED", alarm);
    txtGetFirstLine(BUSINESS_TXT, "SUPPLIED", power);

    inpTurnESC(0);
    inpTurnFunctionKey(0);

    /* liste und node initialisieren */
    SetObjectListAttr(OLF_PRIVATE_LIST, Object_LSObject);
    AskAll(area, ConsistOfRelationID, BuildObjectList);
    objects = ObjectListPrivate;

    /*lsSortObjectList(&objects);*/

    count = (GetNrOfNodes(objects) * perc) / 255;
    node = lsGetSuccObject((NODE *) LIST_HEAD(objects));

    CurrAreaId = areaID;

    if ((GetNodeNrByAddr(objects, node)) > count) {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0, MATT_PICTID, "CANT_LOOK");
    } else {
	while (action != 4) {
	    lso = (LSObject) OL_DATA(node);

	    dbGetObjectName(lso->Type, name);

	    if (lso->uch_Chained) {
		strcat(name, "(");
		if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM) {
		    strcat(name, alarm);
		    if (lso->uch_Chained & Const_tcCHAINED_TO_POWER) {
			strcat(name, ", ");
			strcat(name, power);
		    }
		} else {
		    if (lso->uch_Chained & Const_tcCHAINED_TO_POWER)
			strcat(name, power);
		}

		strcat(name, ")");
	    }
#ifdef THECLOU_DEBUG
	    {
		char debugtxt[TXT_KEY_LENGTH];

		sprintf(debugtxt, " ObjNr=%" PRId32, OL_NR(node));
		strcat(name, debugtxt);
	    }
#endif

	    ShowMenuBackground();
	    PrintStatus(name);

	    CurrLSO = lso;

	    SetMenuTimeOutFunc(FadeInsideObject);

	    inpTurnFunctionKey(0);
	    inpTurnESC(0);
	    action = Menu(menu, 31, (ubyte) action, 0, 10);
	    inpTurnESC(1);
	    inpTurnFunctionKey(1);

	    lsFadeRasterObject(areaID, lso, 1);

	    switch (action) {
	    case 0:
		if ((GetNodeNrByAddr(objects, n = lsGetSuccObject(node))) <
		    (count - 1))
		    node = n;
		break;
	    case 1:
		node = lsGetPredObject(node);
		break;
	    case 2:
		tcShowObjectData(areaID, node, perc);
		break;
	    case 3:
		isGuardedbyAll(buildingID, OLF_NORMAL, Object_Police);
		if (!(LIST_EMPTY(ObjectList))) {
		    gfxSetPens(l_gc, 4, GFX_SAME_PEN, GFX_SAME_PEN);
		    grdDraw(l_gc, buildingID, areaID);
		} else
		    Say(BUSINESS_TXT, 0, MATT_PICTID, "NO_GUARD");
		break;
	    case 4:
		break;
	    default:
		action = 0;
		break;
	    }

	    lso = (LSObject) OL_DATA(node);
	    lsFadeRasterObject(areaID, lso, 1);
	}
    }

    RemoveList(menu);
    RemoveList(objects);
    inpSetWaitTicks(0);
}

void tcShowObjectData(U32 areaID, NODE * node, ubyte perc)
{
    NODE *n;

    /* Objekt selbst pr„sentieren */
    Present(OL_NR(node), "RasterObject", InitObjectPresent);

    /* Inhalt zusammenstellen */
    SetObjectListAttr(OLF_NORMAL, Object_Loot);
    AskAll(OL_DATA(node), hasLootRelationID, BuildObjectList);

    if (!LIST_EMPTY(ObjectList)) {
	/* alle Loots durchgehen und anzeigen! */
	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    /* zur Variablenbergabe... (DIRTY, DIRTY...) */
	    SetP(OL_DATA(n), hasLootRelationID, OL_DATA(n),
		 GetP(OL_DATA(node), hasLootRelationID, OL_DATA(n)));

	    Present(OL_NR(n), "RasterObject", InitOneLootPresent);

	    /* kill these dirty things! */
	    UnSet(OL_DATA(n), hasLootRelationID, OL_DATA(n));
	}
    }

    /* Vebindungen zeigen! */
    lsShowAllConnections(areaID, node, perc);
}

/*
**      $Filename: planing/support.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.support for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "base/base.h"

#include "planing/support.h"


/* loot support */
U32 plGetNextLoot(void)
{
    register ubyte i;

    for (i = 0; i < PLANING_NR_LOOTS; i++) {
	if (!Planing_Loot[i]) {
	    Planing_Loot[i] = 1;
	    return lsAddLootBag(livGetXPos(Planing_Name[CurrentPerson]),
				livGetYPos(Planing_Name[CurrentPerson]) + 5,
				i + 1);
	}
    }

    return 0L;
}

/* check support */
ubyte plLivingsPosAtCar(U32 bldId)
{
    Building bldObj = (Building) dbGetObject(bldId);
    uword xpos, ypos, carxpos, carypos;
    ubyte i, atCar = 1;

    carxpos = bldObj->CarXPos + 8;
    carypos = bldObj->CarYPos + 8;

    startsWithAll(bldId, OLF_NORMAL, Object_LSArea);

    for (i = 0; i < BurglarsNr; i++) {
	xpos = livGetXPos(Planing_Name[i]);
	ypos = livGetYPos(Planing_Name[i]);

	if (livWhereIs(Planing_Name[i]) != OL_NR(LIST_HEAD(ObjectList))) {
	    atCar = 0;
	    break;
	}

	if (((xpos < (carxpos - PLANING_AREA_CAR))
	     || (xpos > (carxpos + PLANING_AREA_CAR)))
	    && ((ypos < (carypos - PLANING_AREA_CAR))
		|| (ypos > (carypos + PLANING_AREA_CAR)))) {
	    atCar = 0;
	    break;
	}
    }

    return atCar;
}

ubyte plAllInCar(U32 bldId)
{
    ubyte maxPerson = 0, i, ret = 1;
    U32 maxTimer = 0L, oldTimer = CurrentTimer(plSys), realCurrentPerson =
	CurrentPerson;

    for (i = 0; i < BurglarsNr; i++) {
	SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, i)));
	if (maxTimer < GetMaxTimer(plSys)) {
	    maxTimer = GetMaxTimer(plSys);
	    maxPerson = i;
	}
    }

    SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, maxPerson)));

    if (maxPerson != realCurrentPerson) {
	SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, maxPerson)));
	plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
	plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
	       GetMaxTimer(plSys) - oldTimer, 1);
    }

    if (!plLivingsPosAtCar(bldId))
	ret = 0;

    CurrentPerson = realCurrentPerson;

    if (maxPerson != realCurrentPerson) {
	SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, CurrentPerson)));
	plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
	plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
	       maxTimer - GetMaxTimer(plSys), 0);
    }

    return ret;
}

ubyte plIsStair(U32 objId)
{
    return (ubyte) ((((LSObject) dbGetObject(objId))->Type == Item_Treppe));
}

/* special items support */
void plCorrectOpened(LSObject obj, ubyte open)
{
    switch (obj->Type) {
    case Item_Holztuer:
    case Item_Stahltuer:
    case Item_Tresorraum:
    case Item_Mauertor:
	if (open)
	    lsTurnObject(obj, LS_OBJECT_INVISIBLE, LS_NO_COLLISION);
	else
	    lsTurnObject(obj, LS_OBJECT_VISIBLE, LS_COLLISION);

	livRefreshAll();
	break;
    }
}

ubyte plIgnoreLock(U32 objId)
{
    ubyte back = 0;

    switch (((LSObject) dbGetObject(objId))->Type) {
    case Item_Alarmanlage_Z3:
	back = PLANING_ALARM_Z3;
	break;

    case Item_Alarmanlage_X3:
	back = PLANING_ALARM_X5;
	break;

    case Item_Alarmanlage_Top:
	back = PLANING_ALARM_TOP3;
	break;

    case Item_Steuerkasten:
	back = PLANING_POWER;
	break;
    }

    return back;
}

/* livings support */
void plMove(U32 current, ubyte direction)
{
    switch (direction) {
    case DIRECTION_LEFT:
	livAnimate(Planing_Name[current], ANM_MOVE_LEFT,
		   -1 * LS_STD_SCROLL_SPEED, 0);
	break;

    case DIRECTION_RIGHT:
	livAnimate(Planing_Name[current], ANM_MOVE_RIGHT,
		   1 * LS_STD_SCROLL_SPEED, 0);
	break;

    case DIRECTION_UP:
	livAnimate(Planing_Name[current], ANM_MOVE_UP, 0,
		   -1 * LS_STD_SCROLL_SPEED);
	break;

    case DIRECTION_DOWN:
	livAnimate(Planing_Name[current], ANM_MOVE_DOWN, 0,
		   1 * LS_STD_SCROLL_SPEED);
	break;
    }
}

void plWork(U32 current)
{
    switch (livGetViewDirection(Planing_Name[current])) {
    case ANM_MOVE_LEFT:
	livAnimate(Planing_Name[current], ANM_WORK_LEFT, 0, 0);
	break;

    case ANM_MOVE_RIGHT:
	livAnimate(Planing_Name[current], ANM_WORK_RIGHT, 0, 0);
	break;

    case ANM_MOVE_UP:
	livAnimate(Planing_Name[current], ANM_WORK_UP, 0, 0);
	break;

    case ANM_MOVE_DOWN:
	livAnimate(Planing_Name[current], ANM_WORK_DOWN, 0, 0);
	break;
    }
}

LIST *plGetObjectsList(U32 current, ubyte addLootBags)
{
    LIST *list = NULL;
    U32 areaId = livWhereIs(Planing_Name[current]), oldAreaId;

    oldAreaId = lsGetCurrObjectRetrieval();
    lsSetObjectRetrievalList(areaId);

    switch (livGetViewDirection(Planing_Name[current])) {
    case ANM_MOVE_LEFT:
	list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) - 9,
				  livGetYPos(Planing_Name[current]) + 2,
				  14, 12, 1, addLootBags);
	break;

    case ANM_MOVE_RIGHT:
	list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 9,
				  livGetYPos(Planing_Name[current]) + 2,
				  14, 12, 1, addLootBags);
	break;

    case ANM_MOVE_UP:
	list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 2,
				  livGetYPos(Planing_Name[current]) - 9,
				  12, 14, 1, addLootBags);
	break;

    case ANM_MOVE_DOWN:
	list = lsGetObjectsByList(livGetXPos(Planing_Name[current]) + 2,
				  livGetYPos(Planing_Name[current]) + 9,
				  12, 14, 1, addLootBags);
	break;
    }

    lsSetObjectRetrievalList(oldAreaId);
    return list;
}

void plInsertGuard(LIST * list, U32 current, U32 guard)
{
    switch (livGetViewDirection(Planing_Name[current])) {
    case ANM_MOVE_LEFT:
	tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		      livGetXPos(Planing_Name[current]) - 9,
		      livGetYPos(Planing_Name[current]),
		      14, 14,
		      OL_NR(GetNthNode(PersonsList, guard)),
		      guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		      livWhereIs(Planing_Name[current]));
	break;

    case ANM_MOVE_RIGHT:
	tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		      livGetXPos(Planing_Name[current]) + 9,
		      livGetYPos(Planing_Name[current]),
		      14, 14,
		      OL_NR(GetNthNode(PersonsList, guard)),
		      guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		      livWhereIs(Planing_Name[current]));
	break;

    case ANM_MOVE_UP:
	tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		      livGetXPos(Planing_Name[current]),
		      livGetYPos(Planing_Name[current]) - 9,
		      14, 14,
		      OL_NR(GetNthNode(PersonsList, guard)),
		      guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		      livWhereIs(Planing_Name[current]));
	break;

    case ANM_MOVE_DOWN:
	tcInsertGuard(list, Planing_GuardRoomList[guard - BurglarsNr],
		      livGetXPos(Planing_Name[current]),
		      livGetYPos(Planing_Name[current]) + 9,
		      14, 14,
		      OL_NR(GetNthNode(PersonsList, guard)),
		      guard + 1 + (PLANING_NR_PERSONS - BurglarsNr),
		      livWhereIs(Planing_Name[current]));
	break;
    }
}

ubyte plObjectInReach(U32 current, U32 objId)
{
    LIST *actionList = plGetObjectsList(current, 1);
    ubyte i, ret = 0;

    for (i = BurglarsNr; i < PersonsNr; i++)
	plInsertGuard(actionList, current, i);

    if (!LIST_EMPTY(actionList)) {
	if (dbHasObjectNode(actionList, objId))
	    ret = 1;
    }

    RemoveList(actionList);

    return ret;
}

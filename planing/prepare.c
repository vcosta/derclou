/*
**      $Filename: planing/prepare.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.prepare for "Der Clou!"
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

#include "planing/prepare.h"


LIST *PersonsList = NULL;
LIST *BurglarsList = NULL;

ubyte PersonsNr = 0;
ubyte BurglarsNr = 0;
U32 CurrentPerson = 0;

U32 Planing_Weight[PLANING_NR_PERSONS];
U32 Planing_Volume[PLANING_NR_PERSONS];

ubyte Planing_Loot[PLANING_NR_LOOTS];
ubyte Planing_Guard[PLANING_NR_GUARDS];

char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

LIST *Planing_GuardRoomList[PLANING_NR_GUARDS];

U32 Planing_BldId;


/* Handler functions */
void plBuildHandler(NODE * n)
{
    U32 flags = SHF_NORMAL;

    if (OL_TYPE(n) == Object_Police)
	flags |= SHF_AUTOREVERS;

    InitHandler(plSys, OL_NR(n), flags);
}

void plClearHandler(NODE * n)
{
    ClearHandler(plSys, OL_NR(n));
}

void plCloseHandler(NODE * n)
{
    CloseHandler(plSys, OL_NR(n));
}

/* Preparation & Unpreparation functions */
void plPrepareData(void)
{
    register ubyte i;

    for (i = 0; i < PLANING_NR_LOOTS; i++)
	Planing_Loot[i] = 0;

    for (i = 0; i < PLANING_NR_PERSONS; i++)
	Planing_Weight[i] = 0;

    for (i = 0; i < PLANING_NR_PERSONS; i++)
	Planing_Volume[i] = 0;

    for (i = 0; i < PLANING_NR_GUARDS; i++)
	Planing_Guard[i] = 0;
}

void plPrepareSprite(U32 livNr, U32 areaId)
{
    LSArea activArea = (LSArea) dbGetObject(areaId);
    uword xpos, ypos;

    livLivesInArea(Planing_Name[livNr], areaId);

    if (livNr < BurglarsNr) {
	switch (livNr) {
	case 0:
	    xpos = activArea->us_StartX0;
	    ypos = activArea->us_StartY0;
	    break;

	case 1:
	    xpos = activArea->us_StartX1;
	    ypos = activArea->us_StartY1;
	    break;

	case 2:
	    xpos = activArea->us_StartX2;
	    ypos = activArea->us_StartY2;
	    break;

	case 3:
	    xpos = activArea->us_StartX3;
	    ypos = activArea->us_StartY3;
	    break;

	default:
	    return;
	}
    } else {
	switch (livNr + (PLANING_NR_PERSONS - BurglarsNr)) {
	case 4:
	case 6:
	    xpos = activArea->us_StartX4 - 4;
	    ypos = activArea->us_StartY4 + 11;
	    break;

	case 5:
	case 7:
	    xpos = activArea->us_StartX5 - 4;
	    ypos = activArea->us_StartY5 + 11;
	    break;

	default:
	    return;
	}
    }

    livSetPos(Planing_Name[livNr], xpos + 9, ypos - 18);

    if (livNr < BurglarsNr)
	livAnimate(Planing_Name[livNr], ANM_STAND, 0, 0);
    else {
	livAnimate(Planing_Name[livNr], ANM_MOVE_DOWN, 0, 0);

	Planing_GuardRoomList[livNr - BurglarsNr] = lsGetRoomsOfArea(areaId);
    }
}

void plPrepareGfx(U32 objId, ubyte landscapMode, ubyte prepareMode)
{
    if (prepareMode & PLANING_GFX_LANDSCAPE)
	lsInitLandScape(objId, landscapMode);

    if (prepareMode & PLANING_GFX_BACKGROUND) {
	CurrentBackground = BGD_PLANUNG;
	ShowMenuBackground();

	/* to blend the menu colours */
	gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_BLEND_UP, 5, -1, -1);
    }

    if (prepareMode & PLANING_GFX_SPRITES) {
	U32 i;

	for (i = 0; i < PersonsNr; i++) {
	    if (dbIsObject(OL_NR(GetNthNode(PersonsList, i)), Object_Person))
		plPrepareSprite(i, lsGetActivAreaID());
	    else {
		((Police) dbGetObject(OL_NR(GetNthNode(PersonsList, i))))->
		    LivingID = i;
		plPrepareSprite(i,
				isGuardedbyGet(objId,
					       OL_NR(GetNthNode
						     (PersonsList, i))));
	    }
	}

	lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1, (uword) - 1);
    }
}

void plUnprepareGfx(void)
{
    register ubyte i;

    gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_FADE_OUT, 5, -1, -1);

    gfxClearArea(u_gc);

    for (i = BurglarsNr; i < PersonsNr; i++)
	RemoveList(Planing_GuardRoomList[i - BurglarsNr]);

    lsDoneLandScape();

    gfxShow(CurrentBackground, GFX_NO_REFRESH | GFX_BLEND_UP, 0, -1, -1);
}

void plPrepareRel(void)
{
    LIST *areas;
    NODE *n;
    LSArea area;

    consistsOfAll(Planing_BldId, OLF_PRIVATE_LIST, Object_LSArea);
    areas = ObjectListPrivate;

    for (n = (NODE *) LIST_HEAD(areas); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n)) {
	area = OL_DATA(n);

	if (!CloneRelation
	    (area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET, hasLoot_Clone_RelId))
	    ErrorMsg(No_Mem, ERROR_MODULE_PLANING, 0);
    }

    RemoveList(areas);

    if (!AddRelation(take_RelId))
	ErrorMsg(No_Mem, ERROR_MODULE_PLANING, 0);
}

void plUnprepareRel(void)
{
    RemRelation(take_RelId);
    RemRelation(hasLoot_Clone_RelId);
}

void plPrepareNames(void)
{
    register ubyte i;

    for (i = 0; i < PersonsNr; i++) {
	if (dbIsObject(OL_NR(GetNthNode(PersonsList, i)), Object_Person))
	    sprintf(Planing_Name[i], "Person_%d", i + 1);
	else
	    sprintf(Planing_Name[i], "Police_%d",
		    i + 1 + (PLANING_NR_PERSONS - BurglarsNr));
    }
}

void plPrepareSys(U32 currPer, U32 objId, ubyte sysMode)
{
    CurrentPerson = currPer;

    if (objId)
	Planing_BldId = objId;

    if (sysMode & PLANING_INIT_PERSONSLIST) {
	if (PersonsList)
	    RemoveList(PersonsList);

	if (BurglarsList)
	    RemoveList(BurglarsList);

	joined_byAll(Person_Matt_Stuvysunt,
		     OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
		     Object_Person);
	PersonsList = ObjectListPrivate;

	joined_byAll(Person_Matt_Stuvysunt,
		     OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
		     Object_Person);
	BurglarsList = ObjectListPrivate;

	dbSortObjectList(&PersonsList, dbStdCompareObjects);
	dbSortObjectList(&BurglarsList, dbStdCompareObjects);

	PersonsNr = GetNrOfNodes(PersonsList);
	BurglarsNr = GetNrOfNodes(BurglarsList);

	plPrepareNames();
    }

    if (sysMode & PLANING_HANDLER_ADD) {
	if (grdAddToList(objId, PersonsList)) {
	    dbSortObjectList(&PersonsList, dbStdCompareObjects);

	    PersonsNr = GetNrOfNodes(PersonsList);

	    plPrepareNames();
	}
    }

    if (sysMode & PLANING_HANDLER_CLEAR)
	foreach(BurglarsList, (void (*)(void *)) plClearHandler);

    if (sysMode & PLANING_HANDLER_CLOSE)
	foreach(PersonsList, (void (*)(void *)) plCloseHandler);

    if (sysMode & PLANING_HANDLER_OPEN)
	foreach(PersonsList, (void (*)(void *)) plBuildHandler);

    if ((sysMode & PLANING_GUARDS_LOAD) && (PersonsNr > BurglarsNr)
	&& !(GamePlayMode & GP_LEVEL_DESIGN)) {
	FILE *fh = NULL;

	startsWithAll(objId, OLF_NORMAL, Object_LSArea);

	if (grdInit(&fh, "r", Planing_BldId, OL_NR(GetNthNode(ObjectList, 0))))
	    grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr,
		  GUARDS_DO_LOAD);

	grdDone(fh);
    }

    if (sysMode & PLANING_HANDLER_SET)
	SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, CurrentPerson)));
}

void plUnprepareSys(void)
{
    plPrepareSys(0, 0, PLANING_HANDLER_CLOSE);

    if (PersonsList)
	RemoveList(PersonsList);

    if (BurglarsList)
	RemoveList(BurglarsList);

    PersonsList = NULL;
    BurglarsList = NULL;
}

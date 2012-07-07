/*
**	$Filename: landscap/init.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	landscap init functions for "Der Clou!"
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

#include "landscap/landscap.h"
#include "landscap/landscap.ph"


static void lsInitFloorSquares(void);
static void lsLoadAllSpots(void);
static void lsSetCurrFloorSquares(U32 areaId);

/*------------------------------------------------------------------------------
 *   global functions for landscape
 *------------------------------------------------------------------------------*/

void lsInitLandScape(U32 bID, ubyte mode)
{				/* initialisiert das Landschaftsmodul */
    S32 i;

    if (!ls)
	ls = TCAllocMem(sizeof(*ls), 0);

    ls->ul_BuildingID = bID;

    ls->us_RasInfoScrollX = 0;
    ls->us_RasInfoScrollY = 0;

    lsInitGfx();		/* dont change location of this line */

    ls->uch_ScrollSpeed = LS_STD_SCROLL_SPEED;
    ls->uch_ShowObjectMask = 0;

    lsSetCollMode(mode);

    lsInitObjects();

    lsInitSpots();

    lsLoadAllSpots();

    ls->uch_FloorsPerWindowColumn = (ubyte) (LS_FLOORS_PER_COLUMN);
    ls->uch_FloorsPerWindowLine = (ubyte) (LS_FLOORS_PER_LINE);

    ls->ul_AreaID = lsGetStartArea();	/* !! MOD 04-01 - vor Sprites!!! */

    BobInitLists();

    ls->us_EscapeCarBobId = BobInit(14, 14);

    /* lootbags must be initialized prior to Livings because they
       have a lower priority (appear below maxis) */
    for (i = 9701; i <= 9708; i++) {
	LSObject lso = dbGetObject(i);

	/* OffsetFact on the PC is not used as offset in the plane,
           but as handle for the bob */
	lso->us_OffsetFact = BobInit(14, 14);

	hasLootBagUnSet(ls->ul_AreaID, (U32) i);
    }

    livInit(0, 0, LS_VISIBLE_X_SIZE, LS_VISIBLE_Y_SIZE, LS_MAX_AREA_WIDTH,
	    LS_MAX_AREA_HEIGHT, 8, ls->ul_AreaID);

    ls->p_CurrFloor = NULL;

    ls->us_DoorXOffset = 0;
    ls->us_DoorYOffset = 32;	/* above are 16er objects */

    ls->p_DoorRefreshList = CreateList();

    lsInitFloorSquares();

    lsInitActivArea(ls->ul_AreaID, (uword) - 1, (uword) - 1, NULL);
    lsShowEscapeCar();
}

void lsInitActivArea(U32 areaID, uword x, uword y, char *livingName)
{
    LSArea area = (LSArea) dbGetObject(areaID);

    ls->ul_AreaID = areaID;

    lsSetRelations(areaID);
    lsSetObjectRetrievalList(areaID);

    ls->us_WindowXSize = area->us_Width;
    ls->us_WindowYSize = area->us_Height;

    if (x == (uword) - 1)
	x = area->us_StartX0;
    if (y == (uword) - 1)
	y = area->us_StartY0;

    lsSetVisibleWindow(x, y);

    livSetActivAreaId(areaID);

    lsSetCurrFloorSquares(areaID);

    gfxCollToMem(area->us_Coll16ID, &LS_COLL16_MEM_RP);
    gfxCollToMem(area->us_Coll32ID, &LS_COLL32_MEM_RP);
    gfxCollToMem(area->us_Coll48ID, &LS_COLL48_MEM_RP);

    gfxCollToMem(area->us_FloorCollID, &LS_FLOOR_MEM_RP);

    lsBuildScrollWindow();
    lsRefreshAllLootBags();
}

void lsInitRelations(U32 areaID)
{
    LSArea area = dbGetObject(areaID);

    AddRelation(area->ul_ObjectBaseNr + REL_CONSIST_OFFSET);
    AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET);
    AddRelation(area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET);
    AddRelation(area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET);
    AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET);
    AddRelation(area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET);
}

void lsSetRelations(U32 areaID)
{
    LSArea area = dbGetObject(areaID);

    ConsistOfRelationID = area->ul_ObjectBaseNr + REL_CONSIST_OFFSET;
    hasLockRelationID = area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET;
    hasAlarmRelationID = area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET;
    hasPowerRelationID = area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET;
    hasLootRelationID = area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET;
    hasRoomRelationID = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;
}

void lsInitObjects(void)
{
    U32 areaCount = 0, i;
    LIST *areas;
    NODE *n;

    /* alle Relationen erzeugen */
    consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
    areas = ObjectListPrivate;

    /* jetzt alle Stockwerke durchgehen! */
    for (i = 0; i < 3; i++) {
	ls->ul_ObjectRetrievalAreaId[i] = 0;
	ls->p_ObjectRetrievalLists[i] = NULL;
    }

    for (n = (NODE *) LIST_HEAD(areas); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n)) {
	lsInitRelations(OL_NR(n));

	/* Daten laden */
	lsInitObjectDB(ls->ul_BuildingID, OL_NR(n));

	lsSetRelations(OL_NR(n));
	lsRefreshObjectList(OL_NR(n));	/* ObjectRetrievalList erstellen */

	/* there's a lot to be patched! */
	lsPatchObjects();

	ls->p_ObjectRetrievalLists[areaCount] = ls->p_ObjectRetrieval;	/* und merken */
	ls->ul_ObjectRetrievalAreaId[areaCount] = OL_NR(n);

	areaCount++;
    }

    lsLoadGlobalData(ls->ul_BuildingID, OL_NR(NODE_PRED(n)));

    RemoveList(areas);
}

void lsLoadGlobalData(U32 bld, U32 ul_AreaId)
{
    char areaName[TXT_KEY_LENGTH], fileName[DSK_PATH_MAX];

    dbGetObjectName(ul_AreaId, areaName);
    areaName[strlen(areaName) - 1] = '\0';

    strcat(areaName, OBJ_GLOBAL_REL_EXTENSION);

    dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName, fileName);

    if (!(LoadRelations(fileName, 0)))
	ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 2);
}

void lsInitObjectDB(U32 bld, U32 areaID)
{
    char fileName[DSK_PATH_MAX], areaName[TXT_KEY_LENGTH];

    dbGetObjectName(areaID, areaName);
    strcat(areaName, OBJ_DATA_EXTENSION);
    dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName, fileName);

    dbSetLoadObjectsMode(DB_LOAD_MODE_NO_NAME);	/* dont fetch names of objects */

    if (dbLoadAllObjects(fileName, 0)) {
	dbGetObjectName(areaID, areaName);
	strcat(areaName, OBJ_REL_EXTENSION);
	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName, fileName);

	if (!(LoadRelations(fileName, 0)))
	    ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 3);
    } else
	ErrorMsg(Disk_Defect, ERROR_MODULE_LANDSCAP, 4);

    dbSetLoadObjectsMode(DB_LOAD_MODE_STD);
}

/* contrary to the Amiga version this loads all floor data into memory at once */
static void lsInitFloorSquares(void)
{
    U32 count;
    unsigned i;
    char fileName[DSK_PATH_MAX], areaName[TXT_KEY_LENGTH];
    NODE *n;
    LIST *areas;

    for (i = 0; i < 3; i++)
	ls->p_AllFloors[i] = NULL;

    count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;

    consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
    areas = ObjectListPrivate;

    /* jetzt alle Stockwerke durchgehen! */
    for (n = LIST_HEAD(areas), i = 0; NODE_SUCC(n); n = NODE_SUCC(n), i++) {
	size_t size = sizeof(struct LSFloorSquare) * count;
	unsigned j;
	FILE *fh;

	ls->p_AllFloors[i] = TCAllocMem(size, 0);
	ls->ul_FloorAreaId[i] = OL_NR(n);

	dbGetObjectName(ls->ul_FloorAreaId[i], areaName);
	strcat(areaName, FLOOR_DATA_EXTENSION);

	dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName, fileName);

	if ((fh = dskOpen(fileName, "rb"))) {
	    for (j = 0; j < count; j++)
		dskRead(fh, &ls->p_AllFloors[i][j].uch_FloorType, sizeof(U8));

	    dskClose(fh);
	}
    }

    RemoveList(areas);
}

static void lsLoadAllSpots(void)
{
    char fileName[TXT_KEY_LENGTH];
    LIST *areas;
    NODE *n;

    consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME,
		  Object_LSArea);
    areas = ObjectListPrivate;

    n = (NODE *) LIST_HEAD(areas);

    strcpy(fileName, NODE_NAME(n));

    fileName[strlen(fileName) - 1] = '\0';

    strcat(fileName, SPOT_DATA_EXTENSION);

    lsLoadSpots(ls->ul_BuildingID, fileName);

    RemoveList(areas);
}

static void lsSetCurrFloorSquares(U32 areaId)
{
    S32 i;

    for (i = 0; i < 3; i++)
	if (areaId == ls->ul_FloorAreaId[i])
	    ls->p_CurrFloor = ls->p_AllFloors[i];
}



/*------------------------------------------------------------------------------
 *   done functions for landscape
 *------------------------------------------------------------------------------*/

/* release all floor data */
static void lsDoneFloorSquares(void)
{
    size_t count, i, size;

    for (i = 0; i < 3; i++) {
	if (ls->p_AllFloors[i]) {
	    count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;

	    size = sizeof(struct LSFloorSquare) * count;

	    TCFreeMem(ls->p_AllFloors[i], size);

	    ls->p_AllFloors[i] = NULL;
	}
    }

}

void lsDoneObjectDB(U32 areaID)
{
    LSArea area = dbGetObject(areaID);

    RemRelations(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);
    dbDeleteAllObjects(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);

    /* globale Relation wieder l”schen */
    RemRelations(Relation_hasClock - 1, 3);

    /* und die "Relationsheader" wieder anlegen */
    AddRelation(Relation_hasClock);
    AddRelation(Relation_ClockTimer);
    AddRelation(Relation_StairConnects);
}

void lsDoneLandScape(void)
{
    NODE *n;
    S32 areaCount = 0, i;

    if (ls) {
	LIST *areas;

	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
	areas = ObjectListPrivate;

	for (n = (NODE *) LIST_HEAD(areas); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n), areaCount++) {
	    lsDoneObjectDB(OL_NR(n));

	    if (ls->p_ObjectRetrievalLists[areaCount]) {
		RemoveList(ls->p_ObjectRetrievalLists[areaCount]);
		ls->p_ObjectRetrievalLists[areaCount] = NULL;
		ls->ul_ObjectRetrievalAreaId[areaCount] = 0;
	    }
	}

	RemoveList(areas);

	lsDoneActivArea(0L);

	livDone();

	for (i = 9701; i <= 9708; i++) {
	    LSObject lso = dbGetObject(i);

	    BobDone(lso->us_OffsetFact);
	}

	BobDone(ls->us_EscapeCarBobId);

	lsDoneSpots();

	lsCloseGfx();

	lsDoneFloorSquares();

	if (ls->p_DoorRefreshList)
	    RemoveList(ls->p_DoorRefreshList);

	TCFreeMem(ls, sizeof(*ls));
	ls = NULL;
    }
}

void lsDoneActivArea(U32 newAreaID)
{
    livSetAllInvisible();	/* MOD 14-01-94 */
}

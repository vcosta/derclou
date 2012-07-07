/*
**      $Filename: planing/planer.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.planer for "Der Clou!"
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

#include "planing/planer.h"


/* Menu ids - planning */
#define PLANING_START      0
#define PLANING_NOTE       1
#define PLANING_SAVE       2
#define PLANING_LOAD       3
#define PLANING_CLEAR      4
#define PLANING_LOOK       5
#define PLANING_RETURN     6

#define PLANING_PERSON_WALK   0
#define PLANING_ACTION_USE    1
#define PLANING_ACTION_OPEN   2
#define PLANING_ACTION_CLOSE  3
#define PLANING_ACTION_TAKE   4
#define PLANING_ACTION_DROP   5
#define PLANING_ACTION_WAIT   6
#define PLANING_ACTION_RADIO  7
#define PLANING_PERSON_CHANGE 8
#define PLANING_ACTION_RETURN 9

#define PLANING_WAIT          0
#define PLANING_WAIT_RADIO    1
#define PLANING_WAIT_RETURN   2


/* Menu ids - notebook */
#define PLANING_NOTE_TARGET            0
#define PLANING_NOTE_TEAM              1
#define PLANING_NOTE_CAR               2
#define PLANING_NOTE_TOOLS             3
#define PLANING_NOTE_LOOTS             4

/* Menu ids - look */
#define PLANING_LOOK_PLAN              0
#define PLANING_LOOK_PERSON_CHANGE     1
#define PLANING_LOOK_RETURN            2

/* Menu ids - leveldesigner */
#define PLANING_LD_MOVE						0
#define PLANING_LD_REFRESH					1
#define PLANING_LD_OK						2
#define PLANING_LD_CANCEL					3

/* Std time defines */
#define PLANING_TIME_TAKE              3
#define PLANING_TIME_DROP              3
#define PLANING_TIME_RADIO             5
#define PLANING_TIME_CONTROL           5
#define PLANING_TIME_FIGHT             5
#define PLANING_TIME_THROUGH_WINDOW    6
#define PLANING_TIME_USE_STAIRS        8


ubyte AnimCounter = 0;
ubyte PlanChanged = 0;

static U32 UseObject;


/* action support functions */
static ubyte plRemLastAction(void)
{
    if (!IsHandlerCleared(plSys)) {
	plSync(PLANING_ANIMATE_NO,
	       GetMaxTimer(plSys) - CurrentAction(plSys)->TimeNeeded,
	       CurrentAction(plSys)->TimeNeeded, 0);
	lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1, (uword) - 1);

	RemLastAction(plSys);

	plDisplayTimer(0, 1);
	plDisplayInfo();

	return 1;
    }

    return 0;
}

/* actions */
static void plActionGo(void)
{
    U32 choice = 0L;
    ubyte direction;
    struct Action *action = CurrentAction(plSys);

    plMessage("WALK", PLANING_MSG_REFRESH);

    inpTurnFunctionKey(0);
    inpTurnMouse(0);
    inpSetKeyRepeat((0 << 5) | 0);

    while (1) {
	direction = 0;

	choice = inpWaitFor(INP_MOVEMENT | INP_LBUTTONP);

	if (choice & INP_LBUTTONP)
	    break;

	if (!(choice & INP_MOUSE)) {
	    if (choice & INP_ESC) {
		if (plRemLastAction())
		    action = CurrentAction(plSys);
	    } else {
		if (choice & INP_LEFT)
		    direction += LS_SCROLL_LEFT;

		if (choice & INP_RIGHT)
		    direction += LS_SCROLL_RIGHT;

		if (choice & INP_UP)
		    direction += LS_SCROLL_UP;

		if (choice & INP_DOWN)
		    direction += LS_SCROLL_DOWN;

		if (!lsInitScrollLandScape(direction, LS_SCROLL_PREPARE)) {
		    if (!action || (action->Type != ACTION_GO)) {
			if ((action =
			     InitAction(plSys, ACTION_GO, (U32) direction, 0L,
					0L)))
			    PlanChanged = 1;
			else {
			    plSay("PLANING_END", CurrentPerson);
			    inpSetKeyRepeat((1 << 5) | 10);
			    inpTurnMouse(1);
			    inpTurnFunctionKey(1);
			    return;
			}
		    }

		    if (ActionData(action, struct ActionGo *)->Direction ==
			(uword) direction)
			 IncCurrentTimer(plSys, 1, 1);
		    else {
			if ((action =
			     InitAction(plSys, ACTION_GO, direction, 0L, 1L)))
			    PlanChanged = 1;
			else {
			    plSay("PLANING_END", CurrentPerson);
			    inpSetKeyRepeat((1 << 5) | 10);
			    inpTurnMouse(1);
			    inpTurnFunctionKey(1);
			    return;
			}
		    }

		    plSync(PLANING_ANIMATE_STD, GetMaxTimer(plSys), 1, 1);
		    plMove(CurrentPerson, direction);

		    lsScrollLandScape((ubyte) - 1);
		    livDoAnims((AnimCounter++) % 2, 1);

		    plDisplayTimer(0, 0);
		}
	    }
	}
    }

    inpSetKeyRepeat((1 << 5) | 10);
    inpTurnMouse(1);
    inpTurnFunctionKey(1);
}

static void plActionWait(void)
{
    LIST *menu = txtGoKey(PLAN_TXT, "MENU_4");
    ubyte activ = 0;
    U32 choice1 = 0L, choice2 = 0L, bitset;

    while (activ != PLANING_WAIT_RETURN) {
	bitset = BIT(PLANING_WAIT) + BIT(PLANING_WAIT_RETURN);

	if (CurrentPerson < BurglarsNr) {
	    if (BurglarsNr > 1)
		bitset += BIT(PLANING_WAIT_RADIO);
	}

	plDisplayTimer(0, 1);
	plDisplayInfo();

	ShowMenuBackground();

	inpTurnESC(0);
	inpTurnFunctionKey(0);
	inpTurnMouse(0);

	activ = Menu(menu, bitset, activ, NULL, 0);

	inpTurnMouse(1);
	inpTurnFunctionKey(1);
	inpTurnESC(1);

	switch (activ) {
	case PLANING_WAIT:
	    {
		choice1 = 0L;
		choice2 = 0L;

		plMessage("WAIT_1", PLANING_MSG_REFRESH);
		plDrawWait(choice2);

		inpTurnESC(0);
		inpTurnFunctionKey(0);
		inpTurnMouse(0);

		while (1) {
		    choice1 =
			inpWaitFor(INP_RIGHT | INP_LEFT | INP_UP | INP_DOWN |
				   INP_LBUTTONP);

		    if (choice1 & INP_LBUTTONP)
			break;

		    if (!(choice1 & INP_MOUSE)) {
			if ((choice1 & INP_RIGHT) && (choice2 < 1800)) {
			    choice2++;
			    plDrawWait(choice2);
			}

			if ((choice1 & INP_LEFT) && choice2) {
			    choice2--;
			    plDrawWait(choice2);
			}

			if ((choice1 & INP_UP) && (choice2 <= 1740)) {
			    choice2 += 60;
			    plDrawWait(choice2);
			}

			if ((choice1 & INP_DOWN) && (choice2 >= 60)) {
			    choice2 -= 60;
			    plDrawWait(choice2);
			}
		    }
		}

		inpTurnMouse(1);
		inpTurnFunctionKey(1);
		inpTurnESC(1);

		if (choice2) {
		    if (InitAction
			(plSys, ACTION_WAIT, 0L, 0L,
			 choice2 * PLANING_CORRECT_TIME)) {
			PlanChanged = 1;

			livAnimate(Planing_Name[CurrentPerson], ANM_STAND, 0,
				   0);
			plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			       choice2 * PLANING_CORRECT_TIME, 1);
			livRefreshAll();
		    } else {
			plSay("PLANING_END", CurrentPerson);
			activ = PLANING_WAIT_RETURN;
		    }
		}
	    }
	    break;

	case PLANING_WAIT_RADIO:
	    if (has(Person_Matt_Stuvysunt, Tool_Funkgeraet)) {
		char exp[TXT_KEY_LENGTH];

		if (BurglarsNr > 2) {
		    NODE *node, *help;

		    plMessage("RADIO_2", PLANING_MSG_REFRESH);
		    SetPictID(((Person)
			       dbGetObject(OL_NR
					   (GetNthNode
					    (BurglarsList,
					     CurrentPerson))))->PictID);
		    node =
			(NODE *) UnLink(BurglarsList,
					OL_NAME(GetNthNode
						(BurglarsList, CurrentPerson)),
					&help);

		    txtGetFirstLine(PLAN_TXT, "EXPAND_RADIO", exp);
		    ExpandObjectList(BurglarsList, exp);

		    choice1 = Bubble(BurglarsList, 0, NULL, 0L);

		    if (ChoiceOk(choice1, GET_OUT, BurglarsList))
			choice1 = OL_NR(GetNthNode(BurglarsList, choice1));
		    else
			choice1 = GET_OUT;

		    Link(BurglarsList, node, help);
		    dbRemObjectNode(BurglarsList, 0L);
		} else {
		    choice1 =
			CurrentPerson ? OL_NR(GetNthNode(BurglarsList, 0)) :
			OL_NR(GetNthNode(BurglarsList, 1));
		    plMessage("RADIO_4",
			      PLANING_MSG_WAIT | PLANING_MSG_REFRESH);
		}

		if (choice1 != GET_OUT) {
		    if (InitAction(plSys, ACTION_WAIT_SIGNAL, choice1, 0L, 1L)) {
			PlanChanged = 1;

			livAnimate(Planing_Name[CurrentPerson], ANM_STAND, 0,
				   0);
			plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			       PLANING_CORRECT_TIME, 1);
			livRefreshAll();
		    } else {
			plSay("PLANING_END", CurrentPerson);
			activ = PLANING_WAIT_RETURN;
		    }
		}
	    } else
		plMessage("NO_RADIO", PLANING_MSG_WAIT);
	    break;
	}
    }

    RemoveList(menu);
}

static void plLevelDesigner(LSObject lso)
{
    LIST *menu = txtGoKey(PLAN_TXT, "MENU_8");
    ubyte ende = 0, activ = 0;
    U32 bitset;
    uword originX = lso->us_DestX, originY = lso->us_DestY;
    U32 area = lsGetActivAreaID();

    bitset = BIT(PLANING_LD_MOVE) +
	BIT(PLANING_LD_REFRESH) + BIT(PLANING_LD_OK) + BIT(PLANING_LD_CANCEL);

    while (!ende) {
	plDisplayTimer(0, 1);
	plDisplayInfo();

	ShowMenuBackground();

	inpTurnESC(0);
	inpTurnFunctionKey(0);
	inpTurnMouse(0);

	activ = Menu(menu, bitset, activ, NULL, 0);

	inpTurnMouse(1);
	inpTurnFunctionKey(1);
	inpTurnESC(1);

	switch (activ) {
	case PLANING_LD_MOVE:
	    while (1) {
		S32 choice =
		    inpWaitFor(INP_RIGHT | INP_LEFT | INP_UP | INP_DOWN |
			       INP_LBUTTONP);

		if (choice & INP_LBUTTONP)
		    break;

		if (choice & INP_RIGHT)
		    lso->us_DestX++;

		if (choice & INP_LEFT)
		    lso->us_DestX--;

		if (choice & INP_DOWN)
		    lso->us_DestY++;

		if (choice & INP_UP)
		    lso->us_DestY--;

		lsShowOneObject(lso, lso->us_DestX, lso->us_DestY, 32);
	    }
	    break;

	case PLANING_LD_REFRESH:
	    lsDoneActivArea(area);
	    lsInitActivArea(area, livGetXPos(Planing_Name[CurrentPerson]),
			    livGetYPos(Planing_Name[CurrentPerson]),
			    Planing_Name[CurrentPerson]);

	    if (lsGetStartArea() == lsGetActivAreaID())
		lsShowEscapeCar();

	    livRefreshAll();
	    break;

	case PLANING_LD_OK:
	    {
		char fileName[DSK_PATH_MAX], areaName[TXT_KEY_LENGTH];

		dbGetObjectName(area, areaName);
		strcat(areaName, ".dat");
		dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, areaName, fileName);

		dbSaveAllObjects(fileName,
				 ((LSArea) dbGetObject(area))->ul_ObjectBaseNr,
				 10000, 0);
	    }

	    ende = 1;
	    break;

	case PLANING_LD_CANCEL:
	    lso->us_DestX = originX;
	    lso->us_DestY = originY;

	    ende = 1;
	    break;
	}
    }

    RemoveList(menu);

    lsDoneActivArea(area);
    lsInitActivArea(area, livGetXPos(Planing_Name[CurrentPerson]),
		    livGetYPos(Planing_Name[CurrentPerson]),
		    Planing_Name[CurrentPerson]);

    if (lsGetStartArea() == lsGetActivAreaID())
	lsShowEscapeCar();

    livRefreshAll();
}

static void plActionOpenClose(uword what)
{
    LIST *actionList = plGetObjectsList(CurrentPerson, 0);
    U32 choice1 = 0L, state;
    char exp[TXT_KEY_LENGTH];

    if (LIST_EMPTY(actionList))
	plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
    else {
	if (what == ACTION_OPEN) {
	    plMessage("OPEN", PLANING_MSG_REFRESH);
	    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
	} else {
	    plMessage("CLOSE", PLANING_MSG_REFRESH);
	    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
	}

	ExpandObjectList(actionList, exp);
	SetPictID(((Person)
		   dbGetObject(OL_NR(GetNthNode(PersonsList, CurrentPerson))))->
		  PictID);

	choice1 = Bubble(actionList, 0, NULL, 0L);

	if (ChoiceOk(choice1, GET_OUT, actionList)) {
	    choice1 = OL_NR(GetNthNode(actionList, choice1));

	    if (GamePlayMode & GP_LEVEL_DESIGN)
		plLevelDesigner(dbGetObject(choice1));
	    else if ((CurrentPerson >= BurglarsNr)
		     || !CHECK_STATE(lsGetObjectState(choice1),
				     Const_tcIN_PROGRESS_BIT)) {
		if ((CurrentPerson >= BurglarsNr) || plIgnoreLock(choice1)
		    || CHECK_STATE(lsGetObjectState(choice1),
				   Const_tcLOCK_UNLOCK_BIT)) {
		    state =
			CHECK_STATE(lsGetObjectState(choice1),
				    Const_tcOPEN_CLOSE_BIT);

		    if ((what == ACTION_OPEN) ? !state : state) {
			if (InitAction
			    (plSys, what, choice1, 0L,
			     opensGet(((LSObject) dbGetObject(choice1))->Type,
				      Tool_Hand) * PLANING_CORRECT_TIME)) {
			    PlanChanged = 1;

			    if (CurrentPerson < BurglarsNr)
				plWork(CurrentPerson);

			    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
				   opensGet(((LSObject) dbGetObject(choice1))->
					    Type,
					    Tool_Hand) * PLANING_CORRECT_TIME,
				   1);

			    lsSetObjectState(choice1, Const_tcOPEN_CLOSE_BIT,
					     ((what == ACTION_OPEN) ? 1 : 0));

			    if (what == ACTION_OPEN)
				plCorrectOpened((LSObject) dbGetObject(choice1),
						1);
			    else
				plCorrectOpened((LSObject) dbGetObject(choice1),
						0);

			    plRefresh(choice1);
			    livRefreshAll();
			} else
			    plSay("PLANING_END", CurrentPerson);
		    } else {
			if (what == ACTION_OPEN)
			    plMessage("OPEN_OPENED",
				      PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
			else
			    plMessage("CLOSE_CLOSED",
				      PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		    }
		} else
		    plMessage("LOCKED", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
	    } else
		plMessage("IN_PROGRESS",
			  PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
	}
    }

    RemoveList(actionList);
}

static void plActionTake(void)
{
    LIST *actionList = plGetObjectsList(CurrentPerson, 1);
    U32 choice = 0L, choice1 = 0L, choice2 = 0L, state;
    char exp[TXT_KEY_LENGTH];

    if (LIST_EMPTY(actionList))
	plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
    else {
	LIST *takeableList = CreateList();
	struct ObjectNode *n, *h, *h2;

	for (n = (struct ObjectNode *) LIST_HEAD(actionList); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n)) {
	    SetObjectListAttr(OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL,
			      0L);
	    AskAll(dbGetObject(OL_NR(n)), hasLoot(CurrentPerson),
		   BuildObjectList);

	    state = lsGetObjectState(OL_NR(n));

	    if (!LIST_EMPTY(ObjectList)) {
		if (CHECK_STATE(state, Const_tcTAKE_BIT)) {
		    h2 = (struct ObjectNode *) LIST_HEAD(ObjectList);

		    h = (struct ObjectNode *) CreateNode(takeableList,
							 sizeof(struct
								ObjectNode),
							 OL_NAME(h2));
		    h->nr = OL_NR(h2);	/* Loot */
		    h->type = OL_NR(n);	/* Original */
		    h->data = NULL;
		} else {
		    if (CHECK_STATE(state, Const_tcOPEN_CLOSE_BIT)) {
			for (h2 = (struct ObjectNode *) LIST_HEAD(ObjectList);
			     NODE_SUCC(h2);
			     h2 = (struct ObjectNode *) NODE_SUCC(h2)) {
			    h = (struct ObjectNode *) CreateNode(takeableList,
								 sizeof(struct
									ObjectNode),
								 OL_NAME(h2));
			    h->nr = OL_NR(h2);	/* Loot */
			    h->type = OL_NR(n);	/* Original */
			    h->data = (void *) 1L;
			}
		    }
		}
	    }
	}

	if (!LIST_EMPTY(takeableList)) {
	    plMessage("TAKE", PLANING_MSG_REFRESH);

	    SetPictID(((Person)
		       dbGetObject(OL_NR
				   (GetNthNode(BurglarsList, CurrentPerson))))->
		      PictID);

	    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
	    ExpandObjectList(takeableList, exp);

	    choice = Bubble(takeableList, 0, NULL, 0L);

	    if (ChoiceOk(choice, GET_OUT, takeableList)) {
		U32 weightPerson =
		    tcWeightPersCanCarry((Person)
					 dbGetObject(OL_NR
						     (GetNthNode
						      (BurglarsList,
						       CurrentPerson))));
		U32 volumePerson =
		    tcVolumePersCanCarry((Person)
					 dbGetObject(OL_NR
						     (GetNthNode
						      (BurglarsList,
						       CurrentPerson))));

		U32 weightLoot;
		U32 volumeLoot;

		choice1 = OL_NR(GetNthNode(takeableList, choice));
		choice2 = OL_TYPE(GetNthNode(takeableList, choice));

		weightLoot = ((Loot) dbGetObject(choice1))->Weight;
		volumeLoot = ((Loot) dbGetObject(choice1))->Volume;

		if ((Planing_Weight[CurrentPerson] + weightLoot) <=
		    weightPerson) {
		    if ((Planing_Volume[CurrentPerson] + volumeLoot) <=
			volumePerson) {
			if (InitAction
			    (plSys, ACTION_TAKE, choice2, choice1,
			     PLANING_TIME_TAKE * PLANING_CORRECT_TIME)) {
			    PlanChanged = 1;

			    plWork(CurrentPerson);
			    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
				   PLANING_TIME_TAKE * PLANING_CORRECT_TIME, 1);

			    if (!OL_DATA(GetNthNode(takeableList, choice))) {
				if ((choice2 >= 9701) && (choice2 <= 9708)) {
				    lsRemLootBag(choice2);
				    Planing_Loot[choice2 - 9701] = 0;
				} else {
				    lsTurnObject((LSObject)
						 dbGetObject(choice2),
						 LS_OBJECT_INVISIBLE,
						 LS_NO_COLLISION);
				    lsSetObjectState(choice2,
						     Const_tcACCESS_BIT, 0);

				    plMessage("TAKEN_LOOT",
					      PLANING_MSG_REFRESH);
				}
			    }

			    {
				U32 newValue =
				    GetP(dbGetObject(choice2),
					 hasLoot(CurrentPerson),
					 dbGetObject(choice1));

				if (Ask
				    (dbGetObject
				     (OL_NR
				      (GetNthNode
				       (BurglarsList, CurrentPerson))),
				     take_RelId, dbGetObject(choice1))) {
				    U32 oldValue =
					GetP(dbGetObject
					     (OL_NR
					      (GetNthNode
					       (BurglarsList, CurrentPerson))),
					     take_RelId, dbGetObject(choice1));

				    SetP(dbGetObject
					 (OL_NR
					  (GetNthNode
					   (BurglarsList, CurrentPerson))),
					 take_RelId, dbGetObject(choice1),
					 oldValue + newValue);
				} else
				    SetP(dbGetObject
					 (OL_NR
					  (GetNthNode
					   (BurglarsList, CurrentPerson))),
					 take_RelId, dbGetObject(choice1),
					 newValue);
			    }

			    Planing_Weight[CurrentPerson] += weightLoot;
			    Planing_Volume[CurrentPerson] += volumeLoot;

			    plRefresh(choice2);
			    livRefreshAll();

			    SetP(dbGetObject(choice1), hasLoot(CurrentPerson),
				 dbGetObject(choice1),
				 GetP(dbGetObject(choice2),
				      hasLoot(CurrentPerson),
				      dbGetObject(choice1)));
			    Present(choice1, "RasterObject",
				    InitOneLootPresent);
			    UnSet(dbGetObject(choice1), hasLoot(CurrentPerson),
				  dbGetObject(choice1));

			    UnSet(dbGetObject(choice2), hasLoot(CurrentPerson),
				  dbGetObject(choice1));
			} else
			    plSay("PLANING_END", CurrentPerson);
		    } else
			plMessage("TOO_BIG",
				  PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		} else
		    plMessage("TOO_HEAVY",
			      PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
	    }
	} else
	    plMessage("NO_OBJECTS", PLANING_MSG_WAIT);

	RemoveList(takeableList);
    }

    RemoveList(actionList);
}

static char *plSetUseString(U32 nr, U32 type, void *data)
{
    static char useString[TXT_KEY_LENGTH];
    U32 actLoudness =
	lsGetLoudness(livGetXPos(Planing_Name[CurrentPerson]),
		      livGetYPos(Planing_Name[CurrentPerson]));
    U32 objLoudness = soundGet(((LSObject) dbGetObject(UseObject))->Type, nr);

    useString[0] = '\0';

    if (break (((LSObject) dbGetObject(UseObject))->Type, nr)) {
	sprintf(useString, "%" PRIu32 " %s",
		tcGuyUsesTool(OL_NR(GetNthNode(PersonsList, CurrentPerson)),
			      (Building) dbGetObject(Planing_BldId), nr,
			      ((LSObject) dbGetObject(UseObject))->Type),
		txtSeconds);

	if (objLoudness >= actLoudness) {
	    strcat(useString, ", ");
	    strcat(useString, txtTooLoud);
	}
    }

    return useString;
}

static ubyte plCheckAbilities(U32 persId, U32 checkToolId)
{
    NODE *n;
    LIST *requires;
    ubyte ret = 1;

    toolRequiresAll(checkToolId, OLF_PRIVATE_LIST, Object_Ability);
    requires = ObjectListPrivate;

    for (n = (NODE *) LIST_HEAD(requires); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	if (!has(persId, OL_NR(n))) {
	    ret = 0;
	    break;
	} else {
	    if (hasGet(persId, OL_NR(n)) <
		toolRequiresGet(checkToolId, OL_NR(n))) {
		ret = 0;
		break;
	    }
	}
    }

    RemoveList(requires);
    return ret;
}

static ubyte plCheckRequiredTools(U32 checkToolId)
{
    LIST *trl;
    NODE *n, *h;
    ubyte ret = 1;

    toolRequiresAll(checkToolId, OLF_PRIVATE_LIST, Object_Tool);
    trl = ObjectListPrivate;

    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

    for (n = (NODE *) LIST_HEAD(trl); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n)) {
	ubyte found = 0;

	ret = 0;

	for (h = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(h);
	     h = (NODE *) NODE_SUCC(h)) {
	    if (OL_NR(n) == OL_NR(h))
		found = 1;
	}

	if (found) {
	    ret = 1;
	    break;
	}
    }

    RemoveList(trl);

    return ret;
}

static void plCorrectToolsList(U32 flags)
{
    /* only insert time-punch card if a guard has been neutralized */
    if (PersonsNr > BurglarsNr) {
	ubyte i;

	for (i = BurglarsNr; i < PersonsNr; i++) {
	    if (Planing_Guard[i - BurglarsNr] == 2) {
		if (!dbHasObjectNode(ObjectList, Tool_Stechkarte))
		    dbAddObjectNode(ObjectList, Tool_Stechkarte, flags);
		break;
	    }
	}
    }

    dbRemObjectNode(ObjectList, Tool_Handschuhe);
    dbRemObjectNode(ObjectList, Tool_Schuhe);
    dbRemObjectNode(ObjectList, Tool_Maske);
    dbRemObjectNode(ObjectList, Tool_Stromgenerator);
    dbRemObjectNode(ObjectList, Tool_Batterie);
    dbRemObjectNode(ObjectList, Tool_Schutzanzug);
}

static void plActionUse(void)
{
    LIST *actionList = plGetObjectsList(CurrentPerson, 0);
    U32 choice1 = 0L, choice2 = 0L, state;
    ubyte i;
    char exp[TXT_KEY_LENGTH];

    if (CurrentPerson < BurglarsNr) {
	for (i = BurglarsNr; i < PersonsNr; i++)
	    plInsertGuard(actionList, CurrentPerson, i);

	hasAll(Person_Matt_Stuvysunt,
	       OLF_NORMAL | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Tool);
	plCorrectToolsList(OLF_INCLUDE_NAME | OLF_INSERT_STAR);

	if (LIST_EMPTY(ObjectList))
	    plMessage("USE_1", PLANING_MSG_WAIT);
	else {
	    if (LIST_EMPTY(actionList))
		plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
	    else {
		plMessage("USE_3", PLANING_MSG_REFRESH);

		SetPictID(((Person)
			   dbGetObject(OL_NR
				       (GetNthNode
					(PersonsList,
					 CurrentPerson))))->PictID);

		txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
		ExpandObjectList(actionList, exp);

		choice1 = Bubble(actionList, 0, NULL, 0L);

		if (ChoiceOk(choice1, GET_OUT, actionList)) {
		    choice1 = OL_NR(GetNthNode(actionList, choice1));

		    if (plIsStair(choice1)) {
			U32 newAreaId = StairConnectsGet(choice1, choice1);

			if (InitAction
			    (plSys, ACTION_USE, choice1, lsGetActivAreaID(),
			     PLANING_TIME_USE_STAIRS * PLANING_CORRECT_TIME)) {
			    PlanChanged = 1;

			    livLivesInArea(Planing_Name[CurrentPerson],
					   newAreaId);

			    lsDoneActivArea(newAreaId);
			    lsInitActivArea(newAreaId,
					    livGetXPos(Planing_Name
						       [CurrentPerson]),
					    livGetYPos(Planing_Name
						       [CurrentPerson]),
					    Planing_Name[CurrentPerson]);

			    if (lsGetStartArea() == lsGetActivAreaID())
				lsShowEscapeCar();	/* Auto neu zeichnen */

			    livRefreshAll();

			    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
				   PLANING_TIME_USE_STAIRS *
				   PLANING_CORRECT_TIME, 1);
			    lsSetActivLiving(Planing_Name[CurrentPerson],
					     (uword) - 1, (uword) - 1);
			} else
			    plSay("PLANING_END", CurrentPerson);
		    } else if (dbIsObject(choice1, Object_Police)) {
			if (has
			    (OL_NR(GetNthNode(PersonsList, CurrentPerson)),
			     Ability_Kampf)) {
			    LIST *objList = CreateList();

			    dbAddObjectNode(objList, Tool_Hand,
					    OLF_INCLUDE_NAME | OLF_INSERT_STAR);
			    dbAddObjectNode(objList, Tool_Fusz,
					    OLF_INCLUDE_NAME | OLF_INSERT_STAR);

			    if (has(Person_Matt_Stuvysunt, Tool_Chloroform))
				dbAddObjectNode(objList, Tool_Chloroform,
						OLF_INCLUDE_NAME |
						OLF_INSERT_STAR);

			    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
			    ExpandObjectList(objList, exp);

			    plMessage("USE_4", PLANING_MSG_REFRESH);

			    SetPictID(((Person)
				       dbGetObject(OL_NR
						   (GetNthNode
						    (PersonsList,
						     CurrentPerson))))->PictID);

			    choice2 = Bubble(objList, 0, NULL, 0L);

			    if (ChoiceOk(choice2, GET_OUT, objList)) {
				choice2 = OL_NR(GetNthNode(objList, choice2));

				if (InitAction
				    (plSys, ACTION_USE, choice1, choice2,
				     tcGuyUsesTool(OL_NR
						   (GetNthNode
						    (PersonsList,
						     CurrentPerson)),
						   (Building)
						   dbGetObject(Planing_BldId),
						   choice2,
						   Item_Wache) *
				     PLANING_CORRECT_TIME)) {
				    PlanChanged = 1;

				    Planing_Guard[((Police)
						   dbGetObject(choice1))->
						  LivingID - BurglarsNr] = 2;

				    plWork(CurrentPerson);
				    plSync(PLANING_ANIMATE_NO,
					   GetMaxTimer(plSys),
					   tcGuyUsesTool(OL_NR
							 (GetNthNode
							  (PersonsList,
							   CurrentPerson)),
							 (Building)
							 dbGetObject
							 (Planing_BldId),
							 choice2,
							 Item_Wache) *
					   PLANING_CORRECT_TIME, 1);
				    livRefreshAll();
				} else
				    plSay("PLANING_END", CurrentPerson);
			    }

			    RemoveList(objList);
			} else
			    plMessage("WRONG_ABILITY",
				      PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		    } else {
			state = lsGetObjectState(choice1);

			if (!CHECK_STATE(state, Const_tcIN_PROGRESS_BIT)) {
			    if (plIgnoreLock(choice1)
				&& !CHECK_STATE(state,
						Const_tcOPEN_CLOSE_BIT)) {
				switch (((LSObject) dbGetObject(choice1))->Type) {
				case Item_Alarmanlage_Z3:
				case Item_Alarmanlage_X3:
				case Item_Alarmanlage_Top:
				    plMessage("ALARM_OPEN",
					      PLANING_MSG_REFRESH |
					      PLANING_MSG_WAIT);
				    break;

				case Item_Steuerkasten:
				    plMessage("POWER_OPEN",
					      PLANING_MSG_REFRESH |
					      PLANING_MSG_WAIT);
				    break;
				}
			    } else {
				if (!CHECK_STATE
				    (state, Const_tcLOCK_UNLOCK_BIT)) {
				    plMessage("USE_2", PLANING_MSG_REFRESH);

				    UseObject = choice1;
				    ObjectListSuccString = plSetUseString;
				    ObjectListWidth = 48L;

				    hasAll(Person_Matt_Stuvysunt,
					   OLF_NORMAL | OLF_INCLUDE_NAME |
					   OLF_INSERT_STAR | OLF_ADD_SUCC_STRING
					   | OLF_ALIGNED, Object_Tool);
				    plCorrectToolsList(OLF_INCLUDE_NAME |
						       OLF_INSERT_STAR |
						       OLF_ADD_SUCC_STRING |
						       OLF_ALIGNED);

				    ObjectListWidth = 0L;
				    ObjectListSuccString = NULL;
				    UseObject = 0L;

				    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL",
						    exp);
				    ExpandObjectList(ObjectList, exp);

				    SetPictID(((Person)
					       dbGetObject(OL_NR
							   (GetNthNode
							    (PersonsList,
							     CurrentPerson))))->
					      PictID);

				    choice2 = Bubble(ObjectList, 0, NULL, 0L);

				    if (ChoiceOk(choice2, GET_OUT, ObjectList)) {
					choice2 =
					    OL_NR(GetNthNode
						  (ObjectList, choice2));

					if (plCheckAbilities
					    (OL_NR
					     (GetNthNode
					      (PersonsList, CurrentPerson)),
					     choice2)) {
					    if (plCheckRequiredTools(choice2)) {
						if (break
						    (((LSObject)
						      dbGetObject(choice1))->
						     Type, choice2)) {
						    if (InitAction
							(plSys, ACTION_USE,
							 choice1, choice2,
							 tcGuyUsesTool(OL_NR
								       (GetNthNode
									(PersonsList,
									 CurrentPerson)),
								       (Building)
								       dbGetObject
								       (Planing_BldId),
								       choice2,
								       ((LSObject) dbGetObject(choice1))->Type) * PLANING_CORRECT_TIME)) {
							PlanChanged = 1;

							plWork(CurrentPerson);
							plSync
							    (PLANING_ANIMATE_NO,
							     GetMaxTimer(plSys),
							     tcGuyUsesTool(OL_NR
									   (GetNthNode
									    (PersonsList,
									     CurrentPerson)),
									   (Building)
									   dbGetObject
									   (Planing_BldId),
									   choice2,
									   ((LSObject) dbGetObject(choice1))->Type) * PLANING_CORRECT_TIME, 1);

							if (!plIgnoreLock
							    (choice1)) {
							    lsSetObjectState
								(choice1,
								 Const_tcLOCK_UNLOCK_BIT,
								 1);

							    if (((Tool)
								 dbGetObject
								 (choice2))->
								Effect &
								Const_tcTOOL_OPENS)
							    {
								lsSetObjectState
								    (choice1,
								     Const_tcOPEN_CLOSE_BIT,
								     1);
								plCorrectOpened((LSObject) dbGetObject(choice1), 1);
							    }
							} else {
							    state =
								lsGetObjectState
								(choice1);

							    if (CHECK_STATE
								(state,
								 Const_tcON_OFF))
							    {
								lsSetObjectState(choice1, Const_tcON_OFF, 0);	/* on setzen  */

								if (plIgnoreLock
								    (choice1) ==
								    PLANING_POWER)
								{
								    lsSetSpotStatus
									(choice1,
									 LS_SPOT_ON);
								    lsShowAllSpots
									(CurrentTimer
									 (plSys),
									 LS_ALL_VISIBLE_SPOTS);
								    plMessage
									("POWER_ON",
									 PLANING_MSG_REFRESH
									 |
									 PLANING_MSG_WAIT);
								} else
								    plMessage
									("ALARM_ON",
									 PLANING_MSG_REFRESH
									 |
									 PLANING_MSG_WAIT);
							    } else {
								lsSetObjectState(choice1, Const_tcON_OFF, 1);	/* off setzen */

								if (plIgnoreLock
								    (choice1) ==
								    PLANING_POWER)
								{
								    lsSetSpotStatus
									(choice1,
									 LS_SPOT_OFF);
								    lsShowAllSpots
									(CurrentTimer
									 (plSys),
									 LS_ALL_INVISIBLE_SPOTS);
								    plMessage
									("POWER_OFF",
									 PLANING_MSG_REFRESH
									 |
									 PLANING_MSG_WAIT);
								} else
								    plMessage
									("ALARM_OFF",
									 PLANING_MSG_REFRESH
									 |
									 PLANING_MSG_WAIT);
							    }
							}

							plRefresh(choice1);
							livRefreshAll();
						    } else
							plSay("PLANING_END",
							      CurrentPerson);
						} else
						    plMessage("DOES_NOT_WORK",
							      PLANING_MSG_REFRESH
							      |
							      PLANING_MSG_WAIT);
					    } else
						plMessage("TOOL_DOES_NOT_WORK",
							  PLANING_MSG_REFRESH |
							  PLANING_MSG_WAIT);
					} else
					    plMessage("WRONG_ABILITY",
						      PLANING_MSG_REFRESH |
						      PLANING_MSG_WAIT);
				    }
				} else
				    if ((((LSObject) dbGetObject(choice1))->
					 Type == Item_Fenster)) {

                                    if (setup.Profidisk && Planing_BldId == Building_Postzug)
                                        plSay("PLANING_TRAIN", CurrentPerson);
				    else
				    if (CHECK_STATE
					    (lsGetObjectState(choice1),
						 Const_tcOPEN_CLOSE_BIT)) {
					if (has
					    (Person_Matt_Stuvysunt,
					     Tool_Strickleiter)) {
					    uword xpos, ypos;

					    if (InitAction
						(plSys, ACTION_USE, choice1, 0L,
						 PLANING_TIME_THROUGH_WINDOW *
						 PLANING_CORRECT_TIME)) {
						PlanChanged = 1;

						lsWalkThroughWindow((LSObject)
								    dbGetObject
								    (choice1),
								    livGetXPos
								    (Planing_Name
								     [CurrentPerson]),
								    livGetYPos
								    (Planing_Name
								     [CurrentPerson]),
								    &xpos,
								    &ypos);

						livSetPos(Planing_Name
							  [CurrentPerson], xpos,
							  ypos);
						plSync(PLANING_ANIMATE_NO,
						       GetMaxTimer(plSys),
						       PLANING_TIME_THROUGH_WINDOW
						       * PLANING_CORRECT_TIME,
						       1);
						livRefreshAll();
					    } else
						plSay("PLANING_END",
						      CurrentPerson);
					} else
					    plMessage("NO_STAIRS",
						      PLANING_MSG_REFRESH |
						      PLANING_MSG_WAIT);
				    } else
					plMessage("WALK_WINDOW",
						  PLANING_MSG_REFRESH |
						  PLANING_MSG_WAIT);
				} else
				    plMessage("UNLOCK_UNLOCKED",
					      PLANING_MSG_REFRESH |
					      PLANING_MSG_WAIT);
			    }
			} else
			    plMessage("IN_PROGRESS",
				      PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		    }
		}
	    }
	}
    } else {
	if (LIST_EMPTY(actionList))
	    plMessage("NO_OBJECTS", PLANING_MSG_WAIT);
	else {
	    plMessage("CONTROL", PLANING_MSG_REFRESH);

	    SetPictID(((Person)
		       dbGetObject(OL_NR
				   (GetNthNode(PersonsList, CurrentPerson))))->
		      PictID);

	    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
	    ExpandObjectList(actionList, exp);

	    choice1 = Bubble(actionList, 0, NULL, 0L);

	    if (ChoiceOk(choice1, GET_OUT, actionList)) {
		choice1 = OL_NR(GetNthNode(actionList, choice1));

		if (InitAction
		    (plSys, ACTION_CONTROL, choice1, 0L,
		     PLANING_TIME_CONTROL * PLANING_CORRECT_TIME)) {
		    PlanChanged = 1;

		    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			   PLANING_TIME_CONTROL * PLANING_CORRECT_TIME, 1);

		    plRefresh(choice1);
		    livRefreshAll();
		} else
		    plSay("PLANING_END", CurrentPerson);
	    }
	}
    }

    RemoveList(actionList);
}

static void plAction(void)
{
    LIST *menu = NULL;
    ubyte activ = 0;
    U32 choice1 = 0L, choice2 = 0L, bitset;
    char exp[TXT_KEY_LENGTH];

    if (CurrentPerson < BurglarsNr)
	menu = txtGoKey(PLAN_TXT, "MENU_2");
    else
	menu = txtGoKey(PLAN_TXT, "MENU_5");

    while (activ != PLANING_ACTION_RETURN) {
	if (CurrentPerson < BurglarsNr) {
	    bitset = BIT(PLANING_PERSON_WALK) +
		BIT(PLANING_ACTION_USE) +
		BIT(PLANING_ACTION_OPEN) +
		BIT(PLANING_ACTION_CLOSE) +
		BIT(PLANING_ACTION_TAKE) +
		BIT(PLANING_ACTION_DROP) +
		BIT(PLANING_ACTION_WAIT) + BIT(PLANING_ACTION_RETURN);


	    if ((GamePlayMode & GP_GUARD_DESIGN) ? (PersonsNr > 1)
		: (BurglarsNr > 1))
		bitset += BIT(PLANING_ACTION_RADIO);

	    if (!(Planing_BldId == Building_Starford_Kaserne)
		|| (GamePlayMode & GP_GUARD_DESIGN))
		bitset += BIT(PLANING_PERSON_CHANGE);
	} else {
	    bitset = BIT(PLANING_PERSON_WALK) +
		BIT(PLANING_ACTION_USE) +
		BIT(PLANING_ACTION_OPEN) +
		BIT(PLANING_ACTION_CLOSE) +
		BIT(PLANING_ACTION_WAIT) + BIT(PLANING_ACTION_RETURN);

	    if (PersonsNr > 1)
		bitset += BIT(PLANING_PERSON_CHANGE);
	}

	plDisplayTimer(0, 1);
	plDisplayInfo();

	ShowMenuBackground();

	inpTurnESC(0);
	inpTurnFunctionKey(0);
	inpTurnMouse(0);

	activ = Menu(menu, bitset, activ, NULL, 0);

	inpTurnMouse(1);
	inpTurnFunctionKey(1);
	inpTurnESC(1);

	switch (activ) {
	case PLANING_PERSON_WALK:
	    plActionGo();
	    break;

	case PLANING_PERSON_CHANGE:
	    plMessage("CHANGE_PERSON_1", PLANING_MSG_REFRESH);

	    if (GamePlayMode & GP_GUARD_DESIGN) {
		if (PersonsNr > 2)
		    choice1 =
			(U32) Bubble(PersonsList, CurrentPerson, NULL, 0L);
		else
		    choice1 = ((CurrentPerson) ? 0L : 1L);
	    } else {
		if (BurglarsNr > 2)
		    choice1 =
			(U32) Bubble(BurglarsList, CurrentPerson, NULL, 0L);
		else
		    choice1 = ((CurrentPerson) ? 0L : 1L);
	    }

	    if (choice1 != GET_OUT) {
		U32 oldTime = CurrentTimer(plSys);

		plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);

		plPrepareSys(choice1, 0, PLANING_HANDLER_SET);

		if (livWhereIs(Planing_Name[choice1]) != lsGetActivAreaID()) {
		    lsDoneActivArea(livWhereIs(Planing_Name[choice1]));
		    lsInitActivArea(livWhereIs(Planing_Name[choice1]),
				    livGetXPos(Planing_Name[choice1]),
				    livGetYPos(Planing_Name[choice1]),
				    Planing_Name[choice1]);
		}

		if (oldTime < GetMaxTimer(plSys))
		    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			   GetMaxTimer(plSys) - oldTime, 1);

		if (oldTime > GetMaxTimer(plSys))
		    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			   oldTime - GetMaxTimer(plSys), 0);

		lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1,
				 (uword) - 1);

		RemoveList(menu);

		if (CurrentPerson < BurglarsNr)
		    menu = txtGoKey(PLAN_TXT, "MENU_2");
		else
		    menu = txtGoKey(PLAN_TXT, "MENU_5");
	    }
	    break;

	case PLANING_ACTION_USE:
	    plActionUse();
	    break;

	case PLANING_ACTION_OPEN:
	    plActionOpenClose(ACTION_OPEN);
	    break;

	case PLANING_ACTION_CLOSE:
	    plActionOpenClose(ACTION_CLOSE);
	    break;

	case PLANING_ACTION_TAKE:
	    plActionTake();
	    break;

	case PLANING_ACTION_DROP:
	    SetObjectListAttr(OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL,
			      0L);
	    AskAll(dbGetObject(OL_NR(GetNthNode(BurglarsList, CurrentPerson))),
		   take_RelId, BuildObjectList);

	    if (LIST_EMPTY(ObjectList))
		plMessage("DROP_1", PLANING_MSG_WAIT);
	    else {
		plMessage("DROP_2", PLANING_MSG_REFRESH);

		SetPictID(((Person)
			   dbGetObject(OL_NR
				       (GetNthNode
					(BurglarsList,
					 CurrentPerson))))->PictID);

		txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
		ExpandObjectList(ObjectList, exp);

		choice1 = Bubble(ObjectList, 0, NULL, 0L);

		if (ChoiceOk(choice1, GET_OUT, ObjectList)) {
		    U32 weightLoot;
		    U32 volumeLoot;

		    choice1 = OL_NR(GetNthNode(ObjectList, choice1));

		    weightLoot = ((Loot) dbGetObject(choice1))->Weight;
		    volumeLoot = ((Loot) dbGetObject(choice1))->Volume;

		    if ((choice2 = plGetNextLoot())) {
			if (InitAction
			    (plSys, ACTION_DROP, choice2, choice1,
			     PLANING_TIME_DROP * PLANING_CORRECT_TIME)) {
			    PlanChanged = 1;

			    plWork(CurrentPerson);
			    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
				   PLANING_TIME_DROP * PLANING_CORRECT_TIME, 1);

			    SetP(dbGetObject(choice2), hasLoot(CurrentPerson),
				 dbGetObject(choice1),
				 GetP(dbGetObject
				      (OL_NR
				       (GetNthNode
					(BurglarsList, CurrentPerson))),
				      take_RelId, dbGetObject(choice1)));
			    UnSet(dbGetObject
				  (OL_NR
				   (GetNthNode(BurglarsList, CurrentPerson))),
				  take_RelId, dbGetObject(choice1));
			    Planing_Weight[CurrentPerson] -= weightLoot;
			    Planing_Volume[CurrentPerson] -= volumeLoot;

			    plRefresh(choice2);
			    livRefreshAll();
			} else
			    plSay("PLANING_END", CurrentPerson);
		    } else
			plMessage("DROP_3",
				  PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
		}
	    }
	    break;

	case PLANING_ACTION_WAIT:
	    plActionWait();
	    break;

	case PLANING_ACTION_RADIO:
	    if (has(Person_Matt_Stuvysunt, Tool_Funkgeraet)) {
		if (BurglarsNr > 2) {
		    NODE *node, *help;

		    plMessage("RADIO_1", PLANING_MSG_REFRESH);
		    SetPictID(((Person)
			       dbGetObject(OL_NR
					   (GetNthNode
					    (BurglarsList,
					     CurrentPerson))))->PictID);
		    node =
			(NODE *) UnLink(BurglarsList,
					OL_NAME(GetNthNode
						(BurglarsList, CurrentPerson)),
					&help);

		    txtGetFirstLine(PLAN_TXT, "EXPAND_ALL", exp);
		    ExpandObjectList(BurglarsList, exp);

		    choice1 = Bubble(BurglarsList, 0, NULL, 0L);

		    if (ChoiceOk(choice1, GET_OUT, BurglarsList))
			choice1 = OL_NR(GetNthNode(BurglarsList, choice1));
		    else
			choice1 = GET_OUT;

		    Link(BurglarsList, node, help);
		    dbRemObjectNode(BurglarsList, 0L);
		} else {
		    choice1 =
			CurrentPerson ? OL_NR(GetNthNode(BurglarsList, 0)) :
			OL_NR(GetNthNode(BurglarsList, 1));
		    plMessage("RADIO_3",
			      PLANING_MSG_WAIT | PLANING_MSG_REFRESH);
		}

		if (choice1 != GET_OUT) {
		    if (InitAction
			(plSys, ACTION_SIGNAL, choice1, 0L,
			 PLANING_TIME_RADIO * PLANING_CORRECT_TIME)) {
			PlanChanged = 1;

			livAnimate(Planing_Name[CurrentPerson], ANM_MAKE_CALL,
				   0, 0);
			plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
			       PLANING_TIME_RADIO * PLANING_CORRECT_TIME, 1);
			livRefreshAll();
		    } else
			plSay("PLANING_END", CurrentPerson);
		}
	    } else
		plMessage("NO_RADIO", PLANING_MSG_WAIT);
	    break;
	}
    }

    RemoveList(menu);
}

static void plNoteBook(void)
{
    LIST *l, *bubble = txtGoKey(PLAN_TXT, "MENU_6");
    U32 choice1 = 0, choice2 = 0;
    char exp[TXT_KEY_LENGTH];

    while (choice1 != GET_OUT) {
	SetBubbleType(THINK_BUBBLE);

	choice1 = Bubble(bubble, choice1, 0L, 0L);

	choice2 = 0L;

	switch (choice1) {
	case PLANING_NOTE_TARGET:
	    Present(Organisation.BuildingID, "Building", InitBuildingPresent);
	    break;

	case PLANING_NOTE_TEAM:
	    while (choice2 != GET_OUT) {
		txtGetFirstLine(PLAN_TXT, "EXPAND_NOTEBOOK", exp);
		ExpandObjectList(BurglarsList, exp);

		SetBubbleType(THINK_BUBBLE);

		choice2 = Bubble(BurglarsList, choice2, 0L, 0L);

		if (ChoiceOk(choice2, GET_OUT, BurglarsList))
		    Present(OL_NR(GetNthNode(BurglarsList, choice2)), "Person",
			    InitPersonPresent);
		else
		    choice2 = GET_OUT;

		dbRemObjectNode(BurglarsList, 0L);
	    }
	    break;

	case PLANING_NOTE_CAR:
	    Present(Organisation.CarID, "Car", InitCarPresent);
	    break;

	case PLANING_NOTE_TOOLS:
	    hasAll(Person_Matt_Stuvysunt,
		   OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
		   Object_Tool);
	    l = ObjectListPrivate;

	    if (!LIST_EMPTY(l)) {
		txtGetFirstLine(PLAN_TXT, "EXPAND_NOTEBOOK", exp);
		ExpandObjectList(l, exp);

		while (choice2 != GET_OUT) {
		    SetBubbleType(THINK_BUBBLE);

		    choice2 = Bubble(l, choice2, 0L, 0L);

		    if (ChoiceOk(choice2, GET_OUT, l))
			Present(OL_NR(GetNthNode(l, choice2)), "Tool",
				InitToolPresent);
		    else
			choice2 = GET_OUT;
		}
	    }

	    RemoveList(l);
	    break;

	default:
	    choice1 = GET_OUT;
	    break;
	}
    }

    RemoveList(bubble);
}

static void plLook(void)
{
    LIST *menu = txtGoKey(PLAN_TXT, "MENU_7");
    ubyte activ = 0, choice;
    U32 timer = 0L, maxTimer = GetMaxTimer(plSys), realCurrentPerson =
	CurrentPerson, choice1;
    U32 bitset;

    plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
    plSync(PLANING_ANIMATE_NO, timer, maxTimer, 0);
    lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1, (uword) - 1);

    while (activ != PLANING_LOOK_RETURN) {
	plDisplayTimer(timer / PLANING_CORRECT_TIME, 0);
	plDisplayInfo();

	ShowMenuBackground();

	bitset = BIT(PLANING_LOOK_PLAN) + BIT(PLANING_LOOK_RETURN);

	if ((CurrentPerson < BurglarsNr) ? BurglarsNr > 1 : PersonsNr > 1)
	    bitset += BIT(PLANING_LOOK_PERSON_CHANGE);

	inpTurnFunctionKey(0);
	inpTurnESC(0);

	activ = Menu(menu, bitset, activ, NULL, 0);

	inpTurnFunctionKey(1);
	inpTurnESC(1);

	switch (activ) {
	case PLANING_LOOK_PLAN:
	    {
		ubyte last = 0;

		plMessage("LOOK_START", PLANING_MSG_REFRESH);

		inpTurnESC(0);
		inpTurnFunctionKey(0);
		inpTurnMouse(0);
		inpSetKeyRepeat((0 << 5) | 0);

		while (1) {
		    plDisplayTimer(timer / PLANING_CORRECT_TIME, 0);

		    choice = inpWaitFor(INP_MOVEMENT | INP_LBUTTONP);

		    if (choice & INP_LBUTTONP)
			break;

		    if (!(choice & INP_MOUSE)) {
			if (choice & INP_RIGHT) {
			    if (timer < maxTimer) {
				if (last != 1) {
				    livSetPlayMode(LIV_PM_NORMAL);
				    last = 1;
				}

				timer++;
				plSync(PLANING_ANIMATE_STD |
				       PLANING_ANIMATE_FOCUS, timer, 1, 1);
				livDoAnims((AnimCounter++) % 2, 1);
			    }
			}

			if (choice & INP_LEFT) {
			    if (timer > 0) {
				if (last != 2) {
				    livSetPlayMode(LIV_PM_REVERSE);
				    last = 2;
				}

				timer--;
				plSync(PLANING_ANIMATE_STD |
				       PLANING_ANIMATE_FOCUS, timer, 1, 0);
				livDoAnims((AnimCounter++) % 2, 1);
			    }
			}
		    }
		}

		inpSetKeyRepeat((1 << 5) | 10);
		inpTurnMouse(1);
		inpTurnFunctionKey(1);
		inpTurnESC(1);
	    }
	    break;

	case PLANING_LOOK_PERSON_CHANGE:
	    plMessage("CHANGE_PERSON_2", PLANING_MSG_REFRESH);

	    if (PersonsNr > 2)
		choice1 = (U32) Bubble(PersonsList, CurrentPerson, NULL, 0L);
	    else
		choice1 = ((CurrentPerson) ? 0L : 1L);

	    if (choice1 != GET_OUT) {
		plPrepareSys(choice1, 0, PLANING_HANDLER_SET);
		maxTimer = GetMaxTimer(plSys);

		if (livWhereIs(Planing_Name[choice1]) != lsGetActivAreaID()) {
		    lsDoneActivArea(livWhereIs(Planing_Name[choice1]));
		    lsInitActivArea(livWhereIs(Planing_Name[choice1]),
				    livGetXPos(Planing_Name[choice1]),
				    livGetYPos(Planing_Name[choice1]),
				    Planing_Name[choice1]);
		}

		lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1,
				 (uword) - 1);
	    }
	    break;
	}
    }

    livSetPlayMode(LIV_PM_NORMAL);

    CurrentPerson = realCurrentPerson;
    plPrepareSys(CurrentPerson, 0, PLANING_HANDLER_SET);

    if (livWhereIs(Planing_Name[CurrentPerson]) != lsGetActivAreaID()) {
	lsDoneActivArea(livWhereIs(Planing_Name[CurrentPerson]));
	lsInitActivArea(livWhereIs(Planing_Name[CurrentPerson]),
			livGetXPos(Planing_Name[CurrentPerson]),
			livGetYPos(Planing_Name[CurrentPerson]),
			Planing_Name[CurrentPerson]);
    }

    plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);

    if (timer < GetMaxTimer(plSys))
	plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
	       GetMaxTimer(plSys) - timer, 1);

    if (timer > GetMaxTimer(plSys))
	plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys),
	       timer - GetMaxTimer(plSys), 0);

    lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1, (uword) - 1);

    RemoveList(menu);
}

/* Planer */
void plPlaner(U32 objId)
{
    LIST *menu = txtGoKey(PLAN_TXT, "MENU_1");
    ubyte activ = 0;
    U32 bitset;

    plPrepareSys(0L, objId,
		 PLANING_INIT_PERSONSLIST | PLANING_HANDLER_ADD |
		 PLANING_HANDLER_OPEN | PLANING_GUARDS_LOAD |
		 PLANING_HANDLER_SET);
    plPrepareGfx(objId, LS_COLL_PLAN,
		 PLANING_GFX_LANDSCAPE | PLANING_GFX_SPRITES |
		 PLANING_GFX_BACKGROUND);
    plPrepareRel();
    plPrepareData();

    AnimCounter = 0;
    PlanChanged = 0;

    if ((Planing_BldId == Building_Starford_Kaserne)
	&& !(GamePlayMode & GP_LEVEL_DESIGN)) {
	/* in the case of the Starford Barracks the plan must be loaded */
	plLoad(Planing_BldId);

	plPrepareSys(0L, 0, PLANING_HANDLER_SET);

	plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys), 1);
	lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1, (uword) - 1);

	PlanChanged = 0;
    }

    while (activ != PLANING_RETURN) {
	plDisplayTimer(0, 1);
	plDisplayInfo();

	ShowMenuBackground();

	bitset = BIT(PLANING_START) +
	    BIT(PLANING_NOTE) +
	    BIT(PLANING_SAVE) +
	    BIT(PLANING_LOAD) +
	    BIT(PLANING_CLEAR) + BIT(PLANING_LOOK) + BIT(PLANING_RETURN);

	inpTurnESC(0);
	inpTurnFunctionKey(0);
	inpTurnMouse(0);

	activ = Menu(menu, bitset, activ, NULL, 0);

	inpTurnMouse(1);
	inpTurnFunctionKey(1);
	inpTurnESC(1);

	switch (activ) {
	case PLANING_START:
	    plAction();
	    break;

	case PLANING_NOTE:
	    plNoteBook();
	    break;

	case PLANING_SAVE:
	    if (!(GamePlayMode & GP_GUARD_DESIGN) && !plAllInCar(objId))
		plSay("PLAN_NOT_FINISHED", 0);

	    plSave(objId);

	    PlanChanged = 0;
	    break;

	case PLANING_LOAD:
	    plSaveChanged(objId);

	    plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
	    plSync(PLANING_ANIMATE_NO, 0, GetMaxTimer(plSys), 0);

	    plPrepareSys(0L, 0, PLANING_HANDLER_CLEAR);
	    plPrepareData();

	    plLoad(objId);

	    plPrepareSys(0L, 0, PLANING_HANDLER_SET);

	    plMessage("PERSON_NOTES", PLANING_MSG_REFRESH);
	    plSync(PLANING_ANIMATE_NO, GetMaxTimer(plSys), GetMaxTimer(plSys),
		   1);
	    lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1,
			     (uword) - 1);

	    PlanChanged = 0;
	    break;

	case PLANING_CLEAR:
	    plSaveChanged(objId);

	    plMessage("CLEAR_PLAN", PLANING_MSG_REFRESH);

	    plSync(PLANING_ANIMATE_NO, 0, GetMaxTimer(plSys), 0);

	    plPrepareSys(0L, 0, PLANING_HANDLER_CLEAR | PLANING_HANDLER_SET);
	    plPrepareData();

	    lsSetActivLiving(Planing_Name[CurrentPerson], (uword) - 1,
			     (uword) - 1);

	    PlanChanged = 0;
	    break;

	case PLANING_LOOK:
	    plLook();
	    break;
	}
    }

    plSaveChanged(objId);

    plUnprepareRel();
    plUnprepareGfx();
    plUnprepareSys();

    RemoveList(menu);
}

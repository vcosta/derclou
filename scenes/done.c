/*
**	$Filename: scenes/done.c
**	$Release:
**	$Revision:
**	$Date:
**
**
**
**	(C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "scenes/scenes.h"

void DoneTaxi(void)
{
    static ubyte i = 0;
    ubyte j;
    char name[TXT_KEY_LENGTH], exp[TXT_KEY_LENGTH];
    LIST *locs = CreateList();
    struct ObjectNode *n, *new;
    U32 locNr;
    Location loc;

    knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);
    taxiAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Location);

    for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	 n = (struct ObjectNode *) NODE_SUCC(n)) {
	loc = OL_DATA(n);
	locNr = ((Location) loc)->LocationNr;

	sprintf(name, "*%s", NODE_NAME(GetNthNode(film->loc_names, locNr)));

	new =
	    (struct ObjectNode *) CreateNode(locs,
					     sizeof(struct ObjectNode),
					     name);
	new->nr = locNr + 1;	/* because of ChoiceOk */
    }

    i = min(i, GetNrOfNodes(locs) - 1);

    txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
    ExpandObjectList(locs, exp);

    if (ChoiceOk(j = Bubble(locs, i, 0L, 0L), GET_OUT, locs)) {
	i = j;

	locNr = OL_NR(GetNthNode(locs, i)) - 1;
	SceneArgs.ReturnValue =
	    (U32) ((struct Scene *) GetLocScene(locNr)->EventNr);
    } else {
	Say(BUSINESS_TXT, 0, MATT_PICTID, "LOVELY_TAXI");

	SceneArgs.ReturnValue =
	    (U32) ((struct Scene *) GetLocScene(GetOldLocation)->EventNr);
    }

    SceneArgs.Ueberschrieben = 1;
    SceneArgs.Moeglichkeiten = 0L;

    RemoveList(locs);
    gfxChangeColors(l_gc, 2L, GFX_FADE_OUT, 0L);
}

void DoneInsideHouse(void)
{
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");
    U32 choice, buildingID, areaID;
    ubyte activ = 0, perc;
    NODE *node;

    SceneArgs.ReturnValue = 0;
    SceneArgs.Ueberschrieben = 1;

    ShowMenuBackground();

    buildingID = GetObjNrOfBuilding(GetLocation + 1);
    consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

    /* jetzt alle Stockwerke laden */
    for (node = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	areaID = OL_NR(node);

	lsInitRelations(areaID);
	lsInitObjectDB(buildingID, areaID);
    }

    startsWithAll(buildingID, OLF_NORMAL, Object_LSArea);

    areaID = OL_NR(LIST_HEAD(ObjectList));
    lsLoadGlobalData(buildingID, areaID);	/* Stechuhren und so... */
    /* muá nur einmal geladen werden.. */

    lsSetRelations(areaID);

    perc = ((Building) dbGetObject(buildingID))->Exactlyness;
    lsShowRaster(areaID, perc);
    gfxShow(154, GFX_FADE_OUT | GFX_BLEND_UP, 5, -1, -1);

    tcRefreshLocationInTitle(GetLocation);

    while (!SceneArgs.ReturnValue) {
	if (SceneArgs.Moeglichkeiten) {
	    inpTurnFunctionKey(0);
	    inpTurnESC(0);
	    activ =
		Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), NULL, 0L);
	    inpTurnFunctionKey(1);
	    inpTurnESC(1);

	    choice = (U32) 1L << activ;

	    switch (choice) {
	    case LOOK:
		tcInsideOfHouse(buildingID, areaID, perc);
		ShowMenuBackground();
		tcRefreshLocationInTitle(GetLocation);
		AddVTime(19);
		ShowTime(0);
		break;
	    case GO:
		if (!(areaID = tcGoInsideOfHouse(buildingID)))
		    SceneArgs.ReturnValue =
			(U32) (((struct TCEventNode
				 *) (LIST_HEAD((GetCurrentScene()->
						std_succ))))->EventNr);
		else {
		    lsSetRelations(areaID);

		    lsShowRaster(areaID, perc);
		}
		tcRefreshLocationInTitle(GetLocation);
		break;
	    default:
		SceneArgs.ReturnValue = StdHandle(choice);
		break;
	    }
	}
    }

    consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

    /* jetzt alle Stockwerke entfernen */
    for (node = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node))
	lsDoneObjectDB(OL_NR(node));

    StopAnim();
    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);

    RemoveList(menu);
}

void DoneTools(void)
{
    ubyte activ = 0;
    U32 choice = 0;
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");

    SceneArgs.Ueberschrieben = 1;

    tcMoveAPerson(Person_Mary_Bolton, Location_Tools_Shop);
    livesInUnSet(London_London_1, Person_Mary_Bolton);

    while (!SceneArgs.ReturnValue) {
	tcPersonIsHere();

	inpTurnFunctionKey(0);	/* dont save in tools shop */
	inpTurnESC(0);
	activ = Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), NULL, 0L);
	inpTurnESC(1);

	choice = (U32) 1L << (activ);

	if (choice == BUSINESS_TALK) {
	    tcToolsShop();
	    AddVTime(9);
	    ShowTime(0);
	} else
	    SceneArgs.ReturnValue = StdHandle(choice);
    }

    inpTurnFunctionKey(1);

    livesInSet(London_London_1, Person_Mary_Bolton);

    RemoveList(menu);
    StopAnim();
    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneDealer(void)
{
    ubyte activ = 0;
    U32 choice = 0;
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");

    SceneArgs.Ueberschrieben = 1;
    SceneArgs.ReturnValue = 0L;

    tcMoveAPerson(Person_Frank_Maloya, Location_Maloya);
    tcMoveAPerson(Person_Eric_Pooly, Location_Pooly);
    tcMoveAPerson(Person_Helen_Parker, Location_Parker);

    livesInUnSet(London_London_1, Person_Frank_Maloya);	/* damit sie sich */
    livesInUnSet(London_London_1, Person_Eric_Pooly);	/* nicht bewegen! */
    livesInUnSet(London_London_1, Person_Helen_Parker);

    while (!SceneArgs.ReturnValue) {
	inpTurnFunctionKey(0);	/* or call save functions in case of space */
	inpTurnESC(0);
	activ = Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), NULL, 0L);
	inpTurnESC(1);
	inpTurnFunctionKey(1);

	choice = (U32) 1L << (activ);

	if (choice == BUSINESS_TALK)
	    tcDealerDlg();
	else
	    SceneArgs.ReturnValue = StdHandle(choice);
    }

    livesInSet(London_London_1, Person_Frank_Maloya);
    livesInSet(London_London_1, Person_Eric_Pooly);
    livesInSet(London_London_1, Person_Helen_Parker);

    RemoveList(menu);
    StopAnim();
    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneParking(void)
{
    LIST *bubble = txtGoKey(BUSINESS_TXT, "PARKING");
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");
    ubyte activ = 0, choice = 0;
    U32 carID;
    Person marc = dbGetObject(Person_Marc_Smith);

    SceneArgs.Ueberschrieben = 1;
    SceneArgs.ReturnValue = 0;

    while (!SceneArgs.ReturnValue) {
	inpTurnFunctionKey(0);	/* or call save functions in case of space */
	inpTurnESC(0);

	activ = Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), 0L, 0L);

	inpTurnESC(1);
	inpTurnFunctionKey(1);

	if ((1L << activ) == BUSINESS_TALK) {
	    choice = 0;

	    while (choice != 2) {
		choice = Bubble(bubble, 0, 0L, 0L);

		switch (choice) {
		case 0:
		    tcBuyCar();
		    AddVTime(9);
		    choice = 2;
		    break;
		case 1:
		    if ((carID = tcChooseCar(27))) {
			tcSellCar(carID);
			AddVTime(11);
		    } else {
			Say(BUSINESS_TXT, 0, marc->PictID, "SELL_HERE");
			choice = 2;
		    }
		    break;
		default:
		    choice = 2;
		    break;
		}
	    }
	    ShowTime(0);
	} else {
	    inpTurnESC(0);
	    SceneArgs.ReturnValue = StdHandle(1L << activ);
	    inpTurnESC(1);
	}
    }

    RemoveList(bubble);
    RemoveList(menu);
    StopAnim();
    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void DoneGarage(void)
{
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");
    ubyte activ = 0;
    U32 choice, carID;
    Person marc = dbGetObject(Person_Marc_Smith);

    SceneArgs.Ueberschrieben = 1;
    SceneArgs.ReturnValue = 0;

    while (!SceneArgs.ReturnValue) {
	inpTurnFunctionKey(0);	/* or call save functions in case of space */
	inpTurnESC(0);

	activ = Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), NULL, 0);

	inpTurnESC(1);
	inpTurnFunctionKey(1);

	choice = (U32) 1L << (activ);

	if (choice == BUSINESS_TALK) {
	    if ((carID = tcChooseCar(26))) {
		tcCarInGarage(carID);
		AddVTime(9);
		ShowTime(0);
	    } else {
		Say(BUSINESS_TXT, 0, marc->PictID, "REPAIR_HERE");
		inpTurnESC(0);
		SceneArgs.ReturnValue = StdHandle(GO);
		inpTurnESC(1);
	    }
	} else {
	    inpTurnESC(0);
	    SceneArgs.ReturnValue = StdHandle(choice);
	    inpTurnESC(1);
	}
    }

    RemoveList(menu);
    StopAnim();
    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void tcInitFahndung(void)
{
    Player player = dbGetObject(Player_Player_1);

    tcMattGoesTo(59);		/* Bro */

    player->NrOfBurglaries++;
}

void tcDoneFahndung(void)
{
    if (tcStartEvidence()) {
	tcDonePrison();
	SceneArgs.ReturnValue = SCENE_NEW_GAME;
    } else {
	switch (((Player) (dbGetObject(Player_Player_1)))->NrOfBurglaries) {
	case 3:
	    SceneArgs.ReturnValue = SCENE_STATION;
	    break;
	default:
	    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
	    break;
	}
    }

    if (tcIsDeadlock())
	SceneArgs.ReturnValue = SCENE_NEW_GAME;

    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

/*
 * ein schlimmer Murks, aber anders geht s nicht!
 * und, .... die ganze Scheiáe nur wegen den Knochen
 */

void DoneHotelRoom(void)
{
    tcCheckForBones();

    if (setup.Profidisk) {
        tcCheckForDowning();
    }

    StdDone();
}

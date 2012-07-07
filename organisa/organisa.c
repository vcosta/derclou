/*
**	$Filename: organisa/organisa.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     07-04-94
**
**	functions for organisation of a burglary for "Der Clou!"
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

#include "organisa/organisa.h"

U32 tcChooseDestBuilding(U32);
U32 tcChooseEscapeCar(U32);
U32 tcChooseDriver(U32);

void tcChooseAccomplice(void);
void tcSpreadTools(void);
void tcChooseTime(void);
void tcChooseGuys(void);
void tcCheckGuyCount(void);
void tcAddGuyToParty(void);
void tcRemGuyFromParty(void);

void tcAddToolToGuy(void);
void tcRemToolFromGuy(void);

ubyte tcCheckOrganisation(void);

/* display functions */

void tcInitDisplayOrganisation(void);
void tcDoneDisplayOrganisation(void);

void tcDisplayOrganisation(void);

void tcDisplayCommon(void);
void tcDisplayPerson(U32 displayMode);
void tcDisplayAbilities(U32 personNr, U32 displayData);
void tcDisplayTools(U32 personNr, U32 displayData);

struct Organisation Organisation;

#include "organisa/display.c"

void tcResetOrganisation(void)
{
    Organisation.CarID = 0;
    Organisation.DriverID = 0;
    Organisation.BuildingID = 0;
    Organisation.GuyCount = 0;
    Organisation.PlacesInCar = 0;
}

static void tcOrganisationSetBuilding(void)
{
    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);
    Organisation.BuildingID = OL_NR(LIST_HEAD(ObjectList));
}

static void tcOrganisationSetCar(void)
{
    Car car;

    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

    Organisation.CarID = OL_NR(LIST_HEAD(ObjectList));

    car = dbGetObject(Organisation.CarID);

    Organisation.PlacesInCar = car->PlacesInCar;
}

static ubyte tcMakeCarOk(void)
{
    joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_INSERT_STAR,
		 Object_Person);

    if (GetNrOfNodes(ObjectList) > Organisation.PlacesInCar) {
	SetBubbleType(THINK_BUBBLE);
	if (GET_OUT ==
	    Say(BUSINESS_TXT, 0,
		((Person) dbGetObject(Person_Matt_Stuvysunt))->PictID,
		"PLAN_TO_MANY_GUYS"))
	    return 0;

	while (GetNrOfNodes(ObjectList) > Organisation.PlacesInCar) {
	    ubyte choice;

	    dbRemObjectNode(ObjectList, Person_Matt_Stuvysunt);

	    inpTurnESC(0);

	    choice = Bubble(ObjectList, 0, 0L, 0L);
	    Organisation.GuyCount--;
	    joined_byUnSet(Person_Matt_Stuvysunt,
			   OL_NR(GetNthNode(ObjectList, (U32) choice)));

	    inpTurnESC(1);

	    joined_byAll(Person_Matt_Stuvysunt,
			 OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Person);
	}
    }

    return 1;
}

U32 tcOrganisation(void)
{
    LIST *menu = txtGoKey(MENU_TXT, "ORGANISATION");
    ubyte activ = 0, ende = 0;
    char line[TXT_KEY_LENGTH];

	/* activate first or memorized building */
    Organisation.BuildingID = 0;
    Organisation.CarID = 0;
    Organisation.DriverID = 0;

    rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

    if (!LIST_EMPTY(ObjectList))
	Organisation.BuildingID = OL_NR(LIST_HEAD(ObjectList));

    if (Organisation.BuildingID) {
	if (!has(Person_Matt_Stuvysunt, Organisation.BuildingID))
	    tcOrganisationSetBuilding();
    } else
	tcOrganisationSetBuilding();

    /* remember current building */
    rememberSet(Person_Matt_Stuvysunt, Organisation.BuildingID);

	/* activate first or memorized car */
    rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

    if (!LIST_EMPTY(ObjectList)) {
	Car car;

	Organisation.CarID = OL_NR(LIST_HEAD(ObjectList));

	car = dbGetObject(Organisation.CarID);

	Organisation.PlacesInCar = car->PlacesInCar;
    }

    if (Organisation.CarID) {
	if (!has(Person_Matt_Stuvysunt, Organisation.CarID))
	    tcOrganisationSetCar();
    } else
	tcOrganisationSetCar();

    /* remember current car */
    rememberSet(Person_Matt_Stuvysunt, Organisation.CarID);

	/* activate first or memorized driver */
    rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

    if (!LIST_EMPTY(ObjectList))
	Organisation.DriverID = OL_NR(LIST_HEAD(ObjectList));

    if (!tcMakeCarOk())
	return 0L;

    tcInitDisplayOrganisation();
    tcDisplayOrganisation();

    txtGetFirstLine(THECLOU_TXT, "ORGANISATION", line);

    while (!ende) {
	inpTurnESC(0);
	inpTurnFunctionKey(0);

	PrintStatus(line);

	activ = Menu(menu, (U32) 255, activ, 0, 0);
	inpTurnESC(1);

	switch (activ) {
	case 0:
	    Organisation.BuildingID =
		tcChooseDestBuilding(Organisation.BuildingID);
	    tcDisplayOrganisation();
	    break;
	case 1:
	    tcChooseGuys();
	    /* tcDisplayOrganisation(); done in tcChooseGuys */
	    break;
	case 2:
	    Organisation.CarID = tcChooseEscapeCar(Organisation.CarID);
	    tcDisplayOrganisation();
	    break;
	case 3:
	    Organisation.DriverID = tcChooseDriver(Organisation.DriverID);
	    tcDisplayOrganisation();
	    break;
	case 4:
	    Information();
	    tcDisplayOrganisation();
	    break;
	case 5:
	    if (!Organisation.BuildingID)
		Organisation.BuildingID =
		    tcChooseDestBuilding(Organisation.BuildingID);

	    if (tcCheckOrganisation()) {
		gfxClearArea(l_gc);

		tcDoneDisplayOrganisation();

		plPlaner(Organisation.BuildingID);

		tcInitDisplayOrganisation();
		tcDisplayOrganisation();
	    }
	    break;
	case 6:
	    if (tcCheckOrganisation())
		ende = 2;
	    break;
	case 7:
	    ende = 1;		/* ende */
	    break;
	default:
	    break;
	}
    }

    RemoveList(menu);

    tcDoneDisplayOrganisation();

    return ((ende - 1) * Organisation.BuildingID);
}

ubyte tcCheckOrganisation(void)
{
    Player player = dbGetObject(Player_Player_1);
    ubyte check = 0;

    if (Organisation.BuildingID) {
	if ((((Building) dbGetObject(Organisation.BuildingID))->Exactlyness) >
	    127) {
	    if (Organisation.DriverID) {
		if (player->NrOfBurglaries == 8) {
		    if (Organisation.CarID == Car_Jaguar_XK_1950)
			check = 1;
		    else
			Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_JAGUAR");
		} else {
		if (setup.Profidisk) {
		    if (Organisation.BuildingID == Building_Westminster_Abbey) {
			if ((Organisation.CarID == Car_Fiat_634_N_1936)
			    || (Organisation.CarID == Car_Fiat_634_N_1943))
			    check = 1;
			else
			    Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_FIAT");
		    } else
			check = 1;
		} else {
		    check = 1;
                }
              }
	    } else
		Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_DRIVER");
	} else
	    Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_KNOWL");
    } else
	Say(BUSINESS_TXT, 0, MATT_PICTID, "PLAN_NO_BUILDING");

    return (check);
}

U32 tcChooseDriver(U32 persID)
{
    LIST *list;
    ubyte choice;
    Person matt = (Person) dbGetObject(Person_Matt_Stuvysunt);
    U32 newPersID;

    joined_byAll(Person_Matt_Stuvysunt,
		 OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
		 Object_Person);
    list = ObjectListPrivate;

    if (LIST_EMPTY(list)) {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_TO_FEW_GUYS");
    } else {
	char exp[TXT_KEY_LENGTH];

	txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
	ExpandObjectList(list, exp);

	if (ChoiceOk(choice = Bubble(list, 0, 0L, 0L), GET_OUT, list)) {
	    newPersID = OL_NR(GetNthNode(list, (U32) choice));

	    if (!has(newPersID, Ability_Autos)) {
		Person pers = dbGetObject(newPersID);

		Say(BUSINESS_TXT, 0, pers->PictID, "PLAN_CANT_DRIVE");
	    } else {
		persID = newPersID;

		rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

		if (!LIST_EMPTY(ObjectList))
		    rememberUnSet(Person_Matt_Stuvysunt,
				  OL_NR(LIST_HEAD(ObjectList)));

		rememberSet(Person_Matt_Stuvysunt, persID);
	    }
	}
    }

    RemoveList(list);

    return (persID);
}

U32 tcChooseDestBuilding(U32 objID)
{
    LIST *list;
    ubyte choice;
    char exp[TXT_KEY_LENGTH];

    hasAll(Person_Matt_Stuvysunt,
	   OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	   Object_Building);
    list = ObjectListPrivate;

    txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
    ExpandObjectList(list, exp);

    if (ChoiceOk(choice = Bubble(list, 0, 0L, 0L), GET_OUT, list)) {
	objID = OL_NR(GetNthNode(list, (U32) choice));

	rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

	if (!LIST_EMPTY(ObjectList))
	    rememberUnSet(Person_Matt_Stuvysunt, OL_NR(LIST_HEAD(ObjectList)));

	rememberSet(Person_Matt_Stuvysunt, objID);
    }

    RemoveList(list);

    return (objID);
}

U32 tcChooseEscapeCar(U32 objID)
{
    LIST *l1, *l2;
    ubyte choice;
    Person matt = (Person) dbGetObject(Person_Matt_Stuvysunt);
    U32 newObjID;

    hasAll(Person_Matt_Stuvysunt,
	   OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST, Object_Car);
    l1 = ObjectListPrivate;

    joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST,
		 Object_Person);
    l2 = ObjectListPrivate;

    if (!LIST_EMPTY(l1)) {
	Car car;
	char exp[TXT_KEY_LENGTH];

	txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
	ExpandObjectList(l1, exp);

	if (ChoiceOk(choice = Bubble(l1, 0, 0L, 0L), GET_OUT, l1)) {
	    newObjID = OL_NR(GetNthNode(l1, (U32) choice));

	    car = (Car) OL_DATA(GetNthNode(l1, (U32) choice));

	    if (GetNrOfNodes(l2) <= car->PlacesInCar) {
		Organisation.PlacesInCar = car->PlacesInCar;
		objID = newObjID;

		rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

		if (!LIST_EMPTY(ObjectList))
		    rememberUnSet(Person_Matt_Stuvysunt,
				  OL_NR(LIST_HEAD(ObjectList)));

		rememberSet(Person_Matt_Stuvysunt, objID);
	    } else {
		SetBubbleType(THINK_BUBBLE);
		Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_NO_PLACE");
	    }
	}
    } else {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_WITHOUT_CAR");
    }

    RemoveList(l2);
    RemoveList(l1);

    return (objID);
}

void tcChooseGuys(void)
{
    LIST *list;
    Person matt = (Person) dbGetObject(Person_Matt_Stuvysunt);

    joinAll(Person_Matt_Stuvysunt,
	    OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	    Object_Person);
    list = ObjectListPrivate;

    dbRemObjectNode(list, Person_Matt_Stuvysunt);

    if (LIST_EMPTY(list)) {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_WITHOUT_GUYS");
    } else {
	LIST *menu = txtGoKey(MENU_TXT, "ORG_KOMPLIZEN");
	char line[TXT_KEY_LENGTH];
	ubyte activ = 0;

	ShowMenuBackground();

	txtGetFirstLine(THECLOU_TXT, "KOMPLIZEN", line);
	PrintStatus(line);

	while (activ != 2) {
	    inpTurnESC(0);
	    activ = Menu(menu, 7, activ, NULL, 0);
	    inpTurnESC(1);

	    switch (activ) {
	    case 0:
		tcAddGuyToParty();
		break;
	    case 1:
		tcRemGuyFromParty();
		break;
	    default:
		break;
	    }
	}

	ShowMenuBackground();
	RemoveList(menu);
    }

    RemoveList(list);
}

void tcAddGuyToParty(void)
{
    LIST *l1, *l2;
    struct ObjectNode *n;
    U32 persID;
    ubyte choice;

    joinAll(Person_Matt_Stuvysunt,
	    OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
	    Object_Person);
    l1 = ObjectListPrivate;

    joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME | OLF_PRIVATE_LIST,
		 Object_Person);
    l2 = ObjectListPrivate;

    if (GetNrOfNodes(l2) < Organisation.PlacesInCar) {
	for (n = (struct ObjectNode *) LIST_HEAD(l2); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n))
	    dbRemObjectNode(l1, OL_NR(n));

	if (!LIST_EMPTY(l1)) {
	    char exp[TXT_KEY_LENGTH];

	    txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
	    ExpandObjectList(l1, exp);

	    if (ChoiceOk(choice = Bubble(l1, 0, 0L, 0L), GET_OUT, l1)) {
		persID =
		    (U32) (((struct ObjectNode *)
			    GetNthNode(l1, (U32) choice))->nr);

		Organisation.GuyCount++;
		joined_bySet(Person_Matt_Stuvysunt, persID);

		tcDisplayOrganisation();
	    }
	} else {
	    SetBubbleType(THINK_BUBBLE);
	    Say(BUSINESS_TXT, 0,
		((Person) dbGetObject(Person_Matt_Stuvysunt))->PictID,
		"PLAN_DO_NOT_KNOW_ANYBODY");
	}
    } else {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0,
	    ((Person) dbGetObject(Person_Matt_Stuvysunt))->PictID,
	    "PLAN_CAR_FULL");
    }

    RemoveList(l2);
    RemoveList(l1);
}

void tcRemGuyFromParty(void)
{
    LIST *list;
    Person matt = (Person) dbGetObject(Person_Matt_Stuvysunt);

    joined_byAll(Person_Matt_Stuvysunt,
		 OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_PRIVATE_LIST,
		 Object_Person);
    list = ObjectListPrivate;

    dbRemObjectNode(list, Person_Matt_Stuvysunt);

    if (LIST_EMPTY(list)) {
	SetBubbleType(THINK_BUBBLE);
	Say(BUSINESS_TXT, 0, matt->PictID, "PLAN_TO_FEW_GUYS");
    } else {
	ubyte choice;
	U32 persID;
	char exp[TXT_KEY_LENGTH];

	txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
	ExpandObjectList(list, exp);

	if (ChoiceOk(choice = Bubble(list, 0, 0L, 0L), GET_OUT, list)) {
	    persID =
		(U32) (((struct ObjectNode *) GetNthNode(list, (U32) choice))->
		       nr);

	    Organisation.GuyCount--;
	    joined_byUnSet(Person_Matt_Stuvysunt, persID);

	    if (persID == Organisation.DriverID) {
		rememberAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

		if (!LIST_EMPTY(ObjectList))
		    rememberUnSet(Person_Matt_Stuvysunt,
				  OL_NR(LIST_HEAD(ObjectList)));

		Organisation.DriverID = 0L;
	    }

	    tcDisplayOrganisation();
	}
    }

    RemoveList(list);
}

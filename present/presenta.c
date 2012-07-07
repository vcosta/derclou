/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
/*
 * Module Present
 *
 * PresentApp.c
 *
 */

#include "present/present.h"

void InitEvidencePresent(U32 nr, LIST * presentationData, LIST * texts)
{
    Evidence e = dbGetObject(nr);
    char data[TXT_KEY_LENGTH];

    dbGetObjectName(e->pers, data);

    AddPresentTextLine(presentationData, data, 0, texts, 0);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->Recognition, 255,
		   texts, 1);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->WalkTrail, 255,
		   texts, 2);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->WaitTrail, 255,
		   texts, 3);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->WorkTrail, 255,
		   texts, 4);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->KillTrail, 255,
		   texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->CallTrail, 255,
		   texts, 6);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->FotoTrail, 255,
		   texts, 8);
    AddPresentLine(presentationData, PRESENT_AS_BAR, e->PaperTrail, 255,
		   texts, 7);
}

void InitLootPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    CompleteLoot comp = dbGetObject(CompleteLoot_LastLoot);
    U32 total;

    RemoveList(tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

    total = comp->Bild + comp->Gold + comp->Geld + comp->Juwelen +
	comp->Statue + comp->Kuriositaet + comp->HistKunst +
	comp->GebrauchsArt + comp->Vase + comp->Delikates;

    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Bild, 0,
		   texts, 0);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Gold, 0,
		   texts, 1);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Geld, 0,
		   texts, 2);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Juwelen, 0,
		   texts, 3);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Delikates, 0,
		   texts, 4);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Statue, 0,
		   texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Kuriositaet,
		   0, texts, 6);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->HistKunst, 0,
		   texts, 7);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->GebrauchsArt,
		   0, texts, 8);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, comp->Vase, 0,
		   texts, 9);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, total, 0, texts, 10);
}

void InitOneLootPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    Loot loot = dbGetObject(nr);
    char data[TXT_KEY_LENGTH];
    U32 value;

    if (loot->Name == Kein_Name)
	dbGetObjectName(nr, data);
    else
	txtGetNthString(OBJECTS_ENUM_TXT, "enum_LootNameE", loot->Name, data);

    AddPresentTextLine(presentationData, data, 0, texts, 4);

    /* some nice guy put the value in this relation */
    /* (... and because he is really nice, he will kill this one after usage) */
    value = GetP(loot, hasLootRelationID, loot);

    AddPresentLine(presentationData, PRESENT_AS_NUMBER, value, 0, texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, loot->Weight, 0,
		   texts, 6);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, loot->Volume, 0,
		   texts, 7);
}

void InitObjectPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    LSObject lso = dbGetObject(nr);
    char data[TXT_KEY_LENGTH];
    LIST *l;

    dbGetObjectName(lso->Type, data);

    AddPresentTextLine(presentationData, data, 0, texts, 0);

    if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM)
	AddPresentTextLine(presentationData, NULL, 0, texts, 2);

    if (lso->uch_Chained & Const_tcCHAINED_TO_POWER)
	AddPresentTextLine(presentationData, NULL, 0, texts, 1);

    if (lso->Type == Item_Stechuhr)
	AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		       ClockTimerGet(nr, nr), 0, texts, 8);

    l = tcMakeLootList(nr, hasLootRelationID);

    if (LIST_EMPTY(l))
	AddPresentTextLine(presentationData, NULL, 0, texts, 3);

    RemoveList(l);
}

void InitToolPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    char data[TXT_KEY_LENGTH];
    ubyte i;
    Tool obj;
    NODE *n;
    LIST *tools = txtGoKey(OBJECTS_ENUM_TXT, "enum_ItemE");
    LIST *abilities = txtGoKey(OBJECTS_ENUM_TXT, "enum_AbilityE");

    obj = (Tool) dbGetObject(nr);

    dbGetObjectName(nr, data);

    AddPresentTextLine(presentationData, data, 0, texts, 0);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		   tcGetToolTraderOffer(obj), 0, texts, 1);
    AddPresentLine(presentationData, PRESENT_AS_BAR, tcRGetDanger(obj),
		   255, texts, 3);
    AddPresentLine(presentationData, PRESENT_AS_BAR, tcRGetVolume(obj),
		   255, texts, 4);

     /*** Werkzeuge ***/

    toolRequiresAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Tool);

    for (n = (NODE *) LIST_HEAD(ObjectList), i = 5; NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n), i = 6)
	AddPresentTextLine(presentationData, NODE_NAME(n), 0, texts, i);

     /*** Eigenschaften ***/

    toolRequiresAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Ability);

    if (!LIST_EMPTY(ObjectList)) {
	AddPresentTextLine(presentationData, NULL, 0, texts, 8);	/* "ben”tigt Wissen ber..." */

	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    Ability ability = OL_DATA(n);

	    AddPresentLine(presentationData, PRESENT_AS_BAR,
			   toolRequiresGet(nr, OL_NR(n)), 255, abilities,
			   ability->Name);
	}
    }

	  /*** Verwendung ***/

    breakAll(nr, OLF_INCLUDE_NAME | OLF_NORMAL, Object_Item);

    if (!(LIST_EMPTY(ObjectList)))
	AddPresentTextLine(presentationData, NULL, 0, texts, 7);

    for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	U32 itemNr = OL_NR(n);
	U32 time = breakGet(nr, itemNr);
	Item item = OL_DATA(n);

	sprintf(data, "%.2d:%.2d", time / 60, time % 60);

	AddPresentTextLine(presentationData, data, 0, tools, item->Type);
    }

    RemoveList(tools);
    RemoveList(abilities);
}

void InitBuildingPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    char data[TXT_KEY_LENGTH];
    Building obj;

    obj = (Building) dbGetObject(nr);

    dbGetObjectName(nr, data);
    AddPresentTextLine(presentationData, data, 0, texts, 0);

    BuildTime(tcGetBuildPoliceT(obj), data);
    AddPresentTextLine(presentationData, data, 0, texts, 1);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		   tcGetBuildValues(obj), 255, texts, 2);

    txtGetNthString(OBJECTS_ENUM_TXT, "enum_RouteE", obj->EscapeRoute, data);
    AddPresentTextLine(presentationData, data, 0, texts, 3);

    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Exactlyness, 255,
		   texts, 4);
    AddPresentLine(presentationData, PRESENT_AS_BAR, tcGetBuildGRate(obj),
		   255, texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_BAR,
		   tcRGetBuildStrike(obj), 255, texts, 6);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->RadioGuarding,
		   255, texts, 7);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->GuardStrength,
		   255, texts, 8);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->MaxVolume, 255,
		   texts, 9);
}


void InitPlayerPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    Player player = (Player) dbGetObject(nr);

    AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->Money, 0,
		   texts, 0);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		   player->StolenMoney, 0, texts, 1);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		   player->MyStolenMoney, 0, texts, 2);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, player->MattsPart,
		   0, texts, 3);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER,
		   player->NrOfBurglaries, 0, texts, 4);
}

void InitPersonPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    char data[TXT_KEY_LENGTH];
    ubyte i;
    LIST *abilities = NULL;
    NODE *node;
    U32 abiNr;
    Person obj;
    Ability abi;

    obj = (Person) dbGetObject(nr);

    dbGetObjectName(nr, data);
    AddPresentTextLine(presentationData, data, 0, texts, 0);

    txtGetNthString(OBJECTS_ENUM_TXT, "enum_JobE", obj->Job, data);
    AddPresentTextLine(presentationData, data, 0L, texts, 1);

    txtGetNthString(OBJECTS_ENUM_TXT, "enum_SexE", obj->Sex, data);
    AddPresentTextLine(presentationData, data, 0L, texts, 2);

    AddPresentLine(presentationData, PRESENT_AS_NUMBER, obj->Age, 0L, texts, 3);

    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Health, 255,
		   texts, 4);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Mood, 255, texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Intelligence,
		   255, texts, 6);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Strength, 255,
		   texts, 7);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Stamina, 255,
		   texts, 8);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Loyality, 255,
		   texts, 9);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Skill, 255,
		   texts, 10);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Known, 255,
		   texts, 11);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Popularity, 255,
		   texts, 12);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Avarice, 255,
		   texts, 13);
    AddPresentLine(presentationData, PRESENT_AS_BAR,
		   (UINT32_C(255) - obj->Panic), 255, texts, 14);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->KnownToPolice,
		   255, texts, 15);

    hasAll(nr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	   Object_Ability);
    abilities = ObjectListPrivate;

    if (!(LIST_EMPTY(abilities))) {
	AddPresentTextLine(presentationData, NULL, 0, texts, 16);

	for (node = (NODE *) LIST_HEAD(abilities), i = 0; NODE_SUCC(node);
	     node = (NODE *) NODE_SUCC(node), i++) {
	    abiNr = ((struct ObjectNode *) GetNthNode(abilities, (U32) i))->nr;
	    abi = (Ability) dbGetObject(abiNr);

	    txtGetNthString(OBJECTS_ENUM_TXT, "enum_AbilityE", abi->Name, data);
	    CreateNode(texts, 0, data);

	    AddPresentLine(presentationData, PRESENT_AS_BAR,
			   hasGet(nr, abiNr), 255, texts, 17 + i);
	}
    }

    RemoveList(abilities);
}

void InitCarPresent(U32 nr, LIST * presentationData, LIST * texts)
{
    char data[TXT_KEY_LENGTH];
    Car obj;

    obj = (Car) dbGetObject(nr);

    dbGetObjectName(nr, data);
    AddPresentTextLine(presentationData, data, 0, texts, 0);

    txtGetNthString(OBJECTS_ENUM_TXT, "enum_LandE", obj->Land, data);
    AddPresentTextLine(presentationData, data, 0, texts, 1);

    AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcRGetCarAge(obj),
		   0, texts, 2);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarPrice(obj),
		   0, texts, 3);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarPS(obj), 0,
		   texts, 4);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetCarSpeed(obj),
		   0, texts, 5);
    AddPresentLine(presentationData, PRESENT_AS_NUMBER, obj->PlacesInCar,
		   0, texts, 6);

    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->State, 255, texts, 7);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->BodyWorkState,
		   255, texts, 8);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->TyreState, 255,
		   texts, 9);
    AddPresentLine(presentationData, PRESENT_AS_BAR, obj->MotorState, 255,
		   texts, 10);

    if (setup.Profidisk) {
        AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Capacity,
		       9000000, texts, 11);
    } else {
        AddPresentLine(presentationData, PRESENT_AS_BAR, obj->Capacity,
		       7000000, texts, 11);
    }

    AddPresentLine(presentationData, PRESENT_AS_BAR, tcGetCarStrike(obj),
		   255, texts, 12);
}

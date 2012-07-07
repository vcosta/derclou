/*
**	$Filename: story/story.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**
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

#include "story/story.h"

ubyte tcDoLastBurglarySpot(U32 ul_Time, U32 ul_BuildingId);

static void tcSomebodyIsComing(void)
{
    ubyte i;

    sndPrepareFX("klopfen.voc");

    for (i = 0; i < 3; i++) {
	inpDelay(50);
	sndPlayFX();
    }
}

static void tcSomebodyIsCalling(void)
{
    ubyte i;

    sndPrepareFX("ring.voc");

    for (i = 0; i < CalcRandomNr(1, 4); i++) {
	inpDelay(180);
	sndPlayFX();
    }
}

ubyte tcKarateOpa(U32 ul_ActionTime, U32 ul_BuildingId)
{
    livSetAllInvisible();
    lsSetViewPort(0, 0);	/* links, oben */

    gfxShow(217, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

    return 1;
}

void tcDoneCredits(void)
{
    Person ben = dbGetObject(Person_Ben_Riggley);

    tcSomebodyIsComing();

    Say(THECLOU_TXT, 0, ben->PictID, "A_LETTER_FOR_YOU");
    Say(THECLOU_TXT, 0, LETTER_PICTID, "SOME_CREDITS");
    Say(THECLOU_TXT, 0, MATT_PICTID, "SO_EIN_SCH");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}


/* THE ARRIVAL (ST_30)
-----------------------------------------------------------------*/

void tcDoneArrival(void)
{
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_30_OLD");

    AddTaxiLocation(0);		/* holland */
    AddTaxiLocation(58);	/* victoria station */

    SceneArgs.ReturnValue = SCENE_STATION;
}

/* HOTEL RECEPTION
-----------------------------------------------------------------*/

void tcDoneHotelReception(void)
{
    Person rig = (Person) dbGetObject(Person_Ben_Riggley);
    Environment env = (Environment) dbGetObject(Environment_TheClou);

    knowsSet(Person_Matt_Stuvysunt, Person_Ben_Riggley);

    if (env->MattHasHotelRoom == 2)	/* er hat es bereits ! */
	SceneArgs.ReturnValue = SCENE_HOTEL;
    else {
	AddTaxiLocation(2);	/* watling */
	AddTaxiLocation(1);	/* cars */

	if (tcGetPlayerMoney >= tcCOSTS_FOR_HOTEL) {
	    Say(STORY_0_TXT, 0, MATT_PICTID, "THE_KEY_PLEASE");
	    Say(STORY_0_TXT, 0, rig->PictID, "I_LL_FETCH_THE_KEY");
	    Say(STORY_0_TXT, 0, MATT_PICTID, "THANKS_FOR_KEY");

	    env->MattHasHotelRoom = 2;

	    tcSetPlayerMoney(tcGetPlayerMoney - tcCOSTS_FOR_HOTEL);

	    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
	} else {
	    if (env->MattHasHotelRoom == 0) {	/* 1. mal da */
		ubyte choice = 2, evaluation = 0;

		while ((choice == 2) || (choice == 3)) {
		    choice = Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_1");

		    switch (choice) {
		    case 0:
			Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_11");

			if (Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_2") ==
			    1) {
			    evaluation = 1;
			    Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_22");
			} else {
			    Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_21");
			    Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_3");
			    Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_31");
			}
			break;
		    case 1:
			Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_12");
			Say(STORY_0_TXT, 0, MATT_PICTID, "HOTEL_MATT_3");
			Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_31");
			break;
		    case 2:
			Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_13");
			break;
		    case 3:
			Say(STORY_0_TXT, 0, rig->PictID, "HOTEL_ANT_14");
			break;
		    case 4:
			break;
		    }
		}

		if (!evaluation) {
		    SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
		    env->MattHasHotelRoom = 1;
		} else {
		    env->MattHasHotelRoom = 2;
		    SetBubbleType(THINK_BUBBLE);
		    Say(STORY_0_TXT, 0, MATT_PICTID, "MILLIONAIRE");

		    Say(STORY_0_TXT, 0, MATT_PICTID, "THANKS_FOR_KEY");

		    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
		}
	    } else {
		if (env->MattHasHotelRoom == 1) {
		    Say(STORY_0_TXT, 0, rig->PictID, "YOU_HAVE_NO_MONEY");

		    SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
		}
	    }
	}
    }

    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
}

/* ZIMMER _ MAMI (ST_31)
-----------------------------------------------------------------*/

void tcDoneMamiCalls(void)
{
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_31_OLD_0");

    Say(STORY_0_TXT, 0, MATT_PICTID, "ST_31_MATT_0");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ST_31_OLD_1");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

/* CASH FROM GLUDO
-----------------------------------------------------------------*/

void tcDoneGludoMoney(void)
{
    Person Gludo = (Person) dbGetObject(Person_John_Gludo);
    Environment env = dbGetObject(Environment_TheClou);
    ubyte choice;

    knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

    choice = Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_1");

    switch (choice) {
    case 0:
	Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_11");
	Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_2");
	Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_21");
	Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_5");
	Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_51");
	tcSetPlayerMoney(tcGetPlayerMoney + tcCOSTS_FOR_HOTEL);
	break;
    case 1:
	Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_12");
	break;
    case 2:
	Say(STORY_0_TXT, 0, Gludo->PictID, "POLI_ANT_13");
	break;
    case 3:
	Say(BUSINESS_TXT, 0, Gludo->PictID, "Bye");
	break;
    }

    SetBubbleType(THINK_BUBBLE);
    Say(STORY_0_TXT, 0, MATT_PICTID, "POLI_MATT_6");

    env->MattHasIdentityCard = 1;

    StopAnim();
    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_WATLING;
}

/* CASH FROM DANNER
-----------------------------------------------------------------*/

void tcDoneDanner(void)
{
    Person Jim = (Person) dbGetObject(Person_Jim_Danner);
    ubyte choice;

    if (tcGetPlayerMoney < tcCOSTS_FOR_HOTEL) {
	knowsSet(Person_Matt_Stuvysunt, Person_Jim_Danner);

	Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_1");
	choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DANNER_2");

	switch (choice) {
	case 0:
	    Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_21");
	    break;
	case 1:
	    Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_22");
	    break;
	case 2:
	    Say(STORY_0_TXT, 0, Jim->PictID, "DANNER_23");
	    SetBubbleType(THINK_BUBBLE);
	    break;
	}

	Say(STORY_0_TXT, 0, MATT_PICTID, "DANNER_3");
	tcAddPlayerMoney(20);

	livesInUnSet(London_London_1, Person_Jim_Danner);
	tcMoveAPerson(Person_Jim_Danner, Location_Nirvana);
    } else
	livesInSet(London_London_1, Person_Jim_Danner);

    SceneArgs.ReturnValue = SCENE_CARS_VANS;
}

/* MEETING BRIGGS
-----------------------------------------------------------------*/

void tcDoneMeetBriggs(void)
{
    Person Briggs = (Person) dbGetObject(Person_Herbert_Briggs);
    ubyte choice;

#ifdef DEEP_DEBUG
    printf("tcDoneMeetBriggs!\n");
#endif
    knowsSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);

    Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_1");
    Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_1");
    Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_2");
    Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_2");
    Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_3");

    choice = Say(STORY_0_TXT, 0, MATT_PICTID, "BRIGGS_MATT_3");

    if (choice == 0) {		/* angenommen ! */
	Building bui = (Building) dbGetObject(Building_Kiosk);

	hasSet(Person_Matt_Stuvysunt, Building_Kiosk);

	tcAddPlayerMoney(15);

	tcAddBuildExactlyness(bui, 255L);
	tcAddBuildStrike(bui, 5L);

	Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_4");

	Present(Car_Fiat_Topolino_1940, "Car", InitCarPresent);
	Present(Building_Kiosk, "Building", InitBuildingPresent);

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "AFTER_MEETING_BRIGGS");

	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1940);

	SceneArgs.ReturnValue = SCENE_FAT_MANS;
    } else {			/* nicht angenommen ! */

	Person james = dbGetObject(Person_Pater_James);

	Say(STORY_0_TXT, 0, Briggs->PictID, "BRIGGS_BRIGGS_5");

	gfxShow(170, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "BRIGGS_MR_WHISKY");

	tcAsTimeGoesBy(GetMinute + 793);

	StopAnim();
	gfxChangeColors(l_gc, 8, GFX_FADE_OUT, 0);

	tcMattGoesTo(60);	/* Kloster */

	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "KLOSTER");
	Say(STORY_0_TXT, 0, james->PictID, "ABT");

	choice = Say(STORY_0_TXT, 0, MATT_PICTID, "HOLY_MATT");

	if (choice == 0) {	/* holy */
	    sndPlaySound("end.bk", 0);

	    Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");	/* pict = holy matt ! */
	    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "THE_EDGE");

	    SceneArgs.ReturnValue = SCENE_NEW_GAME;
	} else {		/* evil */

	    Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT");
	    Say(STORY_0_TXT, 0, james->PictID, "EVIL_MATT_ABT");
	    Say(STORY_0_TXT, 0, MATT_PICTID, "EVIL_MATT_1");

	    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "EVIL_OLD_MATT");

	    hasSetP(Person_Matt_Stuvysunt, Loot_Ring_des_Abtes,
		    tcVALUE_OF_RING_OF_PATER);

	    AddVTime(1440 + 525 - GetMinute);

	    SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
	}

	StopAnim();
	gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
    }

    AddTaxiLocation(10);	/* trafik */
    AddTaxiLocation(18);	/* pink */
    AddTaxiLocation(20);	/* senioren */
    AddTaxiLocation(12);	/* aunt */

    if (setup.Profidisk) {
        AddTaxiLocation(68);	/* baker street */
    }
}

void tcDoneFreeTicket(void)
{
    Person dan = (Person) dbGetObject(Person_Dan_Stanford);

    knowsSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);

    Say(STORY_0_TXT, 0, 7, "AEHHH");
    Say(STORY_0_TXT, 0, dan->PictID, "FREE_TICKET");

    SceneArgs.ReturnValue = GetLocScene(8)->EventNr;
}
#if 0
static void tcDoneAfterMeetingBriggs(void)
{
    SetBubbleType(THINK_BUBBLE);
    Say(STORY_0_TXT, 0, MATT_PICTID, "AFTER_MEETING_BRIGGS");

    SceneArgs.ReturnValue = SCENE_WATLING;
}
#endif
void tcDoneCallFromPooly(void)
{
    ubyte choice;

    knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);

    if (has(Person_Matt_Stuvysunt, Loot_Ring_des_Abtes)) {
	tcSomebodyIsCalling();

	Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");

	Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_0");
	choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DEALER_MATT_1");

	if ((choice == 0) || (choice == 1))
	    Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_1");
	else
	    Say(STORY_0_TXT, 0, PHONE_PICTID, "DEALER_2");
    }

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

void tcInitPrison(void)
{
    /* es ist wichtig, da· die Location hier gesetzt wird */
    /* da sonst diverse Szenen in die GefÑngnisszene      */
    /* hineinplatzen kînnen */

    SetLocation(64);		/* auf ins GefÑngnis */
}

void tcDonePrison(void)
	/*
	 * wird schmutzigerweise von tcDoneGludoAsSailor,
	 * tcLastBurglary
	 * direkt angesprungen
	 */
{
    SetDay(719792L);		/* 13.01.1972? */

    sndPlaySound("end.bk", 0);	/* dont change it! (-> story_9) */
    tcMattGoesTo(7);

    gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "THE_END_PRISON");
    StopAnim();

    tcMattGoesTo(60);
    inpDelay(190);
    Say(STORY_1_TXT, 0, 155, "THE_END_MONASTERY");	/* holy matt */

    StopAnim();
    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_NEW_GAME;
}

S32 tcIsDeadlock(void)
{
    S32 deadlock = 0, total = 0;
    CompleteLoot comp = dbGetObject(CompleteLoot_LastLoot);

    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

    if (LIST_EMPTY(ObjectList)) {
	NODE *n;
	S32 money = tcGetPlayerMoney, enough = 0;

	/* jetzt zum Geld noch die vorhandene Beute addieren */
	RemoveList(tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

	total = comp->Bild + comp->Gold + comp->Geld + comp->Juwelen +
	    comp->Statue + comp->Kuriositaet + comp->HistKunst +
	    comp->GebrauchsArt + comp->Vase + comp->Delikates;

	money += total / 15;	/* im schlimmsten Fall bleibt Matt in */
	/* etwa nur ein FÅnfzehnten           */

	hasAll(Person_Marc_Smith, OLF_NORMAL, Object_Car);

	/* get cheapest car! */
	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    Car car = OL_DATA(n);

	    if (tcGetCarPrice(car) < money)
		enough = 1;
	}

	if (!enough) {
	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "DEADLOCK");

	    sndPlaySound("end.bk", 0);

	    tcMattGoesTo(60);
	    inpDelay(190);
	    Say(STORY_1_TXT, 0, 155, "THE_END_MONASTERY");	/* holy matt */

	    StopAnim();
	    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

	    deadlock = 1;
	}
    }

    return deadlock;
}

/***********************************************************************
 ***
 *** 1st BURGLARY
 ***
 ***********************************************************************/


void tcDone1stBurglary(void)
{
    AddTaxiLocation(22);	/* highgate */
    AddTaxiLocation(16);	/* anti */

    if (setup.Profidisk) {
        AddTaxiLocation(75);	/* train */
    }

    hasSet(Person_Marc_Smith, Car_Morris_Minor_1950);
    hasSet(Person_Marc_Smith, Car_Fiat_Topolino_1942);
    hasSet(Person_Marc_Smith, Car_Jeep_1945);
    hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1946);

    if (setup.Profidisk) {
        hasSet(Person_Marc_Smith, Car_Ford_Model_T__1926);
    }

    hasSet(Person_Mary_Bolton, Tool_Dietrich);
    hasSet(Person_Mary_Bolton, Tool_Bohrmaschine);
    hasSet(Person_Mary_Bolton, Tool_Strickleiter);
    hasSet(Person_Mary_Bolton, Tool_Schloszstecher);
    hasSet(Person_Mary_Bolton, Tool_Winkelschleifer);
    hasSet(Person_Mary_Bolton, Tool_Schutzanzug);

    livesInSet(London_London_1, Person_Marc_Smith);
    livesInSet(London_London_1, Person_Robert_Bull);
    livesInSet(London_London_1, Person_Thomas_Groul);
    livesInSet(London_London_1, Person_Lucas_Grull);
    livesInSet(London_London_1, Person_Peter_Brook);
    livesInSet(London_London_1, Person_Luthmilla_Nervesaw);

    if (setup.Profidisk) {
        livesInSet(London_London_1, Person_Tom_Cooler);
        livesInSet(London_London_1, Person_Tina_Olavson);
    }

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "LOBHUDEL");

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */

    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneGludoAsSailor(void)
{
    knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

    sndPlaySound("gludo.bk", 0);

    Say(STORY_0_TXT, 0, (uword) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_0");
    Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_0");
    Say(STORY_0_TXT, 0, (uword) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_1");

    if (Say(STORY_0_TXT, 0, MATT_PICTID, "SAILOR_MATT_1")) {
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "SAILOR_OLD_MATT_0");

	sndPlaySound("street1.bk", 0);
	SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
    } else {
	Say(STORY_0_TXT, 0, (uword) FACE_GLUDO_SAILOR, "SAILOR_GLUDO_2");
	StopAnim();

	tcDonePrison();
    }
}

void tcDoneCallFromBriggs(void)
{
    tcAsTimeGoesBy(GetMinute + 130);

    tcSomebodyIsCalling();

    Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");
    Say(STORY_0_TXT, 0, PHONE_PICTID, "CALL_BRIGGS_0");
    Say(STORY_0_TXT, 0, MATT_PICTID, "CALL_MATT_0");
    Say(STORY_0_TXT, 0, PHONE_PICTID, "CALL_BRIGGS_1");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 2nd BURGLARY
 ***
 ***********************************************************************/

void tcDone2ndBurglary(void)
{
    AddTaxiLocation(14);	/* jewels */

    if (setup.Profidisk) {
        AddTaxiLocation(72);	/* abbey */
    }

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "FAHNDUNG");

    hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1944);
    hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1953);

    if (setup.Profidisk) {
        hasSet(Person_Marc_Smith, Car_Rover_75_1950);
        hasSet(Person_Marc_Smith, Car_Bentley_Continental_Typ_R_1952);
    }

    hasSet(Person_Mary_Bolton, Tool_Funkgeraet);
    hasSet(Person_Mary_Bolton, Tool_Glasschneider);
    hasSet(Person_Mary_Bolton, Tool_Bohrwinde);
    hasSet(Person_Mary_Bolton, Tool_Elektroset);

    livesInSet(London_London_1, Person_Margrete_Briggs);
    livesInSet(London_London_1, Person_Paul_O_Conner);
    livesInSet(London_London_1, Person_Tony_Allen);


    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */

    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

/* wird von DoneHotel aufgerufen */
void tcCheckForBones(void)
{
    Person luthm = dbGetObject(Person_Luthmilla_Nervesaw);

    if (has(Person_Matt_Stuvysunt, Loot_Gebeine)) {
	if (knows(Person_Matt_Stuvysunt, Person_Luthmilla_Nervesaw)) {
	    Player player = dbGetObject(Player_Player_1);

	    tcSomebodyIsComing();

	    Say(STORY_0_TXT, 0, luthm->PictID, "KARL_MARX");

	    tcAddPlayerMoney(20 * player->MattsPart);

	    player->StolenMoney += 2000;
	    player->MyStolenMoney += (20 * player->MattsPart);

	    livesInUnSet(London_London_1, Person_Luthmilla_Nervesaw);
	    tcMoveAPerson(Person_Luthmilla_Nervesaw, Location_Nirvana);
	    hasUnSet(Person_Matt_Stuvysunt, Loot_Gebeine);
	}
    }
}


/***********************************************************************
 ***
 *** 3rd BURGLARY
 ***
 ***********************************************************************/

void tcDone3rdBurglary(void)
{
    AddTaxiLocation(35);	/* sotherbys */
    AddTaxiLocation(33);	/* chiswick */

    if (setup.Profidisk) {
        AddTaxiLocation(74);	/* downing */
    }

    knowsSet(Person_Matt_Stuvysunt, Person_John_Gludo);

    hasSet(Person_Mary_Bolton, Tool_Schneidbrenner);
    hasSet(Person_Mary_Bolton, Tool_Stethoskop);
    hasSet(Person_Mary_Bolton, Tool_Stromgenerator);
    hasSet(Person_Mary_Bolton, Tool_Maske);

    livesInSet(London_London_1, Person_Miguel_Garcia);
    livesInSet(London_London_1, Person_John_O_Keef);
    livesInSet(London_London_1, Person_Samuel_Rosenblatt);

    gfxShow(166, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_0");

    sndPlaySound("gludo.bk", 0);

    Say(STORY_0_TXT, 0, FACE_GLUDO_MAGIC, "READ_TIMES_GLUDO");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "READ_TIMES_1");

    sndPlaySound("street1.bk", 0);

    gfxShow(150, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */

    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcCheckForDowning(void)
{
    if (has(Person_Matt_Stuvysunt, Loot_Dokument)) {
	Building bui = (Building) dbGetObject(Building_Buckingham_Palace);

	hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);

	tcAddBuildExactlyness(bui, 255L);
	tcAddBuildStrike(bui, 5L);

	SetBubbleType(THINK_BUBBLE);
	Say(INVESTIGATIONS_TXT, 0, OLD_MATT_PICTID, "BuckinghamPlansFound");

	Present(Building_Buckingham_Palace, "Building", InitBuildingPresent);
	hasUnSet(Person_Matt_Stuvysunt, Loot_Dokument);
    }
}


/***********************************************************************
 ***
 *** 4th BURGLARY
 ***
 ***********************************************************************/

void tcDone4thBurglary(void)
{
    Person Gludo = (Person) dbGetObject(Person_John_Gludo);

    AddTaxiLocation(31);	/* osterly */
    AddTaxiLocation(29);	/* ham */

    if (setup.Profidisk) {
        AddTaxiLocation(70);	/* madame */
    }

    tcSomebodyIsComing();

    sndPlaySound("gludo.bk", 0);
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_0");
    Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_0");
    Say(STORY_0_TXT, 0, MATT_PICTID, "ARREST_MATT_0");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "ARREST_OLD_MATT_1");
    Say(STORY_0_TXT, 0, Gludo->PictID, "ARREST_GLUDO_1");

    hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1950);
    hasSet(Person_Marc_Smith, Car_Cadillac_Club_1952);

    if (setup.Profidisk) {
        hasSet(Person_Marc_Smith, Car_Fiat_634_N_1943);
    }

    hasSet(Person_Mary_Bolton, Tool_Dynamit);
    hasSet(Person_Mary_Bolton, Tool_Kernbohrer);
    hasSet(Person_Mary_Bolton, Tool_Sauerstofflanze);
    hasSet(Person_Mary_Bolton, Tool_Chloroform);

    livesInSet(London_London_1, Person_Garry_Stevenson);
    livesInSet(London_London_1, Person_Jiri_Poulin);
    livesInSet(London_London_1, Person_Prof_Emil_Schmitt);

    if (setup.Profidisk) {
        livesInSet(London_London_1, Person_Melanie_Morgan);
        livesInSet(London_London_1, Person_Sid_Palmer);
    }

    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(7)->EventNr;	/* Polizei */
}

void tcDoneMattIsArrested(void)
{
    StopAnim();

    gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* prison */

    AddVTime(1439);

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "IN_PRISON_OLD_0");
    gfxShow(161, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* polizei */

    SceneArgs.ReturnValue = SCENE_POLICE;
}

/***********************************************************************
 ***
 *** 5th BURGLARY
 ***
 ***********************************************************************/

void tcDone5thBurglary(void)
{
    AddTaxiLocation(27);	/* kenw */
    AddTaxiLocation(39);	/* natural museum */

    if (setup.Profidisk) {
        AddTaxiLocation(77);	/* tate */
    }

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_0");

    hasSet(Person_Marc_Smith, Car_Standard_Vanguard_1951);

    if (setup.Profidisk) {
        hasSet(Person_Marc_Smith, Car_Rover_75_1952);
        hasSet(Person_Marc_Smith, Car_Bentley_Continental_Typ_R_1953);
    }

    hasSet(Person_Mary_Bolton, Tool_Schuhe);
    hasSet(Person_Mary_Bolton, Tool_Elektrohammer);

    livesInSet(London_London_1, Person_Thomas_Smith);
    livesInSet(London_London_1, Person_Albert_Liet);
    livesInSet(London_London_1, Person_Frank_Meier);
    livesInSet(London_London_1, Person_Mike_Kahn);
    livesInSet(London_London_1, Person_Mark_Hart);
    livesInSet(London_London_1, Person_Frank_De_Silva);
    livesInSet(London_London_1, Person_Neil_Grey);
    livesInSet(London_London_1, Person_Serge_Fontane);
    livesInSet(London_London_1, Person_Mike_Seeger);
    livesInSet(London_London_1, Person_Mathew_Black);

    if (setup.Profidisk) {
        livesInSet(London_London_1, Person_Prof_Marcus_Green);
        livesInSet(London_London_1, Person_Pere_Ubu);
    }

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneDealerIsAfraid(void)
{
    U32 persID;
    Person pers;

    switch (GetLocation) {
    case 52:
	persID = Person_Helen_Parker;
	SceneArgs.ReturnValue = SCENE_WATLING;
	break;
    case 53:
	persID = Person_Frank_Maloya;
	SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
	break;
    case 54:
	persID = Person_Eric_Pooly;
	SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
	break;
    default:
	return;
    }

    knowsSet(Person_Matt_Stuvysunt, persID);

    pers = (Person) dbGetObject(persID);
    Say(STORY_0_TXT, 0, pers->PictID, "DEALER_IS_AFRAID");

    gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);
}

void tcDoneRaidInWalrus(void)
{
    Person Red = (Person) dbGetObject(Person_Red_Stanson);
    Environment Env = (Environment) dbGetObject(Environment_TheClou);

    knowsSet(Person_Matt_Stuvysunt, Person_Red_Stanson);
    sndPlaySound("gludo.bk", 0);

    Say(STORY_0_TXT, 0, Red->PictID, "RAID_POLICE_0");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_0");

    Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_0");
    Say(STORY_0_TXT, 0, Red->PictID, "RAID_POLICE_1");

    if (Env->MattHasIdentityCard) {
	Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_1");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_1");

	SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
    } else {
	Say(STORY_0_TXT, 0, MATT_PICTID, "RAID_MATT_2");
	Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "RAID_OLD_MATT_2");

	SceneArgs.ReturnValue = SCENE_POLICE;
    }

    StopAnim();
    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
}

void tcDoneDartJager(void)
{
    ubyte choice;
    Person Grull = (Person) dbGetObject(Person_Lucas_Grull);
    Environment Env = (Environment) dbGetObject(Environment_TheClou);

    if (!Env->MattHasIdentityCard) {
	StopAnim();

	gfxShow(169, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Knast */

	knowsSet(Person_Matt_Stuvysunt, Person_Lucas_Grull);

	Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_0");
	choice = Say(STORY_0_TXT, 0, MATT_PICTID, "DART_MATT_1");

	if (choice == 0) {
	    Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_1");

	    sndPrepareFX("darth.voc");
	    sndPlayFX();

	    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_0");

	    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
	    gfxShow(221, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP, 0, -1,
		    -1);

	    sndPrepareFX("darth.voc");
	    sndPlayFX();

	    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_JAEGER_1");

	    sndPlaySound("end.bk", 0);
	    tcMattGoesTo(60);
	    Say(STORY_0_TXT, 0, 155, "THE_END_MONASTERY");	/* pict = holy matt */

	    StopAnim();
	    gfxChangeColors(l_gc, 3L, GFX_FADE_OUT, 0L);

	    SceneArgs.ReturnValue = SCENE_NEW_GAME;
	} else {
	    Say(STORY_0_TXT, 0, Grull->PictID, "DART_GRULL_2");
	    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "DART_OLD_MATT_0");

	    gfxShow(161, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Polizei - refresh */

	    SceneArgs.ReturnValue = SCENE_POLICE;
	}
    } else
	SceneArgs.ReturnValue = SCENE_POLICE;
}

void tcDoneGludoBurnsOffice(void)
{
    Person Gludo = (Person) dbGetObject(Person_John_Gludo);

    StopAnim();

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_1");
    Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_0");
    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_0");

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_2");
    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_1");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_3");

    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_2");

    sndPrepareFX("brille.voc");
    gfxShow(162, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);
    sndPlayFX();

    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_4");
    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_3");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_5");

    Say(STORY_0_TXT, 0, MATT_PICTID, "5TH_MATT_1");
    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_4");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_6");

    sndPrepareFX("streich.voc");
    sndPlayFX();

    gfxShow(153, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

    Say(STORY_0_TXT, 0, Gludo->PictID, "5TH_GLUDO_5");
    Say(STORY_0_TXT, 0, OLD_MATT_PICTID, "5TH_OLD_7");

    gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_WATLING;
}

void tcDoneBeautifullMorning(void)
{
    tcAsTimeGoesBy(GetMinute + 187);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "MORNING_MATT_0");

    PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
    tcAsTimeGoesBy(546);
    StopAnim();

    gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Hotel */

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "MORNING_MATT_1");

    AddTaxiLocation(61);

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

void tcDoneVisitingSabien(void)
{
    Person Sabien = dbGetObject(Person_Sabien_Pardo);

    knowsSet(Person_Matt_Stuvysunt, Person_Sabien_Pardo);

    Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_0");
    Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "GROVE_OLD_MATT_0");
    Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_1");
    Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_1");
    Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_2");
    Say(STORY_1_TXT, 0, MATT_PICTID, "GROVE_MATT_2");
    Say(STORY_1_TXT, 0, Sabien->PictID, "GROVE_SABIEN_3");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "GROVE_OLD_MATT_1");

    gfxChangeColors(l_gc, 5, GFX_FADE_OUT, 0);
    SceneArgs.ReturnValue = SCENE_LISSON_GROVE;
}

void tcDoneADream(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST1_OLD_0");

    PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
    tcAsTimeGoesBy(517);
    StopAnim();

    gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Hotel */

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST1_OLD_1");

    tcSomebodyIsCalling();

    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_0");
    Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_0");
    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_1");
    Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_1");
    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_2");
    Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_2");
    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_3");
    Say(STORY_1_TXT, 0, MATT_PICTID, "ST1_MATT_3");
    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST1_BRIGGS_4");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

void tcMeetingRosenblatt(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_2_OLD_0");

    SceneArgs.ReturnValue = SCENE_STATION;
}

void tcBriggsAngry(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_2_NOT_OLD_0");

    SceneArgs.ReturnValue = SCENE_HOTEL_REC;
}

void tcSabienInWalrus(void)
{
    Person Sabien = dbGetObject(Person_Sabien_Pardo);

    sndPlaySound("sabien.bk", 0);
    StopAnim();

    Say(STORY_1_TXT, 0, Sabien->PictID, "ST_3_SABIEN_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_3_OLD_0");
    gfxShow(157, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Kiss */

    inpSetWaitTicks(200);
    inpWaitFor(INP_LBUTTONP | INP_TIME);
    inpSetWaitTicks(0);

    gfxShow(141, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Walrus */

    SceneArgs.ReturnValue = SCENE_WALRUS;
}

void tcWalrusTombola(void)
{
    Environment Env = dbGetObject(Environment_TheClou);

    Env->Present = 1;
    sndPlaySound("sabien.bk", 0);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_5_OLD_0");

    SceneArgs.ReturnValue = SCENE_WALRUS;
}

void tcRainyEvening(void)
{
    Person Briggs = dbGetObject(Person_Herbert_Briggs);

    tcAsTimeGoesBy(1220);

    Say(STORY_1_TXT, 0, MATT_PICTID, "ST_8_MATT_0");
    Say(STORY_1_TXT, 0, LETTER_PICTID, "ST_8_SABIEN_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_8_OLD_0");

    PlayAnim("Sleep", 30000, GFX_DONT_SHOW_FIRST_PIC);
    tcAsTimeGoesBy(424);
    StopAnim();

    gfxShow(173, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Hotel */

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_9_OLD_0");
    Say(STORY_1_TXT, 0, Briggs->PictID, "ST_9_BRIGGS_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_9_OLD_1");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

void tcDoneMissedDate(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "VERPASST_BRIEF");
    Say(STORY_1_TXT, 0, LETTER_PICTID, "DATE_VERPASST");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 6th BURGLARY
 ***
 ***********************************************************************/

void tcDone6thBurglary(void)
{
    AddTaxiLocation(43);	/* vict & alb */
    AddTaxiLocation(37);	/* brit */

    if (setup.Profidisk) {
        AddTaxiLocation(79);	/* buckingham */
    }

    hasSet(Person_Marc_Smith, Car_Pontiac_Streamliner_1949);
    hasSet(Person_Marc_Smith, Car_Triumph_Roadstar_1949);

    livesInSet(London_London_1, Person_Kevin_Smith);
    livesInSet(London_London_1, Person_Al_Mel);

    if (setup.Profidisk) {
        livesInSet(London_London_1, Person_Phil_Ciggy);
        livesInSet(London_London_1, Person_Rod_Masterson);
    }

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcPoliceInfoTower(void)
{
    Building tower = dbGetObject(Building_Tower_of_London);
    Environment Env = dbGetObject(Environment_TheClou);

    if (Env->Present) {
	Say(STORY_1_TXT, 0, 223, "ST_7_OLD_0");	/* Matt mit Bart */
	tower->Exactlyness = 175;

	Present(Building_Tower_of_London, "Building", InitBuildingPresent);

	SceneArgs.ReturnValue = SCENE_WATLING;

	StopAnim();
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
    } else
	SceneArgs.ReturnValue = SCENE_POLICE;
}

void tcPresentInHotel(void)
{
    Person Ben = dbGetObject(Person_Ben_Riggley);
    Environment Env = dbGetObject(Environment_TheClou);

    Env->Present = 1;

    Say(STORY_1_TXT, 0, Ben->PictID, "ST_6_BEN_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_6_OLD_0");

    SceneArgs.ReturnValue = SCENE_HOTEL_REC;
}

void tcSabienDinner(void)
{
    tcAsTimeGoesBy(1210);
    sndPlaySound("sabien.bk", 0);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_0");
    tcMattGoesTo(62);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_1");
    tcAsTimeGoesBy(GetMinute + 160);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_4_OLD_2");

    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
    SceneArgs.ReturnValue = SCENE_WALRUS;
}


/***********************************************************************
 ***
 *** 7th BURGLARY
 ***
 ***********************************************************************/


void tcDone7thBurglary(void)
{
    AddTaxiLocation(41);	/* national */
    AddTaxiLocation(45);	/* bank */

    if (setup.Profidisk) {
        AddTaxiLocation(81);	/* bulstrode  */
    }

    /* Jaguar kommt hier, da: 1. man barucht ihn fÅr Villa, */
    /* 2. Jaguar wird in Fahndung nie erwischt (sonst kînnte er nicht explodieren) */
    /* 3. man soll Jaguar nicht zu lange haben kînnen (da man nie erwischt wird  */

    hasSet(Person_Marc_Smith, Car_Jaguar_XK_1950);

    livesInSet(London_London_1, Person_Mohammed_Abdula);

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneBirthday(void)
{
    LIST *persons;
    struct ObjectNode *n;

    StopAnim();
    gfxShow(172, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

    sndPrepareFX("birthd2.voc");	/* applause */
    sndPlayFX();

    knowsAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
    persons = ObjectListPrivate;

    for (n = (struct ObjectNode *) LIST_HEAD(persons); NODE_SUCC(n);
	 n = (struct ObjectNode *) NODE_SUCC(n)) {
	Person p = dbGetObject(OL_NR(n));

	switch (OL_NR(n)) {
	case Person_Sabien_Pardo:
	case Person_Herbert_Briggs:
	case Person_John_Gludo:
	case Person_Miles_Chickenwing:
	    /*case Person_Old_Matt:*/
	case Person_Red_Stanson:
	    break;
	default:
	    if (livesIn(London_London_1, OL_NR(n)) && (CalcRandomNr(0, 10) < 7)) {
		tcMoveAPerson(OL_NR(n), Location_Walrus);

		Say(STORY_1_TXT, 0, p->PictID, "ST_11_ALL_0");
	    }
	    break;
	}
    }

    sndPrepareFX("birthd1.voc");	/* cork popping */
    sndPlayFX();

    RemoveList(persons);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_11_OLD_0");
    gfxShow(141, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);

    SceneArgs.ReturnValue = SCENE_WALRUS;
}

void tcWalkWithSabien(void)
{
    Environment Env = dbGetObject(Environment_TheClou);

    sndPlaySound("sabien.bk", 0);

    gfxShow(165, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_13_OLD_0");

    gfxShow(158, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);	/* Sabien outside! */

    Env->MattIsInLove = 1;

    SceneArgs.ReturnValue = SCENE_LISSON_GROVE;
}

void tcDoneSabienCall(void)
{
    tcSomebodyIsCalling();

    Say(STORY_0_TXT, 0, PHONE_PICTID, "A_CALL_FOR_YOU");
    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_10_SABIEN");

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

void tcDoneMeetingAgain(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_10_OLD_0");

    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_LISSON_GROVE;
}

/***********************************************************************
 ***
 *** 8th BURGLARY
 ***
 ***********************************************************************/


void tcDone8thBurglary(void)
{
    AddTaxiLocation(25);	/* villa */

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneAgent(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_14_OLD_0");

    tcSomebodyIsCalling();

    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_14_AGENT_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_14_OLD_1");

    tcAddPlayerMoney(15000);

    SceneArgs.ReturnValue = SCENE_HOTEL_ROOM;
}

/***********************************************************************
 ***
 *** 9th BURGLARY
 ***
 ***********************************************************************/

void tcDone9thBurglary(void)
{
    Environment Env = dbGetObject(Environment_TheClou);

    SetMinute(540);

    if (has(Person_Matt_Stuvysunt, Loot_Koffer)) {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_0");
	gfxShow(174, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);	/* cracks */
    } else {
	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_NOT_0");
	SetBubbleType(THINK_BUBBLE);
	Say(STORY_1_TXT, 0, MATT_PICTID, "ST_15_MATT_NOT_0");

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_NOT_1");
	gfxShow(174, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);	/* cracks */
    }

    inpDelay(150);
    gfxShow(173, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);	/* hotel */

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_15_OLD_1");

    tcSomebodyIsCalling();

    Say(STORY_1_TXT, 0, PHONE_PICTID, "ST_15_ALLEN_0");

    /* keine Polizisten sind mehr unterwegs */
    livesInUnSet(London_London_1, Person_John_Gludo);
    livesInUnSet(London_London_1, Person_Miles_Chickenwing);
    livesInUnSet(London_London_1, Person_Red_Stanson);

    tcMoveAPerson(Person_John_Gludo, Location_Nirvana);
    tcMoveAPerson(Person_Miles_Chickenwing, Location_Nirvana);
    tcMoveAPerson(Person_Red_Stanson, Location_Nirvana);

    /* nur mehr cars&vans bzw. Lisson Grove zu erreichen */
    RemRelation(Relation_taxi);
    AddRelation(Relation_taxi);

    AddTaxiLocation(1);		/* cars */

    if (Env->MattIsInLove)
	AddTaxiLocation(61);	/* lisson */

    /* nur mehr Mîglichkeiten: gehen */
    SetEnabledChoices(GO | WAIT);

    GetScene(SCENE_FAHNDUNG)->Geschehen = 0;	/* damit nicht gleich Burglary 2 geschieht */
    SceneArgs.ReturnValue = GetLocScene(GetLocation)->EventNr;
}

void tcDoneGoAndFetchJaguar(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_16_OLD_0");

    SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
}

void tcDoneThinkOfSabien(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_16_OLD_1");

    SceneArgs.ReturnValue = SCENE_HOLLAND_STR;
}

void tcDoneTerror(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_17_OLD_0");
/* XXX
	gfxPrepareColl (211);
	gfxPrepareColl (210);
	gfxPrepareColl (209);
*/
    gfxShow(176, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
    inpDelay(150);

    sndPrepareFX("explosio.voc");
    sndPlayFX();

    PlayAnim("Explo1", 1, GFX_DONT_SHOW_FIRST_PIC);
    inpDelay(200);

    StopAnim();

    PlayAnim("Explo2", 50, GFX_DONT_SHOW_FIRST_PIC);
    inpDelay(260);

    StopAnim();

    hasUnSet(Person_Matt_Stuvysunt, Car_Jaguar_XK_1950);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_17_OLD_1");
    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_CARS_VANS;
}

void tcDoneConfessingSabien(void)
{
    Person Sabien = dbGetObject(Person_Sabien_Pardo);
    Environment Env = dbGetObject(Environment_TheClou);

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_18_OLD_0");
    Say(STORY_1_TXT, 0, Sabien->PictID, "ST_18_SABIEN_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_18_OLD_1");
    Say(STORY_1_TXT, 0, Sabien->PictID, "ST_18_SABIEN_1");

    Env->WithOrWithoutYou = Say(STORY_1_TXT, 0, MATT_PICTID, "ST_18_MATT_0");

    StopAnim();

    if (Env->WithOrWithoutYou) {	/* bleibt bei Sabien! */
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
	ShowMenuBackground();

	gfxShow(163, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP, 0, -1, -1);	/* south 1 */
	gfxShow(152, GFX_NO_REFRESH | GFX_OVERLAY, 3, -1, -1);	/* family */

	sndPlaySound("sabien.bk", 0);

	Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_19_OLD_0");

	/* hier eventuell glÅckliche Anim zeigen! */
	gfxShow(164, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);	/*  The End */

	inpWaitFor(INP_LBUTTONP);
	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	SceneArgs.ReturnValue = SCENE_NEW_GAME;
    } else {			/* bleibt nicht bei Sabien! */

	AddVTime(2713);		/* hier nicht ausblenden? */

	gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

	SceneArgs.ReturnValue = SCENE_SOUTHHAMPTON;

	SetLocation(-1);	/* damit sicher eingeblendet wird! */
    }
}

void tcDoneSouthhamptonWithoutSabien(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_20_OLD_0");

    SceneArgs.ReturnValue = SCENE_SOUTHHAMPTON;
}

void tcDoneSouthhamptonSabienUnknown(void)
{
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_20W_OLD_0");

    StopAnim();
    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

    SceneArgs.ReturnValue = SCENE_SOUTHHAMPTON;
}

static void tcDoneFirstTimeLonelyInSouth(void)
{
    LIST *menu = txtGoKey(MENU_TXT, "SouthhamptonMenu");
    U32 startTime = 0, actionTime;
    ubyte ende = 0, activ = 1;
    Environment Env = dbGetObject(Environment_TheClou);
    Person Herb = dbGetObject(Person_Herbert_Briggs);

    tcAsDaysGoBy(713518L, 30);
    startTime = GetDay * 1440 + GetMinute;

    while (!ende) {
	inpTurnFunctionKey(0);
	inpTurnESC(0);
	activ = Menu(menu, 46, activ, NULL, 0L);
	inpTurnESC(1);
	inpTurnFunctionKey(1);

	actionTime = CalcRandomNr(180, 300);	/* 2 bis 5 Stunden */

	switch (activ) {
	case 1:
	    tcAsTimeGoesBy(GetMinute + actionTime);
	    break;		/* spazieren */
	case 2:
	    tcAsTimeGoesBy(GetMinute + actionTime);
	    break;		/* warten  */
	case 3:
	    tcAsTimeGoesBy(GetMinute + actionTime);
	    break;		/* fischen */
	case 5:
	    Information();
	    ShowTime(0);
	    break;
	default:
	    break;
	}

	if (((GetDay * 1440 + GetMinute) > (startTime + 420)) && (!(Env->SouthhamptonHappened & 1))) {	/* nach 7 Stunden */
	    Env->SouthhamptonHappened |= 1;

	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_21_OLD");	/* Kummer */
	}

	if (((GetDay * 1440 + GetMinute) > (startTime + 1020)) && (!(Env->SouthhamptonHappened & 2))) {	/* nach 17 Stunden */
	    Env->SouthhamptonHappened |= 2;

	    SetBubbleType(THINK_BUBBLE);

	    if (Env->MattIsInLove)
		Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22_MATT");	/* Neugier */
	    else
		Say(STORY_1_TXT, 0, MATT_PICTID, "ST_22O_MATT");
	}

	if (((GetDay * 1440 + GetMinute) > (startTime + 1500)) && (!(Env->SouthhamptonHappened & 4))) {	/* nach 25 Stunden */
	    Env->SouthhamptonHappened |= 4;

	    if (Env->MattIsInLove)
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23_MATT");	/* Neugier */
	    else
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_23O_MATT");
	}

	if (((GetDay * 1440 + GetMinute) > (startTime + 2100))
	    && (!(Env->SouthhamptonHappened & 8))) {
	    Env->SouthhamptonHappened |= 8;

	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_0");	/* The Return */
	    Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_0");
	    Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_0");
	    Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_1");
	    Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_1");
	    Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_2");

	    if (Env->MattIsInLove) {
		Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_2");
		Say(STORY_1_TXT, 0, Herb->PictID, "ST_24_BRIGGS_3");
	    }

	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_1");
	    Say(STORY_1_TXT, 0, MATT_PICTID, "ST_24_MATT_3");
	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_2");

	    if (Env->MattIsInLove)
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24_OLD_3");
	    else
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_24O_OLD_3");

	    hasSet(Person_Matt_Stuvysunt, Building_Tower_of_London);

	    ende = 1;
	}

	ShowTime(0);
    }

    RemoveList(menu);
}

void tcDoneSouthhampton(void)
{
    LIST *menu = txtGoKey(MENU_TXT, "SouthhamptonMenu");
    ubyte activ = 1;		/* !! */
    U32 actionTime;
    Environment Env = dbGetObject(Environment_TheClou);

    SceneArgs.Ueberschrieben = 1;
    SceneArgs.ReturnValue = 0;	/* MU· SEIN! */

    SetEnabledChoices(GP_ALL_CHOICES_ENABLED);

    AddVTime(CalcRandomNr(560, 830));
    ShowTime(0);

    if (Env->FirstTimeInSouth) {
	Env->FirstTimeInSouth = 0;
	tcDoneFirstTimeLonelyInSouth();
	tcInitTowerBurglary();	/* wird absichtlich X mal aufgerufen - siehe weiter unten! */
    }

    /* mit Gehen oder Planen kommt man aus dem MenÅ raus */
    while ((activ != 0) && (SceneArgs.ReturnValue == 0)) {
	inpTurnESC(0);
	inpTurnFunctionKey(1);
	activ = Menu(menu, 127, activ, NULL, 0L);
	inpTurnFunctionKey(0);
	inpTurnESC(1);

	if (activ == (ubyte) - 1) {
	    ShowTheClouRequester(No_Error);
	    SceneArgs.ReturnValue =
		((Player) dbGetObject(Player_Player_1))->CurrScene;

	    activ = 1;		/* nicht 0! */
	} else {
	    actionTime = CalcRandomNr(180, 300);	/* 2 bis 5 Stunden */

	    switch (activ) {
	    case 0:
		break;

	    case 1:
		tcAsTimeGoesBy(GetMinute + actionTime);
		break;		/* spazieren */
	    case 2:
		tcAsTimeGoesBy(GetMinute + actionTime);
		break;		/* warten  */
	    case 3:
		tcAsTimeGoesBy(GetMinute + actionTime);
		break;		/* fischen */
	    case 4:
		sndPlaySound("hotel.bk", 0);
		tcInitTowerBurglary();	/* just to be sure */
		plPlaner(Building_Tower_of_London);
		RefreshCurrScene();
		break;
	    case 5:
		Information();
		ShowTime(0);
		inpTurnFunctionKey(1);
		break;
	    case 6:
		if (tcDoTowerBurglary())
		    SceneArgs.ReturnValue = SCENE_KASERNE_OUTSIDE;
		else
		    RefreshCurrScene();
		break;
		/* Vorsicht bei Erweiterung -> Speichern weiter oben */
	    default:
		break;
	    }

	    ShowTime(0);
	}
    }

    if (!activ) {
	AddVTime(CalcRandomNr(560, 830));
	SceneArgs.ReturnValue = SCENE_TOWER_OUT;
    }

    StopAnim();
    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);

    RemoveList(menu);
}

void tcInitTowerBurglary(void)
{
    NODE *node;
    Car car = dbGetObject(Car_Cadillac_Club_1952);
    Player player = dbGetObject(Player_Player_1);

    /* alle Personen entfernen! */
    player->MattsPart = 25;

    joined_byAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Person);

    for (node = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node))
	joined_byUnSet(Person_Matt_Stuvysunt, OL_NR(node));

    /* und Personen, Abilities neu setzen! */
    joined_bySet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
    joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
    joined_bySet(Person_Matt_Stuvysunt, Person_Marc_Smith);
    joined_bySet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

    joinSet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
    joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
    joinSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
    joinSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

    knowsSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
    knowsSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

    /* all tools */
    hasSet(Person_Matt_Stuvysunt, Tool_Dietrich);
    hasSet(Person_Matt_Stuvysunt, Tool_Hammer);
    hasSet(Person_Matt_Stuvysunt, Tool_Axt);
    hasSet(Person_Matt_Stuvysunt, Tool_Kernbohrer);
    hasSet(Person_Matt_Stuvysunt, Tool_Handschuhe);
    hasSet(Person_Matt_Stuvysunt, Tool_Schuhe);
    hasSet(Person_Matt_Stuvysunt, Tool_Maske);
    hasSet(Person_Matt_Stuvysunt, Tool_Chloroform);
    hasSet(Person_Matt_Stuvysunt, Tool_Brecheisen);
    hasSet(Person_Matt_Stuvysunt, Tool_Winkelschleifer);
    hasSet(Person_Matt_Stuvysunt, Tool_Bohrmaschine);
    hasSet(Person_Matt_Stuvysunt, Tool_Bohrwinde);
    hasSet(Person_Matt_Stuvysunt, Tool_Schloszstecher);
    hasSet(Person_Matt_Stuvysunt, Tool_Schneidbrenner);
    hasSet(Person_Matt_Stuvysunt, Tool_Sauerstofflanze);
    hasSet(Person_Matt_Stuvysunt, Tool_Stethoskop);
    hasSet(Person_Matt_Stuvysunt, Tool_Batterie);
    hasSet(Person_Matt_Stuvysunt, Tool_Stromgenerator);
    hasSet(Person_Matt_Stuvysunt, Tool_Elektroset);
    hasSet(Person_Matt_Stuvysunt, Tool_Schutzanzug);
    hasSet(Person_Matt_Stuvysunt, Tool_Dynamit);
    hasSet(Person_Matt_Stuvysunt, Tool_Elektrohammer);
    hasSet(Person_Matt_Stuvysunt, Tool_Glasschneider);
    hasSet(Person_Matt_Stuvysunt, Tool_Strickleiter);
    hasSet(Person_Matt_Stuvysunt, Tool_Funkgeraet);

    if (hasGet(Person_Matt_Stuvysunt, Ability_Schloesser) < 210)
	hasSetP(Person_Matt_Stuvysunt, Ability_Schloesser, 210);

    if (hasGet(Person_Matt_Stuvysunt, Ability_Elektronik) < 251)
	hasSetP(Person_Matt_Stuvysunt, Ability_Elektronik, 251);

    hasSetP(Person_Marc_Smith, Ability_Autos, 255);
    hasSetP(Person_Mohammed_Abdula, Ability_Kampf, 255);

    hasSet(Person_Matt_Stuvysunt, Car_Cadillac_Club_1952);

    /* Auto */
    car->State = 255;
    car->MotorState = 255;
    car->BodyWorkState = 180;
    car->TyreState = 255;
    car->Strike = 80;		/* extrem niedrig! (fÅr diesen Wagen) */

    /* Organisatorisches! */
    Organisation.CarID = Car_Cadillac_Club_1952;
    Organisation.DriverID = Person_Marc_Smith;
    Organisation.BuildingID = Building_Tower_of_London;

    Organisation.GuyCount = 4;
}

S32 tcDoTowerBurglary(void)
{
    S32 burglary;

    /* das Organisatorische mu· hier wiederholt werden, da */
    /* es sonst zu Fehler kommen kann, da die Organisation */
    /* nicht abgespeichert wird!                           */
    tcInitTowerBurglary();

    /* und los gehts! */
    burglary = plPlayer(Building_Tower_of_London, 0, NULL);

    if (burglary) {
	if (has(Person_Matt_Stuvysunt, Loot_Kronjuwelen))
	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_0");
	else
	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25O_OLD_0");

	tcDoneMafia();
    }

    return burglary;
}

void tcDoneMafia(void)
{
    Person Ken = dbGetObject(Person_Ken_Addison);
    Person Briggs = dbGetObject(Person_Herbert_Briggs);

    CurrentBackground = BGD_LONDON;
    ShowMenuBackground();

    gfxShow(168, GFX_NO_REFRESH | GFX_ONE_STEP | GFX_CLEAR_FIRST | GFX_FADE_OUT | GFX_BLEND_UP, 3, -1, -1);	/* Mafia */

    Say(STORY_1_TXT, 0, Ken->PictID, "ST_25_PATE_0");

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_1");
    Say(STORY_1_TXT, 0, Briggs->PictID, "ST_25_BRIGGS_0");

    Say(STORY_1_TXT, 0, MATT_PICTID, "ST_25_MATT_0");
    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_25_OLD_2");

    sndPlaySound("street1.bk", 0);

    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
    tcAsTimeGoesBy(GetMinute + 420);

    tcMattGoesTo(25);		/* Villa */

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_0");
    Say(STORY_1_TXT, 0, Ken->PictID, "ST_26_PATE_0");
    Say(STORY_1_TXT, 0, Ken->PictID, "KEN_PLAN");

    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_26_OLD_1");
}

void tcDoneKaserne(void)
{
    Environment Env = dbGetObject(Environment_TheClou);
    Car car = dbGetObject(Car_Cadillac_Club_1952);
    LIST *menu = txtGoKey(MENU_TXT, "KaserneMenu");
    ubyte activ = 0, burglary = 0;
    U32 successor = 0;

    joined_bySet(Person_Matt_Stuvysunt, Person_Matt_Stuvysunt);
    joined_bySet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
    joined_bySet(Person_Matt_Stuvysunt, Person_Marc_Smith);
    joined_bySet(Person_Matt_Stuvysunt, Person_Ken_Addison);

    joined_byUnSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);

    joinSet(Person_Matt_Stuvysunt, Person_Ken_Addison);
    knowsSet(Person_Matt_Stuvysunt, Person_Ken_Addison);

    ((Building) dbGetObject(Building_Starford_Kaserne))->Exactlyness = 255;

    hasSet(Person_Matt_Stuvysunt, Building_Starford_Kaserne);

    /* Auto */
    car->State = 255;
    car->MotorState = 255;
    car->BodyWorkState = 180;
    car->TyreState = 255;
    car->Strike = 80;		/* extrem niedrig! (fÅr diesen Wagen) */

    /* Organisatorisches! */
    Organisation.CarID = Car_Cadillac_Club_1952;
    Organisation.DriverID = Person_Marc_Smith;

    Organisation.GuyCount = 4;

    /* und los gehts! */
    while ((!burglary) && (!successor)) {
	inpTurnESC(0);
	activ = Menu(menu, 15, activ, NULL, 0L);
	inpTurnESC(1);

	/* change possibilites in PatchStory too! */
	if (activ == (ubyte) - 1) {
	    ShowTheClouRequester(No_Error);
	    successor = ((Player) dbGetObject(Player_Player_1))->CurrScene;

	    activ = 1;
	} else {
	    switch (activ) {
	    case 0:
		StopAnim();
		gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
		SetLocation(65);
		SceneArgs.Moeglichkeiten = 265;
		DoneInsideHouse();
		tcMattGoesTo(66);
		break;
	    case 1:
		Information();
		break;
	    case 2:
		StopAnim();
		plPlaner(Building_Starford_Kaserne);
		tcMattGoesTo(66);	/* refresh! */
		break;
	    case 3:
		StopAnim();
		Search.KaserneOk = 0;
		if (!(burglary = plPlayer(Building_Starford_Kaserne, 0, NULL)))
		    tcMattGoesTo(66);	/* refresh! */
		break;
	    default:
		break;
	    }
	}
    }

    StopAnim();

    if (burglary) {
	if (Search.KaserneOk) {
	    sndPlaySound("end.bk", 0);

	    if (Env->MattIsInLove)
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_27_OLD_0");
	    else
		Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_0");

	    gfxShow(224,
		    GFX_NO_REFRESH | GFX_ONE_STEP | GFX_FADE_OUT | GFX_BLEND_UP,
		    3, -1, -1);
	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_29_OLD_0");
	} else {
	    /* sndPlaySound() in tcDonePrison() */

	    Say(STORY_1_TXT, 0, OLD_MATT_PICTID, "ST_28_OLD_1");
	    tcDonePrison();
	}

	successor = SCENE_NEW_GAME;
    }

    gfxChangeColors(l_gc, 3, GFX_FADE_OUT, 0);
    RemoveList(menu);

    SceneArgs.ReturnValue = successor;
}

S32 tcIsLastBurglaryOk(void)
{
    Building kaserne = dbGetObject(Building_Starford_Kaserne);
    word carXPos0 = kaserne->CarXPos - 40;
    word carYPos0 = kaserne->CarYPos - 40;
    word carXPos1 = kaserne->CarXPos + 40;
    word carYPos1 = kaserne->CarYPos + 40;
    S32 madeIt = 1, i;
    LSObject left = dbGetObject(tcLAST_BURGLARY_LEFT_CTRL_OBJ);
    LSObject right = dbGetObject(tcLAST_BURGLARY_RIGHT_CTRL_OBJ);

    /* Links mu· ein, Rechts mu· ausgeschalten sein */
    /* 1.. OFF!    */

    if ((left->ul_Status & (1 << Const_tcON_OFF)))
	madeIt = 0;

    if (!(right->ul_Status & (1 << Const_tcON_OFF)))
	madeIt = 0;

    if (!has(Person_Matt_Stuvysunt, Loot_Dokument))
	madeIt = 0;

    /* alle anderen mÅssen beim Auto sein... */
    for (i = 1; i < 4; i++)
	if ((Search.GuyXPos[i] < carXPos0) || (Search.GuyXPos[i] > carXPos1) ||
	    (Search.GuyYPos[i] < carYPos0) || (Search.GuyYPos[i] > carYPos1))
	    madeIt = 0;

    if ((Search.GuyXPos[0] >= carXPos0) && (Search.GuyXPos[0] <= carXPos1) &&
	(Search.GuyYPos[0] >= carYPos0) && (Search.GuyYPos[i] <= carYPos1))
	madeIt = 0;

    return madeIt;
}

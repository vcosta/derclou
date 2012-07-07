/*
**	$Filename: gameplay/gp_app.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     08-04-94
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

#include "gameplay/gp_app.h"

#include <assert.h>

void tcAsTimeGoesBy(U32 untilMinute)
{
    untilMinute = untilMinute % 1440;

    while (GetMinute != untilMinute) {
	inpDelay(1);

	AddVTime(1);

	if (!(GetMinute % 60))
	    ShowTime(0);
    }
}

void tcAsDaysGoBy(U32 day, U32 stepSize)
{
    U32 add;

    while (GetDay < day) {
	inpDelay(3);

	add = CalcRandomNr(stepSize - stepSize / 30, stepSize + stepSize / 30);

	SetDay(GetDay + add);

	tcRefreshLocationInTitle(GetLocation);
    }
}

void tcMattGoesTo(U32 locNr)
{
    NODE *node = GetNthNode(film->loc_names, locNr);

    SetLocation(locNr);
    tcRefreshLocationInTitle(locNr);
    ShowTime(0);

    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
    PlayAnim(NODE_NAME(node), (word) 30000,
	     GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);
}

void tcTheAlmighty(U32 time)
{
    tcMovePersons(2, time);
}

void tcMovePersons(U32 personCount, U32 time)
{
    U32 i, count;
    U32 persID, locID;

    for (i = 0; i < personCount; i++) {

        if (setup.Profidisk) {
	    persID = CalcRandomNr(Person_Paul_O_Conner, Person_Pere_Ubu + 1);
        } else {
    	    persID = CalcRandomNr(Person_Paul_O_Conner, Person_Red_Stanson + 1);
        }

	if (livesIn(London_London_1, persID)) {
	    likes_to_beAll(persID, 0, Object_Location);

	    if (!(LIST_EMPTY(ObjectList))) {
		count = GetNrOfNodes(ObjectList);
		time = time * count / 1441;

		locID = OL_NR(GetNthNode(ObjectList, time));

		tcMoveAPerson(persID, locID);
	    }
	}
    }
}

void tcMoveAPerson(U32 persID, U32 newLocID)
{
    U32 oldLocID;

    hasAll(persID, 0, Object_Location);	/* wo is er denn ? */

    if (!(LIST_EMPTY(ObjectList))) {
	NODE *n;

	for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    oldLocID = OL_NR(n);

	    hasUnSet(persID, oldLocID);
	    hasUnSet(oldLocID, persID);
	}
    }

    hasSet(persID, newLocID);	/* neuen Ort setzen ! */
    hasSet(newLocID, persID);
}

U32 tcBurglary(U32 buildingID)
	/* wird von 2 Stellen aufgerufen! (story_9)! */
{
    S32 ret;
    Building b = dbGetObject(buildingID);

    if (buildingID == Building_Seniorenheim)
	ret = plPlayer(buildingID, 200, tcKarateOpa);
    else
	ret = plPlayer(buildingID, 0L, NULL);

    SetMenuTimeOutFunc(NULL);	/* sicher ist sicher... */

    if (ret) {			/* nur wenn Einbruch stattgefunden hat! */
	/* bei Grab nicht das Grab sondern "Highgate" aus der Taxiliste */
	/* entfernen */

	if (buildingID != Building_Grab_von_Karl_Marx)
	    RemTaxiLocation(b->LocationNr);
	else
	    RemTaxiLocation(((Building) dbGetObject(Location_Highgate_Out))->
			    LocationNr);

	hasUnSet(Person_Matt_Stuvysunt, buildingID);
    }

    CurrentBackground = BGD_LONDON;
    ShowMenuBackground();

    switch (ret) {
    case 0:
	RefreshCurrScene();

	tcPlaySound();

	return 0;		/* Plan hat nicht funktioniert! */
    case FAHN_ESCAPED:
	sndPlaySound("ok.bk", 0);

	inpDelay(300);

	return SCENE_FAHNDUNG;	/*Plan hat funktioniert & Flucht gelungen */
    case FAHN_NOT_ESCAPED:
	sndPlaySound("failed.bk", 0);

	inpDelay(300);

	return SCENE_PRISON;	/* Plan hat funktioniert & Flucht nicht gelungen */
    default:
	assert(0);
	return 0;
    }
}

void tcRefreshLocationInTitle(U32 locNr)
{
    char date[TXT_KEY_LENGTH], line[TXT_KEY_LENGTH];
    NODE *node;

    gfxSetPens(m_gc, 3, GFX_SAME_PEN, GFX_SAME_PEN);

    BuildDate(GetDay, date);
    node = GetNthNode(film->loc_names, locNr);

    sprintf(line, "%s %s", NODE_NAME(node), date);
    ShowMenuBackground();
    PrintStatus(line);
}

void StdInit(void)
{
    struct Scene *sc;
    ubyte sameLocation = 0;
    NODE *node;

    sc = GetCurrentScene();

    if (sc->LocationNr == GetLocation)
	sameLocation = 1;

    if ((sc->LocationNr != -1UL) && (GetLocation != sc->LocationNr))
	SetLocation(sc->LocationNr);

    tcRefreshLocationInTitle(sc->LocationNr);

    node = GetNthNode(film->loc_names, sc->LocationNr);

    if (((RefreshMode) || (!sameLocation)))
	PlayAnim(NODE_NAME(node), (word) 30000,
		 GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

    ShowTime(0);		/* Zeit sollte nach der Anim gezeigt werden, sonst Probleme mit Diskversion */

    tcPersonGreetsMatt();
}

void tcPlaySound()
{
    if (!(GamePlayMode & GP_MUSIC_OFF)) {
	switch (GetCurrentScene()->EventNr) {
	case SCENE_PARKER:
	    sndPlaySound("parker.bk", 0);
	    break;
	case SCENE_MALOYA:
	    sndPlaySound("maloya.bk", 0);
	    break;
	case SCENE_POOLY:
	    sndPlaySound("dealer.bk", 0);
	    break;

	case SCENE_WALRUS:
	    sndPlaySound("bar1.bk", 0);
	    break;
	case SCENE_FAT_MANS:
	    sndPlaySound("bar2.bk", 0);
	    break;
	case SCENE_HOTEL_ROOM:
	    sndPlaySound("hotel.bk", 0);
	    break;
	case SCENE_GARAGE:
	    sndPlaySound("cars.bk", 0);
	    break;
	case SCENE_PARKING:
	    sndPlaySound("cars.bk", 0);
	    break;
	case SCENE_CARS_OFFICE:
	    sndPlaySound("cars.bk", 0);
	    break;

	case SCENE_POLICE:
	case SCENE_GLUDO_SAILOR:
	case SCENE_ARRESTED_MATT:
	    sndPlaySound("gludo.bk", 0);
	    break;

	case SCENE_TOOLS:
	    sndPlaySound("shop.bk", 0);
	    break;

	case SCENE_AT_SABIEN:
	    sndPlaySound("sabien.bk", 0);
	    break;

	case SCENE_THE_END:
	case SCENE_PRISON:
	    sndPlaySound("end.bk", 0);
	    break;

	case SCENE_SOUTH_WITHOUT:
	case SCENE_SOUTH_UNKNOWN:
	case SCENE_SOUTHHAMPTON:
	    sndPlaySound("south.bk", 0);
	    break;

	case SCENE_FAHNDUNG:
	    sndPlaySound("fahndung.bk", 0);
	    break;

	default:
	    tcPlayStreetSound();
	    break;
	}
    }
}

void tcPlayStreetSound()
{
    if (!(GamePlayMode & GP_MUSIC_OFF)) {
        if (setup.Profidisk) {
	    switch (GetCurrentScene()->EventNr) {
	    case SCENE_PROFI_21:
	        sndPlaySound("snd21.bk", 0);
	        return;
	    case SCENE_PROFI_22:
	        sndPlaySound("snd22.bk", 0);
	        return;
	    case SCENE_PROFI_23:
	        sndPlaySound("snd23.bk", 0);
	        return;
	    case SCENE_PROFI_24:
	        sndPlaySound("snd24.bk", 0);
	        return;
	    case SCENE_PROFI_25:
	        sndPlaySound("snd25.bk", 0);
	        return;
	    case SCENE_PROFI_26:
	        sndPlaySound("snd26.bk", 0);
	        return;
	    case SCENE_PROFI_27:
	        sndPlaySound("snd27.bk", 0);
	        return;
	    case SCENE_PROFI_28:
	        sndPlaySound("snd28.bk", 0);
	        return;
            default:
                break;
            }
        }

	{
	    static ubyte counter = 0;
	    bool noStreetMusic = false;

	    if (strcmp(sndGetCurrSoundName(), "street1.bk") &&
	        strcmp(sndGetCurrSoundName(), "street2.bk") &&
	        strcmp(sndGetCurrSoundName(), "street3.bk"))
	        noStreetMusic = true;

	    if (!counter || noStreetMusic) {
	        counter = CalcRandomNr(7, 13);

	        switch (CalcRandomNr(0, 3)) {
	        case 0:
	            sndPlaySound("street1.bk", 0);
	            break;
	        case 1:
	            sndPlaySound("street2.bk", 0);
	            break;
	        default:
	            sndPlaySound("street3.bk", 0);
	            break;
	        }
	    } else
	        counter--;
	}
    }
}

void ShowTime(U32 delay)
{
    char time[TXT_KEY_LENGTH];

    BuildTime(GetMinute, time);

    gfxShow(25, GFX_NO_REFRESH | GFX_OVERLAY, delay, -1, -1);

    gfxSetFont(u_gc, bubbleFont);
    gfxSetPens(u_gc, 254, 252, GFX_SAME_PEN);
    gfxSetRect(280, 32);
    gfxSetDrMd(u_gc, GFX_JAM_1);

    gfxPrint(u_gc, time, 5, GFX_PRINT_CENTER | GFX_PRINT_SHADOW);
}

U32 StdHandle(U32 choice)
{
    U32 succ_eventnr = 0, locNr, objNr;
    struct Scene *scene = GetCurrentScene();
    char line[TXT_KEY_LENGTH];
    Location loc;

    switch ((U32) (choice)) {
    case GO:
	succ_eventnr = Go(scene->std_succ);

	if (succ_eventnr) {
	    locNr = GetScene(succ_eventnr)->LocationNr;

	    if (locNr != (U32) - 1) {
		objNr = GetObjNrOfLocation(locNr);

		if (objNr) {
		    loc = dbGetObject(objNr);

		    if ((GetMinute < loc->OpenFromMinute)
			|| (GetMinute > loc->OpenToMinute)) {
			ShowMenuBackground();

			txtGetFirstLine(THECLOU_TXT, "No_Entry", line);

			PrintStatus(line);
			inpWaitFor(INP_LBUTTONP);

			ShowMenuBackground();
			tcRefreshLocationInTitle(GetLocation);

			succ_eventnr = 0L;
		    } else
			StopAnim();
		}
	    }
	}
	break;
    case BUSINESS_TALK:
	succ_eventnr = Talk();
	AddVTime(7);
	ShowTime(0);
	break;
    case LOOK:
	Look((U32) GetCurrentScene()->LocationNr);
	AddVTime(1);
	ShowTime(0);
	break;
    case INVESTIGATE:
	Investigate(NODE_NAME
		    (GetNthNode
		     (film->loc_names, (GetCurrentScene()->LocationNr))));
	ShowTime(0);
	break;
    case MAKE_CALL:
	AddVTime(4);
	succ_eventnr = tcTelefon();
	ShowTime(0);
	break;
    case CALL_TAXI:
	if (CalcRandomNr(0, 10) == 1) {
	    sndPrepareFX("taxi.voc");
	    sndPlayFX();
	}

	succ_eventnr = GetLocScene(8)->EventNr;	/* taxi */
	break;
    case PLAN:
	if (!(GamePlayMode & GP_DEMO)) {
	    StopAnim();

	    film->StoryIsRunning = GP_STORY_PLAN;

	    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Building);

	    if (LIST_EMPTY(ObjectList)) {
		SetBubbleType(THINK_BUBBLE);
		Say(THECLOU_TXT, 0, MATT_PICTID, "NO_PLAN");
		ShowTime(0);
	    } else {
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Car);

		if (LIST_EMPTY(ObjectList)) {
		    SetBubbleType(THINK_BUBBLE);
		    Say(THECLOU_TXT, 0, MATT_PICTID, "NO_CAR_FOR_PLAN");
		    ShowTime(0);
		} else {
		    U32 building;

		    gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);

		    if (GamePlayMode & GP_NO_MUSIC_IN_PLANING)
			sndStopSound(0);

		    if ((building = tcOrganisation())) {
			AddVTime(27153);	/* etwas ber 15 Tage ! */

			succ_eventnr = tcBurglary(building);
		    } else {
			AddVTime(443);	/* etwas ber 7 Stunden */

			CurrentBackground = BGD_LONDON;
			ShowMenuBackground();

			gfxShow(173,
				GFX_ONE_STEP | GFX_NO_REFRESH | GFX_BLEND_UP, 3,
				-1, -1);
			tcRefreshLocationInTitle(GetLocation);

			succ_eventnr = 0L;
		    }
		}
	    }

	    film->StoryIsRunning = GP_STORY_TOWN;
	}
	ShowTime(0);
	break;
    case INFO:
	AddVTime(1);
	Information();
	ShowTime(0);
	break;
    case WAIT:
	tcWait();
	break;
    default:
	break;
    }

    return (succ_eventnr);
}

void StdDone(void)
{
    U32 choice;
    ubyte activ = 0;
    LIST *menu = txtGoKey(MENU_TXT, "Mainmenu");

    SceneArgs.ReturnValue = 0L;

    while (!SceneArgs.ReturnValue) {
	if (tcPersonIsHere())
	    if (!(SceneArgs.Moeglichkeiten & BUSINESS_TALK))
		SceneArgs.Moeglichkeiten |=
		    (BUSINESS_TALK & film->EnabledChoices);

        if (setup.Profidisk) {
	    if (GetCurrentScene()->EventNr == SCENE_PROFI_26) {
	        Environment env = (Environment)dbGetObject(Environment_TheClou);

	        if (env->PostzugDone)
		    SceneArgs.Moeglichkeiten &= ~INVESTIGATE;
	    }
        }

	if (SceneArgs.Moeglichkeiten) {
	    inpTurnESC(0);
	    inpTurnFunctionKey(1);

	    activ =
		Menu(menu, SceneArgs.Moeglichkeiten, (ubyte) (activ), NULL, 0L);

	    if (activ == (ubyte) - 1) {
		ShowTheClouRequester(No_Error);
		SceneArgs.ReturnValue =
		    ((Player) dbGetObject(Player_Player_1))->CurrScene;

		activ = 0;
	    } else {
		if (activ == ((ubyte) (TXT_MENU_TIMEOUT)))
		    activ = 0;
		else {
		    choice = (U32) 1L << (activ);

		    SceneArgs.ReturnValue = StdHandle(choice);
		}
	    }

	    inpTurnESC(1);
	} else
	    ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 1);
    }

    StopAnim();

    if (!(SceneArgs.Ueberschrieben))
	gfxChangeColors(l_gc, 5L, GFX_FADE_OUT, 0L);

    RemoveList(menu);
}


void InitTaxiLocations(void)
{
    RemRelation(Relation_taxi);	/* alle Relationen l”schen! */
    AddRelation(Relation_taxi);

    if (GamePlayMode & GP_STORY_OFF) {
	AddTaxiLocation(1);	/* cars */
	AddTaxiLocation(12);	/* aunt */
	AddTaxiLocation(14);	/* jewels */
	AddTaxiLocation(27);	/* kenw */
	AddTaxiLocation(45);	/* bank */

	if (!(GamePlayMode & GP_DEMO)) {
	    AddTaxiLocation(0);	/* holland */
	    AddTaxiLocation(2);	/* watling */
	    AddTaxiLocation(10);	/* trafik */
	    AddTaxiLocation(16);	/* anti */
	    AddTaxiLocation(18);	/* pink */
	    AddTaxiLocation(20);	/* senioren */
	    AddTaxiLocation(22);	/* highgate */
	    AddTaxiLocation(25);	/* villa */
	    AddTaxiLocation(29);	/* ham */
	    AddTaxiLocation(31);	/* osterly */
	    AddTaxiLocation(33);	/* chiswick */
	    AddTaxiLocation(35);	/* sotherbys */
	    AddTaxiLocation(37);	/* brit */
	    AddTaxiLocation(39);	/* natur */
	    AddTaxiLocation(41);	/* national */
	    AddTaxiLocation(43);	/* vict & alb */
	    AddTaxiLocation(47);	/* tower */
	    AddTaxiLocation(58);	/* vict */

            if (setup.Profidisk) {
	        AddTaxiLocation(81);	/* bulstrode */
	        AddTaxiLocation(79);	/* buck */
	        AddTaxiLocation(77);	/* tate */
	        AddTaxiLocation(75);	/* train */
	        AddTaxiLocation(74);	/* downing */
	        AddTaxiLocation(72);	/* abbey */
	        AddTaxiLocation(70);	/* madame */
	        AddTaxiLocation(68);	/* baker */
            }
	}
    } else {
	AddTaxiLocation(0);	/* holland */
	AddTaxiLocation(58);	/* victoria */
    }
}

void LinkScenes(void)
{
    (GetLocScene(8))->Done = DoneTaxi;
    (GetLocScene(51))->Done = DoneParking;
    (GetLocScene(50))->Done = DoneGarage;
    (GetLocScene(6))->Done = DoneTools;
    (GetLocScene(54))->Done = DoneDealer;
    (GetLocScene(53))->Done = DoneDealer;
    (GetLocScene(52))->Done = DoneDealer;

    SetFunc(GetScene(SCENE_KASERNE_OUTSIDE), StdInit, tcDoneKaserne);
    SetFunc(GetScene(SCENE_HOTEL_ROOM), StdInit, DoneHotelRoom);
    SetFunc(GetScene(SCENE_CREDITS), NULL, tcDoneCredits);
    SetFunc(GetScene(SCENE_FREIFAHRT), NULL, tcDoneFreeTicket);
    SetFunc(GetScene(SCENE_ARRIVAL), NULL, tcDoneArrival);
    SetFunc(GetScene(SCENE_HOTEL_1ST_TIME), NULL, tcDoneHotelReception);
    SetFunc(GetScene(SCENE_DANNER), NULL, tcDoneDanner);
    SetFunc(GetScene(SCENE_GLUDO_MONEY), NULL, tcDoneGludoMoney);
    SetFunc(GetScene(SCENE_MAMI_CALLS), NULL, tcDoneMamiCalls);
    SetFunc(GetScene(SCENE_FST_MEET_BRIGGS), NULL, tcDoneMeetBriggs);
    SetFunc(GetScene(SCENE_FAHNDUNG), tcInitFahndung, tcDoneFahndung);
    SetFunc(GetScene(SCENE_GLUDO_SAILOR), NULL, tcDoneGludoAsSailor);
    SetFunc(GetScene(SCENE_CALL_BRIGGS), NULL, tcDoneCallFromBriggs);
    SetFunc(GetScene(SCENE_1ST_BURG), NULL, tcDone1stBurglary);
    SetFunc(GetScene(SCENE_2ND_BURG), NULL, tcDone2ndBurglary);
    SetFunc(GetScene(SCENE_3RD_BURG), NULL, tcDone3rdBurglary);
    SetFunc(GetScene(SCENE_4TH_BURG), NULL, tcDone4thBurglary);
    SetFunc(GetScene(SCENE_5TH_BURG), NULL, tcDone5thBurglary);
    SetFunc(GetScene(SCENE_6TH_BURG), NULL, tcDone6thBurglary);
    SetFunc(GetScene(SCENE_7TH_BURG), NULL, tcDone7thBurglary);
    SetFunc(GetScene(SCENE_8TH_BURG), NULL, tcDone8thBurglary);
    SetFunc(GetScene(SCENE_9TH_BURG), NULL, tcDone9thBurglary);
    SetFunc(GetScene(SCENE_ARRESTED_MATT), NULL, tcDoneMattIsArrested);

    SetFunc(GetScene(SCENE_POOLY_AFRAID), NULL, tcDoneDealerIsAfraid);
    SetFunc(GetScene(SCENE_MALOYA_AFRAID), NULL, tcDoneDealerIsAfraid);
    SetFunc(GetScene(SCENE_PARKER_AFRAID), NULL, tcDoneDealerIsAfraid);
    SetFunc(GetScene(SCENE_RAID), NULL, tcDoneRaidInWalrus);
    SetFunc(GetScene(SCENE_DART), NULL, tcDoneDartJager);
    SetFunc(GetScene(SCENE_CALL_FROM_POOLY), NULL, tcDoneCallFromPooly);

    SetFunc(GetScene(SCENE_GLUDO_BURNS), NULL, tcDoneGludoBurnsOffice);
    SetFunc(GetScene(SCENE_MORNING), NULL, tcDoneBeautifullMorning);
    SetFunc(GetScene(SCENE_VISITING), NULL, tcDoneVisitingSabien);
    SetFunc(GetScene(SCENE_A_DREAM), NULL, tcDoneADream);
    SetFunc(GetScene(SCENE_ROSENBLATT), NULL, tcMeetingRosenblatt);
    SetFunc(GetScene(SCENE_BRIGGS_ANGRY), NULL, tcBriggsAngry);
    SetFunc(GetScene(SCENE_SABIEN_WALRUS), NULL, tcSabienInWalrus);
    SetFunc(GetScene(SCENE_SABIEN_DINNER), NULL, tcSabienDinner);
    SetFunc(GetScene(SCENE_TOMBOLA), NULL, tcWalrusTombola);
    SetFunc(GetScene(SCENE_PRESENT_HOTEL), NULL, tcPresentInHotel);
    SetFunc(GetScene(SCENE_INFO_TOWER), NULL, tcPoliceInfoTower);
    SetFunc(GetScene(SCENE_RAINY_EVENING), NULL, tcRainyEvening);
    SetFunc(GetScene(SCENE_MEETING_AGAIN), NULL, tcDoneMeetingAgain);

    SetFunc(GetScene(SCENE_SABIEN_CALL), NULL, tcDoneSabienCall);
    SetFunc(GetScene(SCENE_BIRTHDAY), NULL, tcDoneBirthday);
    SetFunc(GetScene(SCENE_WALK_WITH), NULL, tcWalkWithSabien);
    SetFunc(GetScene(SCENE_AGENT), NULL, tcDoneAgent);
    SetFunc(GetScene(SCENE_JAGUAR), NULL, tcDoneGoAndFetchJaguar);
    SetFunc(GetScene(SCENE_THINK_OF), NULL, tcDoneThinkOfSabien);
    SetFunc(GetScene(SCENE_TERROR), NULL, tcDoneTerror);
    SetFunc(GetScene(SCENE_CONFESSING), NULL, tcDoneConfessingSabien);

    SetFunc(GetScene(SCENE_PRISON), tcInitPrison, tcDonePrison);

    SetFunc(GetScene(SCENE_SOUTH_WITHOUT), NULL,
	    tcDoneSouthhamptonWithoutSabien);
    SetFunc(GetScene(SCENE_SOUTH_UNKNOWN), NULL,
	    tcDoneSouthhamptonSabienUnknown);
    SetFunc(GetScene(SCENE_SOUTHHAMPTON), StdInit, tcDoneSouthhampton);

    SetFunc(GetScene(SCENE_MISSED_DATE_0), NULL, tcDoneMissedDate);
    SetFunc(GetScene(SCENE_MISSED_DATE_1), NULL, tcDoneMissedDate);

    SetFunc(GetScene(SCENE_VILLA_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_KENWO_INSIDE), StdInit, DoneInsideHouse);

    SetFunc(GetScene(SCENE_TANTE_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_JUWEL_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_ANTIQ_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_TRANS_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_SENIO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_KENWO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_HAMHO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_OSTER_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_CHISW_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_SOTHE_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_BRITI_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_NATUR_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_NATIO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_VICTO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_BANKO_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_TOWER_INSIDE), StdInit, DoneInsideHouse);
    SetFunc(GetScene(SCENE_KASERNE_INSIDE), StdInit, DoneInsideHouse);

    if (setup.Profidisk) {
        SetFunc(GetScene(SCENE_PROFI_21_INSIDE), StdInit, DoneInsideHouse);
        SetFunc(GetScene(SCENE_PROFI_22_INSIDE), StdInit, DoneInsideHouse);
        SetFunc(GetScene(SCENE_PROFI_23_INSIDE), StdInit, DoneInsideHouse);
        SetFunc(GetScene(SCENE_PROFI_24_INSIDE), StdInit, DoneInsideHouse);
        SetFunc(GetScene(SCENE_PROFI_25_INSIDE), StdInit, DoneInsideHouse);
        /* SetFunc( GetScene(SCENE_PROFI_26_INSIDE),StdInit, DoneInsideHouse); */
        SetFunc(GetScene(SCENE_PROFI_27_INSIDE), StdInit, DoneInsideHouse);
        SetFunc(GetScene(SCENE_PROFI_28_INSIDE), StdInit, DoneInsideHouse);
    }
}

void SetFunc(struct Scene *sc, void (*init) (void), void (*done) (void))
{
    sc->Init = init;
    sc->Done = done;
}

ubyte tcPersonIsHere(void)
{
    U32 locNr = GetObjNrOfLocation(GetLocation);

    if (locNr) {
	if (locNr == Location_Fat_Mans_Pub) {
	    tcMoveAPerson(Person_Richard_Doil, locNr);
	} else if (locNr == Location_Cars_Vans_Office) {
	    tcMoveAPerson(Person_Marc_Smith, locNr);
	} else if (locNr == Location_Walrus) {
	    tcMoveAPerson(Person_Thomas_Smith, locNr);
	} else if (locNr == Location_Holland_Street) {
	    tcMoveAPerson(Person_Frank_Maloya, locNr);
	} else if (locNr == Location_Policestation) {
	    tcMoveAPerson(Person_John_Gludo, locNr);
	    tcMoveAPerson(Person_Miles_Chickenwing, locNr);
	} else if (locNr == Location_Hotel) {
	    tcMoveAPerson(Person_Ben_Riggley, locNr);
        }

	hasAll(locNr, 0, Object_Person);

	if (LIST_EMPTY(ObjectList))
	    return (0);
	else
	    return (1);
    }
    return (0);
}

void tcPersonGreetsMatt(void)
{
    static U32 upper = 4L;
    U32 locNr;

    if (CalcRandomNr(0L, upper) == 1) {	/* alle upper mal wird Matt gegrát ! */
	if (CalcRandomNr(0L, 4L) == 1)	/* alle 4 mal */
	    upper += 2;		/* wahrscheinlichkeit wird kleiner ! */

	locNr = GetObjNrOfLocation(GetLocation);

	if (locNr) {
	    hasAll(locNr, 0, Object_Person);

	    if (!(LIST_EMPTY(ObjectList))) {
		U32 persNr = OL_NR(LIST_HEAD(ObjectList));

		if (knows(Person_Matt_Stuvysunt, persNr)) {
		    Person pers = dbGetObject(persNr);

		    Say(BUSINESS_TXT, 0, pers->PictID, "HI_MATT");
		}
	    }
	}
    }
    ShowTime(0);
}

void tcGetLastName(char *Name, char *dest, U32 maxLength)
{
    char *s;
    char lastName[TXT_KEY_LENGTH];

    for (s = Name; *s != '\0'; s++)
	if (*s == ' ')
	    break;

    strcpy(lastName, s + 1);

    if (strlen(lastName) > maxLength)
	lastName[maxLength] = '\0';

    strcpy(dest, lastName);
}

void tcCutName(char *Name, ubyte Sign, U32 maxLength)
{
    S32 i;
    U32 j;
    char Source[TXT_KEY_LENGTH];

    strcpy(Source, Name);

    if ((j = strlen(Source)) > maxLength)
	j = maxLength;

    for (i = j - 1; i >= 0; i--)
	if (Source[i] == Sign)
	    Source[i] = '\0';

    strcpy(Name, Source);
}

void ShowMenuBackground(void)
{
    if (CurrentBackground != BGD_CLEAR)	/* MOD */
	gfxShow(CurrentBackground, GFX_ONE_STEP | GFX_NO_REFRESH, 0, -1, -1);
    else
	gfxClearArea(m_gc);
}

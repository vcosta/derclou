/*
**	$Filename: scenes/evidence.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	 functions for evidence for "Der Clou!"
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

#include "scenes/evidence.h"

U32 tcPersonWanted(U32 persId);
U32 tcPersonQuestioning(Person person);

struct Search Search;

static ubyte tcCarFound(Car car, U32 time)
{
    S32 i = 0, hours;
    Person john = dbGetObject(Person_John_Gludo);
    Person miles = dbGetObject(Person_Miles_Chickenwing);
    ubyte found = 0;

    /* Der Jaguar darf nicht gefunden werden - sonst kînnte er ja */
    /* nicht explodieren! */

    if (car != dbGetObject(Car_Jaguar_XK_1950)) {
	if (tcIsCarRecognised(car, time)) {	/* Wagen wird erkannt! */
	    Say(BUSINESS_TXT, 0, john->PictID, "CAR_RECOG");

	    hours = CalcRandomNr(2L, 5L);

	    while ((i++) < hours) {
		AddVTime(60);
		inpDelay(35);
		ShowTime(2);
	    }

	    if (!tcIsCarRecognised(car, time)) {	/* Wagen wird nicht gefunden */
		Say(BUSINESS_TXT, 0, john->PictID, "CAR_NOT_FOUND");
		car->Strike = CalcRandomNr(200, 255);
	    } else {		/* Wagen wird gefunden! */

		found = 1;
		Say(BUSINESS_TXT, 0, john->PictID, "CAR_FOUND");
		Say(BUSINESS_TXT, 0, miles->PictID, "GUTE_ARBEIT");
		UnSet(dbGetObject(Person_Matt_Stuvysunt), Relation_has, car);
	    }
	}
    }

    return found;
}

static U32 tcATraitor(U32 traitorId)
{
    char name[TXT_KEY_LENGTH], line[TXT_KEY_LENGTH];
    LIST *bubble = txtGoKey(BUSINESS_TXT, "A_TRAITOR");
    LIST *newList = CreateList();
    Person john = dbGetObject(Person_John_Gludo);

    dbGetObjectName(traitorId, name);
    sprintf(line, NODE_NAME(LIST_HEAD(bubble)), name);

    CreateNode(newList, 0L, line);
    CreateNode(newList, 0L, NODE_NAME(GetNthNode(bubble, 1)));
    CreateNode(newList, 0L, NODE_NAME(GetNthNode(bubble, 2)));

    SetPictID(john->PictID);
    Bubble(newList, 0, 0L, 0L);

    RemoveList(bubble);
    RemoveList(newList);

    Say(BUSINESS_TXT, 0, john->PictID, "ARRESTED");

    return 1;			/* gefangen! */
}

static U32 tcIsThereATraitor(void)
{
    Player player = dbGetObject(Player_Player_1);
    Person matt = dbGetObject(Person_Matt_Stuvysunt);
    ubyte symp = 255;
    U32 traitorId = 0, caught = 0;
    NODE *n;

    if (player->JobOfferCount > 50 + CalcRandomNr(0, 20)) {	/* ein Verrat?! */
	if (CalcRandomNr(0, 255) < matt->Popularity) {	/* Verrat ! */
	    joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME,
			 Object_Person);

	    for (n = (NODE *) LIST_HEAD(ObjectList); NODE_SUCC(n);
		 n = (NODE *) NODE_SUCC(n)) {
		Person pers = OL_DATA(n);

		if (OL_NR(n) != Person_Matt_Stuvysunt) {	/* Matt verrÑt sich nicht selbst */
		    if ((traitorId == 0) || (pers->Known < symp)) {
			traitorId = OL_NR(n);
			symp = pers->Known;
		    }
		}
	    }

	    caught = tcATraitor(traitorId);
	}
    }

    return caught;
}

U32 tcStartEvidence(void)
{
    S32 MyEvidence[4][7], guarded, radio;
    U32 totalEvidence[7], i, j, shownEvidence[4], Recognition[4], caught = 0;
    ubyte guyReady, guyNr, evidenceNr, guyCount, shown = 0;
    char line[TXT_KEY_LENGTH];
    Person p[4];
    Evidence evidence = dbGetObject(Evidence_Evidence_1);	/* just for presentation */
    struct ObjectNode *n;
    LIST *guys, *spuren;

    if ((!(Search.EscapeBits & FAHN_ALARM))
	&& (!(Search.EscapeBits & FAHN_QUIET_ALARM)))
	Say(BUSINESS_TXT, 0, ((Person) dbGetObject(Person_John_Gludo))->PictID,
	    "A_BURGLARY_SIR");

    Say(BUSINESS_TXT, 0,
	((Person) dbGetObject(Person_Miles_Chickenwing))->PictID,
	"START_EVIDENCE");

    joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
    guys = ObjectListPrivate;
    dbSortObjectList(&guys, dbStdCompareObjects);

    guyCount = (ubyte) GetNrOfNodes(guys);
    spuren = txtGoKey(BUSINESS_TXT, "SPUREN");

    p[0] = p[1] = p[2] = p[3] = NULL;

    guarded = ChangeAbs(((Building) dbGetObject(Search.BuildingId))->GRate,
			((Building) dbGetObject(Search.BuildingId))->Strike / 7,
			0, 255);

    radio = (S32) ((Building) dbGetObject(Search.BuildingId))->RadioGuarding;

    for (n = (struct ObjectNode *) LIST_HEAD(guys), i = 0; NODE_SUCC(n);
	 n = (struct ObjectNode *) NODE_SUCC(n), i++) {
	S32 div = 380;

	p[i] = OL_DATA(n);

	/* alle folgenden Werte sind zwischen 0 und 255 */

	/* statt durch 765 zu dividieren wurde ursprÅnglich durch 255 dividiert -> */
	/* viel zu gro·e Werte */

	if (Search.SpotTouchCount[i])
	    div = 1;

	MyEvidence[i][0] =
	    (((tcGetTrail(p[i], 0) * (S32) Search.WalkTime[i] *
	       (max(1, 255 - guarded))) / ((S32) Search.TimeOfBurglary +
					   1))) / div;
	MyEvidence[i][1] =
	    (((tcGetTrail(p[i], 1) * (S32) Search.WaitTime[i] *
	       (max(1, 255 - guarded))) / ((S32) Search.TimeOfBurglary +
					   1))) / div;
	MyEvidence[i][2] =
	    (((tcGetTrail(p[i], 2) * (S32) Search.WorkTime[i] *
	       (max(1, 255 - guarded))) / ((S32) Search.TimeOfBurglary +
					   1))) / div;
	MyEvidence[i][3] =
	    (((tcGetTrail(p[i], 3) * (S32) Search.KillTime[i] *
	       (max(1, 255 - guarded))) / ((S32) Search.TimeOfBurglary + 1)));
	MyEvidence[i][4] =
	    ChangeAbs(0, (S32) Search.CallCount * (S32) radio / 5, 0, 255);
	MyEvidence[i][5] =
	    (p[i]->KnownToPolice * (max(1, guarded))) / (div * 3);
	MyEvidence[i][6] =
	    ChangeAbs(0,
		      (S32) CalcRandomNr(200,
					 255) * (S32) Search.SpotTouchCount[i],
		      0, 255);

	for (j = 0; j < 7; j++)	/* jeden Betrag != 0 AUFRUNDEN auf 1% ! */
	    if (MyEvidence[i][j])
		MyEvidence[i][j] = max(MyEvidence[i][j], 5);

	if (has(Person_Matt_Stuvysunt, Tool_Maske))
	    MyEvidence[i][6] = (MyEvidence[i][6] * 2) / 3;

	/* im Fluchtfall viele Gehspuren! */
	if (Search.EscapeBits & FAHN_ESCAPE)
	    MyEvidence[i][0] =
		CalcValue(MyEvidence[i][0], CalcRandomNr(80, 120), 255, 255,
			  50);

	totalEvidence[i] =
	    MyEvidence[i][0] + MyEvidence[i][1] + MyEvidence[i][2] +
	    MyEvidence[i][3] + MyEvidence[i][4] + MyEvidence[i][5] +
	    MyEvidence[i][6];

	shownEvidence[i] = Recognition[i] = 0;

	tcPersonLearns(OL_NR(n));
    }

    prSetBarPrefs(m_gc, 300, 12, 1, 3, 0);

    guyReady = 0;
    txtGetFirstLine(BUSINESS_TXT, "FAHNDUNG", line);

    while (guyReady != ((1 << guyCount) - 1)) {
	if (shown) {
	    ShowTime(0);
	    inpDelay(35L);
	    AddVTime(CalcRandomNr(1, 11));

	    shown = 0;
	}

	guyNr = CalcRandomNr(0, guyCount);

	/* wer ist den noch nicht fertig? */
	while ((1 << guyNr) & guyReady)
	    guyNr = (guyNr + 1) % guyCount;

	/* bei folgendem CalcRandomNr darf nicht 4 - guyCount stehe, sonst
	 * Division durch 0!
	 */

	/* zufÑllig eine Spurenart auswÑhlen */
	evidenceNr = CalcRandomNr(0, 7);

	/* wenn diese Spurenart schon angzeigt wurde, eine freie
	 * Spur suchen
	 */
	j = 0;

	while (((1 << evidenceNr) & shownEvidence[guyNr]) && (j++ < 7))
	    evidenceNr = (evidenceNr + 1) % 7;

	if (j < 8) {		/* 8 stimmt! -> ober wird anschlie·end noch erhîht */
	    shownEvidence[guyNr] |= (1 << evidenceNr);

	    if (MyEvidence[guyNr][evidenceNr] > 0) {
		ShowMenuBackground();
		PrintStatus(NODE_NAME(GetNthNode(spuren, evidenceNr)));

		Recognition[guyNr] += MyEvidence[guyNr][evidenceNr];

		evidence->Recognition = Recognition[guyNr] / 3;	/* change also: totalEvidence /= 3.... */
		evidence->pers = (U32) OL_NR(GetNthNode(guys, (U32) guyNr));

		/* fÅr alle "Evidences" - stimmt so, da fÅr alle */
		/* Personen die selbe Evidence Struct benutzt wird -> */
		/* bestimmte Werte sind von vorher gesetzt und mÅssen gelîscht */
		/* werden */

		evidence->WalkTrail = 0;
		evidence->WaitTrail = 0;
		evidence->WorkTrail = 0;
		evidence->KillTrail = 0;
		evidence->CallTrail = 0;
		evidence->PaperTrail = 0;
		evidence->FotoTrail = 0;

		if (shownEvidence[guyNr] & 1)
		    evidence->WalkTrail = MyEvidence[guyNr][0];

		if (shownEvidence[guyNr] & 2)
		    evidence->WaitTrail = MyEvidence[guyNr][1];

		if (shownEvidence[guyNr] & 4)
		    evidence->WorkTrail = MyEvidence[guyNr][2];

		if (shownEvidence[guyNr] & 8)
		    evidence->KillTrail = MyEvidence[guyNr][3];

		if (shownEvidence[guyNr] & 16)
		    evidence->CallTrail = MyEvidence[guyNr][4];

		if (shownEvidence[guyNr] & 32)
		    evidence->PaperTrail = MyEvidence[guyNr][5];

		if (shownEvidence[guyNr] & 64)
		    evidence->FotoTrail = MyEvidence[guyNr][6];

		Present(Evidence_Evidence_1, "Fahndung", InitEvidencePresent);

		ShowMenuBackground();
		PrintStatus(line);

		shown = 1;
	    }
	}

	if (shownEvidence[guyNr] == ((1 << 7) - 1))
	    guyReady |= (1 << guyNr);
    }

    guyReady = 0;

    /* ein gewisses Restwissen bleibt der Polizei ! */
    for (i = 0; i < guyCount; i++) {
	totalEvidence[i] /= 3;	/* change als in recognition = ... */

	if (p[i] != 0) {
	    if (totalEvidence[i] > 255)
		totalEvidence[i] = 255;

	    p[i]->KnownToPolice = (ubyte) (totalEvidence[i]);

	    if (p[i]->KnownToPolice > tcPERSON_IS_ARRESTED)
		caught |= tcPersonWanted(OL_NR(GetNthNode(guys, i)));

	    if (!caught)
		if (p[i] == dbGetObject(Person_Robert_Bull))
		    caught |= tcATraitor(Person_Robert_Bull);

	    if (!caught)
		if (p[i] == dbGetObject(Person_Marc_Smith))
		    caught |= tcATraitor(Person_Marc_Smith);

            if (setup.Profidisk) {
	        if (!caught)
		    if (p[i] == dbGetObject(Person_Phil_Ciggy))
		        caught |= tcATraitor(Person_Phil_Ciggy);
            }
	}
    }

    caught |= tcIsThereATraitor();

    if (!
	(tcCarFound
	 ((Car) dbGetObject(Organisation.CarID),
	  Search.TimeOfBurglary - Search.TimeOfAlarm))) {
	S32 newStrike;
	Car car = dbGetObject(Organisation.CarID);

	newStrike = CalcValue((S32) car->Strike, 0, 255, 255, 15);

	if (newStrike < (car->Strike + 40))
	    newStrike = ChangeAbs((S32) car->Strike, 40, 0, 255);

	car->Strike = newStrike;
    }

    ((Player) dbGetObject(Player_Player_1))->MattsPart =
	(ubyte) tcCalcMattsPart();
    tcForgetGuys();

    RemoveList(spuren);
    RemoveList(guys);

    return caught;
}

void tcForgetGuys(void)
{
    LIST *guys;
    NODE *node;

    joined_byAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST, Object_Person);
    guys = ObjectListPrivate;

    for (node = (NODE *) LIST_HEAD(guys); NODE_SUCC(node);
	 node = (NODE *) NODE_SUCC(node)) {
	if (OL_NR(node) != Person_Matt_Stuvysunt) {
	    Person pers = OL_DATA(node);

	    pers->TalkBits |= (1 << Const_tcTALK_JOB_OFFER);	/* Åber Jobs kann man wieder reden! */

	    joined_byUnSet(Person_Matt_Stuvysunt, OL_NR(node));
	    joinUnSet(Person_Matt_Stuvysunt, OL_NR(node));
	    rememberUnSet(Person_Matt_Stuvysunt, OL_NR(node));
	}
    }

    RemoveList(guys);
}

U32 tcPersonWanted(U32 persId)
{
    U32 hours, i = 0, caught = 0;
    Person john = dbGetObject(Person_John_Gludo);
    Person miles = dbGetObject(Person_Miles_Chickenwing);
    LIST *bubble;
    LIST *jobs = txtGoKey(OBJECTS_ENUM_TXT, "enum_JobE");
    char line[TXT_KEY_LENGTH], name[TXT_KEY_LENGTH];

    dbGetObjectName(persId, name);

    bubble = txtGoKey(BUSINESS_TXT, "BURGLAR_RECOG");

    sprintf(line, "%s %s.", NODE_NAME(GetNthNode(bubble, 3)), name);

    RemoveNode(bubble, NODE_NAME(GetNthNode(bubble, 3)));
    CreateNode(bubble, 0L, line);

    SetPictID(john->PictID);
    Bubble(bubble, 0, 0L, 0L);
    RemoveList(bubble);

    Say(BUSINESS_TXT, 0, miles->PictID, "ARREST_HIM");
    livesInUnSet(London_London_1, persId);
    tcMoveAPerson(persId, Location_Nirvana);

    hours = CalcRandomNr(4L, 7L);

    while ((i++) < hours) {
	AddVTime(60);
	inpDelay(35);
	ShowTime(2);
    }

    if (tcGuyCanEscape(dbGetObject(persId)) > CalcRandomNr(100, 255)) {	/* Flucht gelingt */
	Say(BUSINESS_TXT, 0, john->PictID, "ESCAPED");

	livesInSet(London_Escape, persId);
    } else {			/* nicht */

	Say(BUSINESS_TXT, 0, john->PictID, "ARRESTED");

	livesInSet(London_Jail, persId);

	caught = tcPersonQuestioning(dbGetObject(persId));
    }

    RemoveList(jobs);

    return caught;
}

U32 tcPersonQuestioning(Person person)
{
    U32 caught = 0;
    Person john = dbGetObject(Person_John_Gludo);
    Person miles = dbGetObject(Person_Miles_Chickenwing);

    if (person != dbGetObject(Person_Matt_Stuvysunt)) {
	if (tcGuyTellsAll(person) > CalcRandomNr(0, 180)) {	/* er spricht */
	    Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT");
	    Say(BUSINESS_TXT, 0, miles->PictID, "GUTE_ARBEIT");
	    caught = 1;
	} else			/* er spricht nicht */
	    Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT_NICHT");
    } else {
	Say(BUSINESS_TXT, 0, john->PictID, "ER_GESTEHT_NICHT");
	caught = 1;
    }

    return caught;
}

S32 tcEscapeFromBuilding(U32 escBits)
{
    Person gludo = dbGetObject(Person_John_Gludo);
    ubyte escapeSucc = FAHN_NOT_ESCAPED;
    S32 timeLeft = INT32_MAX;

    /* Fluchtbilder zeigen! */
    livSetAllInvisible();
    lsSetDarkness(((LSArea) dbGetObject(lsGetActivAreaID()))->uch_Darkness);

    if (escBits & FAHN_ESCAPE) {
	gfxShow(213, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	inpDelay(80);

	gfxShow(215, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	inpDelay(80);
    }

    /*
     * Behandlung von "Umstellt"
     * sowie Berechnung eines Fluchtversuches (in jedem Flucht - fall)
     */

    if (!(escBits & FAHN_SURROUNDED)) {
	if ((escBits & FAHN_ALARM) || (escBits & FAHN_QUIET_ALARM)) {
	    Building build = dbGetObject(Search.BuildingId);

	    timeLeft =
		build->PoliceTime - (Search.TimeOfBurglary -
				     Search.TimeOfAlarm);
	    timeLeft -= tcCalcEscapeTime();

	    if (timeLeft > 0)
		escapeSucc = FAHN_ESCAPED;
	} else
	    escapeSucc = FAHN_ESCAPED;
    }

    if (escapeSucc == FAHN_ESCAPED) {
	gfxShow(211, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);
	inpDelay(120);
    } else {
	Say(BUSINESS_TXT, 0, gludo->PictID, "UMSTELLT");	/* noch ein Umstellt */
	timeLeft = 0;		/* wenn Zeit nicht gereicht hat */
    }

    return timeLeft;
}

S32 tcEscapeByCar(U32 escBits, S32 timeLeft)
{
    Person gludo = dbGetObject(Person_John_Gludo);
    Person miles = dbGetObject(Person_Miles_Chickenwing);
    ubyte escapeSucc;

    if (timeLeft > 0)
	escapeSucc = FAHN_ESCAPED;
    else
	escapeSucc = FAHN_NOT_ESCAPED;

    /* Flucht per Auto! */
    if (escapeSucc == FAHN_ESCAPED) {
	if ((Search.BuildingId != Building_Tower_of_London) &&
	    (Search.BuildingId != Building_Starford_Kaserne))
	    escapeSucc = tcCalcCarEscape(timeLeft);
    } else
	tcMattGoesTo(7);	/* Polizei */

    /* Ausgabe : Flucht gelungen, Flucht nicht gelungen */
    if (escapeSucc == FAHN_ESCAPED) {
	if ((escBits & FAHN_ALARM) || (escBits & FAHN_QUIET_ALARM)) {
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_0");	/* entkommen!  */
	    Say(BUSINESS_TXT, 0, miles->PictID, "HINWEIS_2");	/* woher Hinweis? */
	} else
	    Say(BUSINESS_TXT, 0, OLD_MATT_PICTID, "I_ESCAPED");	/* no problems */
    } else {
	Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_1");	/* geschnappt! */
	Say(BUSINESS_TXT, 0, miles->PictID, "HINWEIS_2");	/* woher Hinweis? */
    }

    /* Gludo erzÑhlt woher Hinweise stammten (falls es welche gibt) */
    if (escBits & FAHN_ALARM) {
	if (escBits & FAHN_ALARM_ALARM)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_6");	/* Alarmanlage */

	if (escBits & FAHN_ALARM_POWER)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_8");

	if (escBits & FAHN_ALARM_TIMER)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_6");	/* Alarmanlage */

	if (escBits & FAHN_ALARM_MICRO)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_9");

	if (escBits & FAHN_ALARM_TIMECLOCK)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_7");	/* Alarmanlage */

	if (escBits & FAHN_ALARM_GUARD)
	    Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_10");
    } else {
	if (escBits & FAHN_QUIET_ALARM) {
	    if (escBits & FAHN_ALARM_RADIO)
		Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_5");

	    if (escBits & FAHN_ALARM_PATRO)
		Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_4");

	    if (escBits & FAHN_ALARM_LOUDN)
		Say(BUSINESS_TXT, 0, gludo->PictID, "HINWEIS_3");
	}
    }

    if (escapeSucc != FAHN_ESCAPED)
	StopAnim();

    return ((S32) escapeSucc);
}

S32 tcCalcCarEscape(S32 timeLeft)
{
    ubyte kmhWeight[4] = { 32, 44, 60, 67 };
    ubyte psWeight[4] = { 68, 56, 40, 33 };
    ubyte driverWeight[4] = { 50, 40, 25, 20 };
    ubyte policeSpeed[4] = { 90, 95, 103, 107 }
    , paint;
    char line[TXT_KEY_LENGTH];
    S32 kmh, ps, i, j, YardsInFront, length, wayType, unrealSpeed, x, xOldMatt =
	-1, xOldPoli = -1;
    Car car = dbGetObject(Organisation.CarID);
    Building build = dbGetObject(Organisation.BuildingID);
    S32 result = FAHN_ESCAPED;
    U8 palette[GFX_PALETTE_SIZE];

    if ((Organisation.BuildingID != Building_Tower_of_London) &&
	(Organisation.BuildingID != Building_Starford_Kaserne)) {
	wayType = build->EscapeRoute;
	length = build->EscapeRouteLength;

	kmh = car->Speed;
	ps = car->PS;

	/* Motor : pro 10% Schaden -> 5% Leistung weniger */
	kmh = CalcValue(kmh, 0, 65535L, (car->MotorState) / 2, 50);
	ps = CalcValue(ps, 0, 65535L, (car->MotorState) / 2, 50);

	/* pro Jahr 3% weniger */
	i = (car->Speed * 3 * (tcRGetCarAge(car) + 1)) / 100;
	j = (car->PS * 3 * (tcRGetCarAge(car) + 1)) / 100;

	if (kmh >= i)
	    kmh -= i;
	else
	    kmh = 0;

	if (ps >= j)
	    ps -= j;
	else
	    ps = 0;

	/* Reifen pro 10% um 2% weniger */
	i = (car->Speed * 2 * (255 - car->TyreState)) / 2500;
	j = (car->Speed * 2 * (255 - car->PS)) / 2500;

	if (kmh >= i)
	    kmh -= i;
	else
	    kmh = 0;

	if (ps >= j)
	    ps -= j;
	else
	    ps = 0;

	/* Fahrer */
	/* kann aus einem Auto um ein Viertel mehr herausholen, als eigentlich */
	/* drinnen ist! */
	i = hasGet(Organisation.DriverID, Ability_Autos);
	kmh =
	    CalcValue(kmh, 0, car->Speed + car->Speed / 4, i,
		      driverWeight[wayType]);
	ps = CalcValue(ps, 0, car->PS + car->PS / 4, i, driverWeight[wayType]);

	/* Ma·zahl fÅr Geschwindigkeit */
	/* Einheit = m pro Schleifendurchlauf */
	unrealSpeed = (kmh * kmhWeight[wayType] + ps * psWeight[wayType]) / 100;

	unrealSpeed = unrealSpeed + 5 - (S32) (CalcRandomNr(0, 10));

	if (unrealSpeed <= 0)
	    unrealSpeed = 5;

	policeSpeed[wayType] = policeSpeed[wayType] + 5 - CalcRandomNr(0, 10);
	policeSpeed[wayType] =
	    CalcValue(policeSpeed[wayType], 0, 255, build->GRate, 25);

	/* Vorsprung berechnen */
	/* Vorsprung ist maximal die halbe Strecke (length * 500 m) und */
	/* mindestens 2000 Meter (damit man von der Fluct etwas sieht!) */

	i = min(((unrealSpeed * timeLeft) / 5), length * 500);
	i = max(2000, i);	/* mindestens 2000 Meter */

	j = 0;
	length *= 1000;		/* Fluchtweg in Meter */

	/* Bildschirmdarstellung */

	txtGetFirstLine(BUSINESS_TXT, "FLUCHT", line);
	ShowMenuBackground();
	PrintStatus(line);

	gfxChangeColors(l_gc, 0, GFX_FADE_OUT, 0);
	gfxShow(car->PictID, GFX_NO_REFRESH | GFX_ONE_STEP, 0, -1, -1);

	gfxPrepareColl(GFX_COLL_PARKING);
	gfxGetPalette(GFX_COLL_PARKING, palette);

	gfxChangeColors(l_gc, 0, GFX_BLEND_UP, palette);
	SetCarColors(car->ColorIndex);

	gfxSetPens(l_gc, 20, GFX_SAME_PEN, 21);
	gfxRectFill(l_gc, 60, 40, 60 + 199, 40 + 19);

	i = max(i, 60);
	j = max(j, 5);

	sndPrepareFX("flucht.voc");
	sndPlayFX();

	do {
	    i += unrealSpeed;	/* KmH von Matt */
	    i = max(i, 0);

	    paint = 0;

	    if (Search.TimeOfAlarm)
		j += policeSpeed[wayType];

	    YardsInFront = i - j;	/* zum Vorsprung addieren */

	    /* Berechnung der Darstellung */
	    x = (i * 190) / length;	/* Matts Car */

	    if (x != xOldMatt) {
		if (xOldMatt != -1) {
		    gfxSetPens(l_gc, 20, GFX_SAME_PEN, 20);

		    if ((195 - x + 1) < (198 + 195 - xOldMatt))
			gfxRectFill(l_gc, 60 + 195 - xOldMatt, 45,
				    60 + 198 - xOldMatt, 55);
		}

		gfxSetPens(l_gc, 11, GFX_SAME_PEN, 11);
		gfxRectFill(l_gc, 60 + 195 - x, 45, 60 + 198 - x, 55);

		xOldMatt = x;
		paint = 1;
	    }

	    if (Search.TimeOfAlarm) {	/* Police Car */
		x = (j * 190) / length;

		if (x != xOldPoli) {
		    if (xOldPoli != -1) {
			gfxSetPens(l_gc, 20, GFX_SAME_PEN, 20);
			gfxRectFill(l_gc, 60 + 195 - xOldPoli, 45,
				    60 + 198 - xOldPoli, 55);
		    }

		    gfxSetPens(l_gc, 23, GFX_SAME_PEN, 23);
		    gfxRectFill(l_gc, 60 + 195 - x, 45, 60 + 198 - x, 55);

		    paint = 1;
		}

		xOldPoli = x;
	    }

	    if (paint)
		inpDelay(3);
	}
	while ((i < length) && (YardsInFront > 50));

	if (YardsInFront > 50)
	    result = FAHN_ESCAPED;
	else
	    result = FAHN_NOT_ESCAPED;
    }

    return result;
}

/*
**      $Filename: planing/player.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.player for "Der Clou!"
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

#include "planing/player.h"


/* Menu ids - player */
#define PLANING_PLAYER_PERSON_CHANGE   0
#define PLANING_PLAYER_RADIO_ALL       1
#define PLANING_PLAYER_RADIO_ONE       2
#define PLANING_PLAYER_ESCAPE          3

#ifdef THECLOU_DEBUG
#define PLANING_PLAYER_DEBUG           4
#endif

/* Player defines */
#define PLANING_EXHAUST_MAX         240
#define PLANING_MOOD_MIN            40
#define PLANING_LOUDNESS_STD        5
#define PLANING_LOUDNESS_RADIO      25
#define PLANING_LOUDNESS_OPEN_CLOSE 14

/* pseudo actions */
#define ACTION_EXHAUST  1000

/* derivation defines */
#define PLANING_DERI_ALARM              (PD.realTime / CalcRandomNr(1, 3))
#define PLANING_DERI_WATCHDOG           (PD.realTime / CalcRandomNr(6, 10))
#define PLANING_DERI_UNABLE_TO_WORK     3
#define PLANING_DERI_IGNORE_ACTION      (PD.realTime / CalcRandomNr(15, 20))
#define PLANING_DERI_GUARD_ESCAPED      (PD.realTime / CalcRandomNr(4, 8))

/* sound system */
#define PLANING_MUSIC_PLAYER_BEGIN_KASERNE   "final.bk"
#define PLANING_MUSIC_PLAYER_BEGIN_STD       "bruch1.bk"
#define PLANING_MUSIC_PLAYER_END_STD         "bruch2.bk"

/* if no checking should be done define this */
/* #define PLAN_IS_PERFECT */


/* Player data structure */
static struct {
    struct Action *action;

    ubyte handlerEnded[PLANING_NR_PERSONS];
    ubyte guardKO[PLANING_NR_GUARDS];
    ubyte currLoudness[PLANING_NR_PERSONS];
    ubyte unableToWork[PLANING_NR_PERSONS];

    ubyte ende;

    U32 maxTimer;
    U32 timer;
    U32 realTime;

    ubyte badPlaning;
    ubyte mood;

    U32 bldId;
    Building bldObj;

    uword changeCount;
    uword totalCount;
    ubyte patrolCount;

    U32 alarmTimer;

    ubyte isItDark;

    ubyte sndState;	/* Careful! Behaviour of sndState is different from the Amiga version */

    U32 actionTime;
     ubyte(*actionFunc) (U32, U32);
} PD;


static void CheckSurrounding(U32 current)
{
#ifndef PLAN_IS_PERFECT
    if (has(OL_NR(GetNthNode(PersonsList, current)), Ability_Aufpassen)) {
	register S32 watch = 0;

	if (Search.EscapeBits & FAHN_QUIET_ALARM)
	    watch = tcWatchDogWarning(OL_NR(GetNthNode(PersonsList, current)));
	else
	    watch =
		tcWrongWatchDogWarning(OL_NR(GetNthNode(PersonsList, current)));

	if (watch) {
	    plSay("PLAYER_WATCHDOG", current);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    Search.DeriTime += PLANING_DERI_WATCHDOG;
	    Search.WarningCount++;
	}
    }
#endif
}

static void UnableToWork(U32 current, U32 action)
{
    PD.currLoudness[current] = PLANING_LOUDNESS_STD;
    Search.Exhaust[current] =
	tcGuyIsWaiting(OL_NR(GetNthNode(PersonsList, current)),
		       Search.Exhaust[current]);
    Search.WaitTime[current]++;

    livAnimate(Planing_Name[current], ANM_STAND, 0, 0);

    /* special stuff */
    if (!PD.unableToWork[current]) {
	switch (action) {
	case ACTION_EXHAUST:
	    plSay("PLAYER_UTW_EXHAUST", current);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    break;

	case ACTION_GO:
	    plSay("PLAYER_UTW_GO", current);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    break;

	case ACTION_OPEN:
	    plSay("PLAYER_UTW_OPEN", current);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    break;

	case ACTION_CLOSE:
	    plSay("PLAYER_UTW_CLOSE", current);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    break;
	}

	PD.unableToWork[current] = 1;
    }
    /* if we can not go on wait and check */
    CheckSurrounding(current);

    if ((action != ACTION_WAIT) && (action != ACTION_WAIT_SIGNAL)) {
	Search.DeriTime += PLANING_DERI_UNABLE_TO_WORK;

	if (!PD.badPlaning) {
	    plSay("PLAYER_BAD_PLANING", 0);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
	    PD.badPlaning = 1;
	}
    }

    if (action != ACTION_WAIT)
	IncCurrentTimer(plSys, 1, 0);
}

static ubyte plGetMood(U32 time)
{
    U32 guyId[PLANING_NR_PERSONS];
    ubyte i;

    for (i = 0; i < PLANING_NR_PERSONS; i++)
	guyId[i] = 0L;

    for (i = 0; i < BurglarsNr; i++)
	guyId[i] = OL_NR(GetNthNode(PersonsList, i));

    return (ubyte) tcGetTeamMood(guyId, time);
}

static void plPersonLearns(U32 persId, U32 toolId)
{
    U32 para;

    hasAll(persId, OLF_NORMAL, Object_Ability);

    switch (toolId) {
    case Tool_Hand:
    case Tool_Fusz:
    case Tool_Chloroform:
	para = learnedGet(persId, Ability_Kampf);

	if (para == NO_PARAMETER)
	    learnedSetP(persId, Ability_Kampf, 1);
	else
	    learnedSetP(persId, Ability_Kampf, para + 1);
	break;

    case Tool_Winkelschleifer:
    case Tool_Kernbohrer:
    case Tool_Schneidbrenner:
    case Tool_Sauerstofflanze:
    case Tool_Stethoskop:
    case Tool_Elektrohammer:
	para = learnedGet(persId, Ability_Safes);

	if (para == NO_PARAMETER)
	    learnedSetP(persId, Ability_Safes, 1);
	else
	    learnedSetP(persId, Ability_Safes, para + 1);
	break;

    case Tool_Dietrich:
    case Tool_Brecheisen:
    case Tool_Bohrmaschine:
    case Tool_Bohrwinde:
    case Tool_Schloszstecher:
    case Tool_Glasschneider:
	para = learnedGet(persId, Ability_Schloesser);

	if (para == NO_PARAMETER)
	    learnedSetP(persId, Ability_Schloesser, 1);
	else
	    learnedSetP(persId, Ability_Schloesser, para + 1);
	break;

    case Tool_Elektroset:
	para = learnedGet(persId, Ability_Elektronik);

	if (para == NO_PARAMETER)
	    learnedSetP(persId, Ability_Elektronik, 1);
	else
	    learnedSetP(persId, Ability_Elektronik, para + 1);
	break;

    case Tool_Dynamit:
	para = learnedGet(persId, Ability_Sprengstoff);

	if (para == NO_PARAMETER)
	    learnedSetP(persId, Ability_Sprengstoff, 1);
	else
	    learnedSetP(persId, Ability_Sprengstoff, para + 1);
	break;
    }
}

static ubyte plCarTooFull(void)
{
    LIST *l = tcMakeLootList(Person_Matt_Stuvysunt, Relation_has);

    CompleteLoot complete = (CompleteLoot) dbGetObject(CompleteLoot_LastLoot);
    Car car = (Car) dbGetObject(Organisation.CarID);
    ubyte ret = 0;

    if (complete->TotalVolume > car->Capacity)
	ret = 1;

    RemoveList(l);

    return ret;
}

static void plPlayerAction(void)
{
    register ubyte i, patroCounter = 3;
    register ubyte DoScrolling = 0;

    PD.timer++;
    PD.realTime = PD.timer / PLANING_CORRECT_TIME;

    plDisplayTimer(PD.realTime, 0);

    if (PD.sndState && (PD.timer > (PD.maxTimer * 20) / 100)
	&& (PD.bldId != Building_Starford_Kaserne)) {
	sndPlaySound(PLANING_MUSIC_PLAYER_END_STD, 0L);
	PD.sndState = 0;
    }
#ifndef PLAN_IS_PERFECT
    if (!(Search.EscapeBits & FAHN_ALARM_TIMECLOCK) && !(PD.timer % 3)
	&& tcCheckTimeClocks(PD.bldId)) {
	Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_TIMECLOCK;
	Search.DeriTime += PLANING_DERI_ALARM;
	plSay("PLAYER_TIMECLOCK", 0);
	inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
    }
#endif

#ifndef PLAN_IS_PERFECT
    if (!(PD.timer % 15)) {
	if (tcAlarmByLoudness
	    (PD.bldObj,
	     tcGetTotalLoudness(PD.currLoudness[0], PD.currLoudness[1],
				PD.currLoudness[2], PD.currLoudness[3])))
	    Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_LOUDN;
    }
#endif

    if (PD.bldId == Building_Tower_of_London)
	patroCounter = 9;

    if (PD.bldId == Building_Starford_Kaserne)
	patroCounter = 30;

    if (!(PD.timer % patroCounter)) {
#ifndef PLAN_IS_PERFECT
	if ((CalcRandomNr(0, 30 * (270 - tcRGetGRate(PD.bldObj) / 2 + 1))) == 0) {
	    plSay("PLAYER_PATROL", 0);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

	    PD.patrolCount++;

	    if (tcAlarmByPatrol
		(PD.changeCount, PD.totalCount, PD.patrolCount))
		Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_PATRO;
	}
#endif
    }
#ifndef PLAN_IS_PERFECT
    if (PD.alarmTimer && !(PD.timer % 180)) {
	if (!(--PD.alarmTimer))
	    Search.EscapeBits |= FAHN_QUIET_ALARM | FAHN_ALARM_TIMER;
    }
#endif

#ifndef PLAN_IS_PERFECT
    if ((Search.EscapeBits & FAHN_QUIET_ALARM)
	|| (Search.EscapeBits & FAHN_ALARM)) {
	if (!Search.TimeOfAlarm) {
	    Search.TimeOfAlarm = PD.realTime;

	    if (Search.EscapeBits & FAHN_ALARM) {
		sndPrepareFX("sirene.voc");
		sndPlayFX();
	    }
	}

	if ((Search.EscapeBits & FAHN_ALARM)
	    && (((PD.realTime - Search.TimeOfAlarm) % 120) == 119)) {
	    sndPrepareFX("sirene.voc");
	    sndPlayFX();
	}

	if (PD.realTime >= (Search.TimeOfAlarm + PD.bldObj->PoliceTime)) {
	    Search.EscapeBits |= FAHN_SURROUNDED;

	    for (i = 0; i < PLANING_NR_PERSONS; i++)
		PD.handlerEnded[i] = 1;

	    sndPrepareFX("marthorn.voc");
	    sndPlayFX();
	}
    }
#endif

#ifndef PLAN_IS_PERFECT
    if ((PD.bldId != Building_Starford_Kaserne) && !(PD.timer % 15)) {
	if ((PD.mood = plGetMood(PD.realTime)) < PLANING_MOOD_MIN) {
	    plSay("PLAYER_FLUCHT", 0);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

	    Search.EscapeBits |= FAHN_ESCAPE;

	    for (i = 0; i < PLANING_NR_PERSONS; i++)
		PD.handlerEnded[i] = 1;
	}
    }
#endif

    if ((PD.realTime == PD.actionTime) && PD.actionFunc) {
	ubyte actionRet;

	if ((actionRet = PD.actionFunc(PD.actionTime, PD.bldId))) {
	    char actionText[TXT_KEY_LENGTH];

	    sprintf(actionText, "PLAYER_ACTION_%d", actionRet);
	    plSay(actionText, 0);
	    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);

	    Search.EscapeBits |= FAHN_ESCAPE;

	    for (i = 0; i < PLANING_NR_PERSONS; i++)
		PD.handlerEnded[i] = 1;
	}
    }
#ifndef PLAN_IS_PERFECT
    if (!(PD.timer % 3)) {
	if (PD.isItDark) {
	    register ubyte j, i;
	    uword xpos[PLANING_NR_PERSONS];
	    uword ypos[PLANING_NR_PERSONS];
	    U32 area[PLANING_NR_PERSONS];

	    /* there are more efficient solutions, but no safer ones */
	    for (i = 0; i < PLANING_NR_PERSONS; i++) {
		xpos[i] = (uword) - 1;
		ypos[i] = (uword) - 1;
		area[i] = (U32) - 1;
	    }

	    for (j = 0; j < BurglarsNr; j++) {
		xpos[j] = livGetXPos(Planing_Name[j]);
		ypos[j] = livGetYPos(Planing_Name[j]);
		area[j] = livWhereIs(Planing_Name[j]);
	    }


	    lsGuyInsideSpot(xpos, ypos, area);
	}
    }
#endif

    for (i = 0; i < PersonsNr; i++) {
	if ((i >= BurglarsNr) || !PD.handlerEnded[i]) {
	    SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, i)));

	    if ((i >= BurglarsNr) && PD.guardKO[i - BurglarsNr]) {
		switch (PD.guardKO[i - BurglarsNr]) {
		case 1:
		    livAnimate(Planing_Name[i],
			       livGetViewDirection(Planing_Name[i]), 0, 0);
		    break;

		case 2:
		    livAnimate(Planing_Name[i], ANM_DUSEL_POLICE, 0, 0);
		    break;

		case 3:
		    livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0, 0);
		    break;
		}
	    } else if ((PD.action = NextAction(plSys))) {
		if (!(PD.timer % 12) && (i >= BurglarsNr)
		    && !(Search.EscapeBits & FAHN_ALARM_GUARD)) {
		    register ubyte j, dir =
			livGetViewDirection(Planing_Name[i]);
		    register uword xpos = livGetXPos(Planing_Name[i]);
		    register uword ypos = livGetYPos(Planing_Name[i]);

#ifndef PLAN_IS_PERFECT
		    for (j = 0; j < BurglarsNr; j++) {
			if (tcGuardDetectsGuy
			    (Planing_GuardRoomList[i - BurglarsNr], xpos, ypos,
			     dir, Planing_Name[i], Planing_Name[j])) {
			    if ((PD.bldId == Building_Starford_Kaserne) && j)
				break;

			    plSay("PLAYER_GUARD_ALARM_1", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			    Search.DeriTime += PLANING_DERI_ALARM;
			    Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;

			    PD.guardKO[((Police)
					dbGetObject(OL_NR
						    (GetNthNode
						     (PersonsList,
						      i))))->LivingID -
				       BurglarsNr] = 3;
			    break;
			}
		    }
#endif
		}
#ifndef PLAN_IS_PERFECT
		if (!(PD.timer % 3) && (i < BurglarsNr)
		    && (livWhereIs(Planing_Name[i]) == lsGetActivAreaID())) {
		    if (PD.bldId != Building_Starford_Kaserne) {
			if (!(Search.EscapeBits & FAHN_ALARM_MICRO)
			    && tcAlarmByMicro(livGetXPos(Planing_Name[i]),
					      livGetYPos(Planing_Name[i]),
					      PD.currLoudness[i])) {
			    Search.DeriTime += PLANING_DERI_ALARM;
			    Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_MICRO;
			    plSay("PLAYER_MICRO", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			}
		    }
		}
#endif

		if (PD.action->Type != ACTION_GO) {
		    if (i < BurglarsNr) {
			if (PD.action->Type == ACTION_SIGNAL)
			    livAnimate(Planing_Name[i], ANM_MAKE_CALL, 0, 0);
			else
			    plWork(i);
		    } else
			livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0, 0);
		}

		switch (PD.action->Type) {
		case ACTION_GO:
		    if (i < BurglarsNr) {
			if (Search.Exhaust[i] > PLANING_EXHAUST_MAX)
			    UnableToWork(i, ACTION_EXHAUST);
			else {
			    plMove(i,
				   ActionData(PD.action,
					      struct ActionGo *)->Direction);

			    if (livCanWalk(Planing_Name[i])) {
				PD.currLoudness[i] = tcGetWalkLoudness();
				PD.unableToWork[i] = 0;

				Search.Exhaust[i] =
				    tcGuyInAction(OL_NR
						  (GetNthNode(PersonsList, i)),
						  Search.Exhaust[i]);
				Search.WalkTime[i]++;

				if (i == CurrentPerson) {
				    lsInitScrollLandScape(ActionData
							  (PD.action,
							   struct ActionGo *)->
							  Direction,
							  LS_SCROLL_PREPARE);
				    DoScrolling = 1;
				}
			    } else {
				if (livWhereIs(Planing_Name[i]) ==
				    lsGetActivAreaID())
				    UnableToWork(i, ACTION_GO);
			    }
			}
		    } else
			plMove(i,
			       ActionData(PD.action,
					  struct ActionGo *)->Direction);
		    break;

		case ACTION_WAIT:
		    if (i < BurglarsNr)
			UnableToWork(i, ACTION_WAIT);
		    break;

		case ACTION_SIGNAL:
		    if (i < BurglarsNr) {
			PD.currLoudness[i] = PLANING_LOUDNESS_RADIO;

			if (ActionStarted(plSys)) {
			    Search.CallCount++;

			    InitSignal(plSys, OL_NR(GetNthNode(PersonsList, i)),
				       ActionData(PD.action,
						  struct ActionSignal *)->
				       ReceiverId);

#ifndef PLAN_IS_PERFECT
			    if (tcAlarmByRadio(PD.bldObj))
				Search.EscapeBits |=
				    FAHN_QUIET_ALARM | FAHN_ALARM_RADIO;
#endif
			}

			if (ActionEnded(plSys)) {
			    register struct plSignal *sig =
				IsSignal(plSys,
					 OL_NR(GetNthNode(PersonsList, i)),
					 ActionData(PD.action,
						    struct ActionSignal *)->
					 ReceiverId);

			    if (sig)
				CloseSignal(sig);
			}
		    }
		    break;

		case ACTION_WAIT_SIGNAL:
		    if (i < BurglarsNr) {
			register struct plSignal *sig =
			    IsSignal(plSys,
				     ActionData(PD.action,
						struct ActionWaitSignal *)->
				     SenderId,
				     OL_NR(GetNthNode(PersonsList, i)));

			if (sig)
			    CloseSignal(sig);
			else
			    UnableToWork(i, ACTION_WAIT_SIGNAL);
		    }
		    break;

		case ACTION_USE:
		    if (i < BurglarsNr) {
			Search.WorkTime[i]++;
			Search.Exhaust[i] =
			    tcGuyInAction(OL_NR(GetNthNode(PersonsList, i)),
					  Search.Exhaust[i]);
			PD.unableToWork[i] = 0;

			if (ActionStarted(plSys)) {
			    if (plIsStair
				(ActionData(PD.action, struct ActionUse *)->
				 ItemId))
				 livLivesInArea(Planing_Name[i],
						StairConnectsGet(ActionData
								 (PD.action,
								  struct
								  ActionUse *)->
								 ItemId,
								 ActionData(PD.
									    action,
									    struct
									    ActionUse
									    *)->
								 ItemId));
			    else {
				register U32 needTime = PD.action->TimeNeeded;
				register U32 realTime = 0L;

				if (dbIsObject
				    (ActionData(PD.action, struct ActionUse *)->
				     ItemId, Object_Police)) {
				    if (plObjectInReach
					(i,
					 ActionData(PD.action,
						    struct ActionUse *)->
					 ItemId)) {
					PD.currLoudness[i] =
					    tcGetToolLoudness(OL_NR
							      (GetNthNode
							       (PersonsList,
								i)),
							      ActionData(PD.
									 action,
									 struct
									 ActionUse
									 *)->
							      ToolId,
							      Item_Wache);
					PD.guardKO[((Police)
						    dbGetObject(ActionData
								(PD.action,
								 struct
								 ActionUse *)->
								ItemId))->
						   LivingID - BurglarsNr] = 1;
					PD.changeCount += 5;

					realTime =
					    tcGuyUsesToolInPlayer(OL_NR
								  (GetNthNode
								   (PersonsList,
								    i)),
								  PD.bldObj,
								  ActionData(PD.
									     action,
									     struct
									     ActionUse
									     *)->
								  ToolId,
								  Item_Wache,
								  needTime /
								  PLANING_CORRECT_TIME);
					Search.KillTime[i] += realTime;
				    } else {
					Search.DeriTime +=
					    PLANING_DERI_GUARD_ESCAPED;
					IgnoreAction(plSys);

					plSay("PLAYER_IGNORE_1", i);
					inpSetWaitTicks
					    (INP_AS_FAST_AS_POSSIBLE);

					break;
				    }
				} else {
				    if (!CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (PD.action,
					   struct ActionUse *)->ItemId),
					 Const_tcLOCK_UNLOCK_BIT))
					 PD.currLoudness[i] =
					    tcGetToolLoudness(OL_NR
							      (GetNthNode
							       (PersonsList,
								i)),
							      ActionData(PD.
									 action,
									 struct
									 ActionUse
									 *)->
							      ToolId,
							      ((LSObject)
							       dbGetObject
							       (ActionData
								(PD.action,
								 struct
								 ActionUse *)->
								ItemId))->Type);
				    else if ((((LSObject)
					       dbGetObject(ActionData
							   (PD.action,
							    struct ActionUse
							    *)->ItemId))->
					      Type == Item_Fenster))
					 PD.currLoudness[i] =
					    PLANING_LOUDNESS_STD;
				    else if (!plIgnoreLock
					     (ActionData
					      (PD.action,
					       struct ActionUse *)->ItemId)) {
					Search.DeriTime +=
					    PLANING_DERI_IGNORE_ACTION;
					IgnoreAction(plSys);

					plSay("PLAYER_IGNORE_2", i);
					inpSetWaitTicks
					    (INP_AS_FAST_AS_POSSIBLE);

					break;
				    }
#ifndef PLAN_IS_PERFECT
				    if (tcGetDanger
					(OL_NR(GetNthNode(PersonsList, i)),
					 ActionData(PD.action,
						    struct ActionUse *)->ToolId,
					 ((LSObject)
					  dbGetObject(ActionData
						      (PD.action,
						       struct ActionUse *)->
						      ItemId))->Type))
					 PD.handlerEnded[i] = 2;
#endif

				    realTime =
					tcGuyUsesToolInPlayer(OL_NR
							      (GetNthNode
							       (PersonsList,
								i)), PD.bldObj,
							      ActionData(PD.
									 action,
									 struct
									 ActionUse
									 *)->
							      ToolId,
							      ((LSObject)
							       dbGetObject
							       (ActionData
								(PD.action,
								 struct
								 ActionUse *)->
								ItemId))->Type,
							      needTime /
							      PLANING_CORRECT_TIME);
				}

				if (realTime) {
				    Search.DeriTime +=
					((realTime * PLANING_CORRECT_TIME -
					  needTime) * 5) / PLANING_CORRECT_TIME;

				    IncCurrentTimer(plSys,
						    realTime *
						    PLANING_CORRECT_TIME -
						    needTime, 0);
				}

				plPersonLearns(OL_NR
					       (GetNthNode(PersonsList, i)),
					       ActionData(PD.action,
							  struct ActionUse *)->
					       ToolId);
			    }
			}
#ifndef PLAN_IS_PERFECT
			if (!(Search.EscapeBits & FAHN_ALARM_ALARM)
			    &&
			    tcAlarmByTouch(ActionData
					   (PD.action,
					    struct ActionUse *)->ItemId)) {
			    Search.DeriTime += PLANING_DERI_ALARM;
			    Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_ALARM;
			    plSay("PLAYER_ALARM", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			}
#endif

			if (ActionEnded(plSys)) {
			    if (!plIgnoreLock
				(ActionData(PD.action, struct ActionUse *)->
				 ItemId)) {
				if (plIsStair
				    (ActionData(PD.action, struct ActionUse *)->
				     ItemId)) {
				    if (i == CurrentPerson) {
					U32 newAreaId =
					    StairConnectsGet(ActionData
							     (PD.action,
							      struct ActionUse
							      *)->ItemId,
							     ActionData(PD.
									action,
									struct
									ActionUse
									*)->
							     ItemId);

					lsDoneActivArea(newAreaId);
					lsInitActivArea(newAreaId,
							livGetXPos(Planing_Name
								   [i]),
							livGetYPos(Planing_Name
								   [i]),
							Planing_Name[i]);

					if (lsGetStartArea() ==
					    lsGetActivAreaID())
					    lsShowEscapeCar();	/* Auto neu zeichnen */

					livRefreshAll();
				    }
				} else
				    if (dbIsObject
					(ActionData
					 (PD.action,
					  struct ActionUse *)->ItemId,
					 Object_Police)) {
				    if (tcKillTheGuard
					(OL_NR(GetNthNode(PersonsList, i)),
					 PD.bldId))
					PD.guardKO[((Police)
						    dbGetObject(ActionData
								(PD.action,
								 struct
								 ActionUse *)->
								ItemId))->
						   LivingID - BurglarsNr] = 2;
				    else {
					if (!
					    (Search.
					     EscapeBits & FAHN_ALARM_GUARD)) {
					    plSay("PLAYER_GUARD_ALARM_5",
						  ((Police)
						   dbGetObject(ActionData
							       (PD.action,
								struct ActionUse
								*)->ItemId))->
						  LivingID);
					    inpSetWaitTicks
						(INP_AS_FAST_AS_POSSIBLE);
					    Search.DeriTime +=
						PLANING_DERI_ALARM;
					    Search.EscapeBits |=
						FAHN_ALARM | FAHN_ALARM_GUARD;
					}

					PD.handlerEnded[i] = 32;
					PD.guardKO[((Police)
						    dbGetObject(ActionData
								(PD.action,
								 struct
								 ActionUse *)->
								ItemId))->
						   LivingID - BurglarsNr] = 3;
				    }
				} else {
				    if (!CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (PD.action,
					   struct ActionUse *)->ItemId),
					 Const_tcLOCK_UNLOCK_BIT)) {
					PD.changeCount++;

					lsSetObjectState(ActionData
							 (PD.action,
							  struct ActionUse *)->
							 ItemId,
							 Const_tcLOCK_UNLOCK_BIT,
							 1);

					if (((Tool)
					     dbGetObject(ActionData
							 (PD.action,
							  struct ActionUse *)->
							 ToolId))->
					    Effect & Const_tcTOOL_OPENS) {
					    lsSetObjectState(ActionData
							     (PD.action,
							      struct ActionUse
							      *)->ItemId,
							     Const_tcOPEN_CLOSE_BIT,
							     1);
					    plCorrectOpened((LSObject)
							    dbGetObject
							    (ActionData
							     (PD.action,
							      struct ActionUse
							      *)->ItemId), 1);
					}

					if (ActionData
					    (PD.action,
					     struct ActionUse *)->ToolId ==
					    Tool_Stechkarte)
					     tcRefreshTimeClock(PD.bldId,
								ActionData(PD.
									   action,
									   struct
									   ActionUse
									   *)->
								ItemId);
				    } else {
					if ((((LSObject)
					      dbGetObject(ActionData
							  (PD.action,
							   struct ActionUse *)->
							  ItemId))->Type ==
					     Item_Fenster)) {
					    uword xpos, ypos;

					    lsWalkThroughWindow((LSObject)
								dbGetObject
								(ActionData
								 (PD.action,
								  struct
								  ActionUse *)->
								 ItemId),
								livGetXPos
								(Planing_Name
								 [i]),
								livGetYPos
								(Planing_Name
								 [i]), &xpos,
								&ypos);
					    livSetPos(Planing_Name[i], xpos,
						      ypos);

					    livRefreshAll();
					}
				    }
				}
			    } else {
				register U32 state =
				    lsGetObjectState(ActionData
						     (PD.action,
						      struct ActionUse *)->
						     ItemId);

				if (CHECK_STATE(state, Const_tcON_OFF)) {
				    lsSetObjectState(ActionData(PD.action, struct ActionUse *)->ItemId, Const_tcON_OFF, 0);	/* on setzen  */

				    PD.changeCount--;

				    if (plIgnoreLock
					(ActionData
					 (PD.action,
					  struct ActionUse *)->ItemId) ==
					PLANING_POWER) {
					lsSetSpotStatus(ActionData
							(PD.action,
							 struct ActionUse *)->
							ItemId, LS_SPOT_ON);
					lsShowAllSpots(PD.realTime,
						       LS_ALL_VISIBLE_SPOTS);
				    }
				} else {
				    lsSetObjectState(ActionData(PD.action, struct ActionUse *)->ItemId, Const_tcON_OFF, 1);	/* off setzen */

				    PD.changeCount++;

				    switch (plIgnoreLock
					    (ActionData
					     (PD.action,
					      struct ActionUse *)->ItemId)) {
				    case PLANING_ALARM_TOP3:
					if (PD.alarmTimer)
					    PD.alarmTimer =
						min(PD.alarmTimer,
						    PLANING_ALARM_TOP3);
					else
					    PD.alarmTimer = PLANING_ALARM_TOP3;
					break;

				    case PLANING_POWER:
					lsSetSpotStatus(ActionData
							(PD.action,
							 struct ActionUse *)->
							ItemId, LS_SPOT_OFF);
					lsShowAllSpots(PD.realTime,
						       LS_ALL_INVISIBLE_SPOTS);

#ifndef PLAN_IS_PERFECT
					if (!
					    (Search.
					     EscapeBits & FAHN_ALARM_POWER)
					    &&
					    tcAlarmByPowerLoss(ActionData
							       (PD.action,
								struct ActionUse
								*)->ItemId)) {
					    Search.EscapeBits |=
						FAHN_ALARM | FAHN_ALARM_POWER;
					    Search.DeriTime +=
						PLANING_DERI_ALARM;
					    plSay("PLAYER_POWER", i);
					    inpSetWaitTicks
						(INP_AS_FAST_AS_POSSIBLE);
					}
#endif
					break;
				    }
				}
			    }

			    plRefresh(ActionData
				      (PD.action, struct ActionUse *)->ItemId);
			    plDisplayInfo();
			}
		    }
		    break;

		case ACTION_TAKE:
		    if (i < BurglarsNr) {
			PD.currLoudness[i] = PLANING_LOUDNESS_STD;
			Search.WorkTime[i]++;
			PD.unableToWork[i] = 0;

			if (ActionStarted(plSys)) {
			    PD.changeCount++;
			}

			if (Ask
			    (dbGetObject
			     (ActionData(PD.action, struct ActionTake *)->
			      ItemId), hasLoot(i),
			     dbGetObject(ActionData
					 (PD.action,
					  struct ActionTake *)->LootId))
			    && plObjectInReach(i,
					       ActionData(PD.action,
							  struct ActionTake *)->
					       ItemId)) {
			    lsSetObjectState(ActionData
					     (PD.action,
					      struct ActionTake *)->ItemId,
					     Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
			    if (!(Search.EscapeBits & FAHN_ALARM_ALARM)
				&&
				tcAlarmByTouch(ActionData
					       (PD.action,
						struct ActionUse *)->ItemId)) {
				Search.DeriTime += PLANING_DERI_ALARM;
				Search.EscapeBits |=
				    FAHN_ALARM | FAHN_ALARM_ALARM;
				plSay("PLAYER_ALARM", i);
				inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			    }
#endif

			    if (ActionEnded(plSys)) {
				U32 newValue =
				    GetP(dbGetObject
					 (ActionData
					  (PD.action,
					   struct ActionTake *)->ItemId),
					 hasLoot(i),
					 dbGetObject(ActionData
						     (PD.action,
						      struct ActionTake *)->
						     LootId));

				Planing_Weight[i] +=
				    ((Loot)
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionTake *)->
						 LootId))->Weight;
				Planing_Volume[i] +=
				    ((Loot)
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionTake *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionTake *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 0);

				if ((ActionData
				     (PD.action,
				      struct ActionTake *)->ItemId >= 9701)
				    &&
				    (ActionData
				     (PD.action,
				      struct ActionTake *)->ItemId <= 9708)) {
				    lsRemLootBag(ActionData
						 (PD.action,
						  struct ActionTake *)->ItemId);
				    Planing_Loot[ActionData
						 (PD.action,
						  struct ActionTake *)->ItemId -
						 9701] = 0;
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (PD.action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT)) {
					lsSetObjectState(ActionData
							 (PD.action,
							  struct ActionTake *)->
							 ItemId,
							 Const_tcACCESS_BIT, 0);
					lsTurnObject((LSObject)
						     dbGetObject(ActionData
								 (PD.action,
								  struct
								  ActionTake
								  *)->ItemId),
						     LS_OBJECT_INVISIBLE,
						     LS_NO_COLLISION);
				    }
				}

				UnSet(dbGetObject
				      (ActionData
				       (PD.action,
					struct ActionTake *)->ItemId),
				      hasLoot(i),
				      dbGetObject(ActionData
						  (PD.action,
						   struct ActionTake *)->
						  LootId));

				if (Ask
				    (dbGetObject
				     (OL_NR(GetNthNode(PersonsList, i))),
				     take_RelId,
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionTake *)->
						 LootId))) {
				    register U32 oldValue =
					GetP(dbGetObject
					     (OL_NR
					      (GetNthNode(PersonsList, i))),
					     take_RelId,
					     dbGetObject(ActionData
							 (PD.action,
							  struct ActionTake *)->
							 LootId));

				    SetP(dbGetObject
					 (OL_NR(GetNthNode(PersonsList, i))),
					 take_RelId,
					 dbGetObject(ActionData
						     (PD.action,
						      struct ActionTake *)->
						     LootId),
					 oldValue + newValue);
				} else
				    SetP(dbGetObject
					 (OL_NR(GetNthNode(PersonsList, i))),
					 take_RelId,
					 dbGetObject(ActionData
						     (PD.action,
						      struct ActionTake *)->
						     LootId), newValue);

				plRefresh(ActionData
					  (PD.action,
					   struct ActionTake *)->ItemId);
				plDisplayInfo();
			    }
			} else {
			    Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
			    IgnoreAction(plSys);

			    plSay("PLAYER_IGNORE_3", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			}
		    }
		    break;

		case ACTION_DROP:
		    if (i < BurglarsNr) {
			PD.currLoudness[i] = PLANING_LOUDNESS_STD;
			Search.WorkTime[i]++;
			PD.unableToWork[i] = 0;

			if (ActionStarted(plSys)) {
			    PD.changeCount++;
			}

			if (Ask
			    (dbGetObject(OL_NR(GetNthNode(PersonsList, i))),
			     take_RelId,
			     dbGetObject(ActionData
					 (PD.action,
					  struct ActionDrop *)->LootId))) {
			    lsSetObjectState(ActionData
					     (PD.action,
					      struct ActionDrop *)->ItemId,
					     Const_tcIN_PROGRESS_BIT, 1);

			    if (ActionEnded(plSys)) {
				U32 newValue =
				    GetP(dbGetObject
					 (OL_NR(GetNthNode(PersonsList, i))),
					 take_RelId,
					 dbGetObject(ActionData
						     (PD.action,
						      struct ActionDrop *)->
						     LootId));

				Planing_Weight[i] -=
				    ((Loot)
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionDrop *)->
						 LootId))->Weight;
				Planing_Volume[i] -=
				    ((Loot)
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionDrop *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionDrop *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);

				if ((ActionData
				     (PD.action,
				      struct ActionDrop *)->ItemId >= 9701)
				    &&
				    (ActionData
				     (PD.action,
				      struct ActionDrop *)->ItemId <= 9708)) {
				    lsAddLootBag(livGetXPos(Planing_Name[i]),
						 livGetYPos(Planing_Name[i]),
						 ActionData(PD.action,
							    struct ActionDrop
							    *)->ItemId - 9700);
				    Planing_Loot[ActionData
						 (PD.action,
						  struct ActionDrop *)->ItemId -
						 9701] = 1;
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (PD.action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT))
					 lsTurnObject((LSObject)
						      dbGetObject(ActionData
								  (PD.action,
								   struct
								   ActionDrop
								   *)->ItemId),
						      LS_OBJECT_VISIBLE,
						      LS_COLLISION);
				}

				SetP(dbGetObject
				     (ActionData
				      (PD.action, struct ActionDrop *)->ItemId),
				     hasLoot(i),
				     dbGetObject(ActionData
						 (PD.action,
						  struct ActionDrop *)->LootId),
				     newValue);
				UnSet(dbGetObject
				      (OL_NR(GetNthNode(PersonsList, i))),
				      take_RelId,
				      dbGetObject(ActionData
						  (PD.action,
						   struct ActionDrop *)->
						  LootId));

				plRefresh(ActionData
					  (PD.action,
					   struct ActionDrop *)->ItemId);
				plDisplayInfo();
			    }
			} else {
			    Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
			    IgnoreAction(plSys);
			}
		    }
		    break;

		case ACTION_OPEN:
		    if (ActionStarted(plSys)) {
			PD.unableToWork[i] = 0;

			if (i >= BurglarsNr) {
#ifndef PLAN_IS_PERFECT
			    if (CHECK_STATE
				(lsGetObjectState
				 (ActionData(PD.action, struct ActionOpen *)->
				  ItemId), Const_tcOPEN_CLOSE_BIT)) {
				if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
				    plSay("PLAYER_GUARD_ALARM_3", i);
				    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
				    Search.DeriTime += PLANING_DERI_ALARM;
				    Search.EscapeBits |=
					FAHN_ALARM | FAHN_ALARM_GUARD;
				}

				PD.guardKO[((Police)
					    dbGetObject(OL_NR
							(GetNthNode
							 (PersonsList,
							  i))))->LivingID -
					   BurglarsNr] = 3;

				break;
			    }
#endif
			}
		    }

		    if ((i >= BurglarsNr)
			||
			plIgnoreLock(ActionData
				     (PD.action, struct ActionOpen *)->ItemId)
			||
			CHECK_STATE(lsGetObjectState
				    (ActionData
				     (PD.action, struct ActionOpen *)->ItemId),
				    Const_tcLOCK_UNLOCK_BIT)) {
			if ((i >= BurglarsNr)
			    ||
			    plIgnoreLock(ActionData
					 (PD.action,
					  struct ActionOpen *)->ItemId)
			    ||
			    !CHECK_STATE(lsGetObjectState
					 (ActionData
					  (PD.action,
					   struct ActionOpen *)->ItemId),
					 Const_tcOPEN_CLOSE_BIT)) {
			    if (i < BurglarsNr) {
				PD.currLoudness[i] =
				    PLANING_LOUDNESS_OPEN_CLOSE;
				Search.WorkTime[i]++;
				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionOpen *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
				if (!(Search.EscapeBits & FAHN_ALARM_ALARM)
				    &&
				    tcAlarmByTouch(ActionData
						   (PD.action,
						    struct ActionUse *)->
						   ItemId)) {
				    Search.DeriTime += PLANING_DERI_ALARM;
				    Search.EscapeBits |=
					FAHN_ALARM | FAHN_ALARM_ALARM;
				    plSay("PLAYER_ALARM", i);
				    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
				}
#endif
			    }

			    if (ActionEnded(plSys)) {
				if (i < BurglarsNr) {
				    PD.changeCount++;
				    lsSetObjectState(ActionData
						     (PD.action,
						      struct ActionOpen *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);
				}

				if ((((LSObject)
				      dbGetObject(ActionData
						  (PD.action,
						   struct ActionOpen *)->
						  ItemId))->Type == Item_WC)
				    && (CalcRandomNr(0, 3) == 1)) {
				    sndPrepareFX("wc.voc");
				    sndPlayFX();
				}

				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionOpen *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 1);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (PD.action,
							     struct ActionOpen
							     *)->ItemId), 1);

				plRefresh(ActionData
					  (PD.action,
					   struct ActionOpen *)->ItemId);
				plDisplayInfo();
			    }
			} else {
			    Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
			    IgnoreAction(plSys);

			    plSay("PLAYER_IGNORE_4", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			}
		    } else
			UnableToWork(i, ACTION_OPEN);
		    break;

		case ACTION_CLOSE:
		    if (ActionStarted(plSys)) {
			PD.unableToWork[i] = 0;

			if (i >= BurglarsNr) {
#ifndef PLAN_IS_PERFECT
			    if (!CHECK_STATE
				(lsGetObjectState
				 (ActionData(PD.action, struct ActionOpen *)->
				  ItemId), Const_tcOPEN_CLOSE_BIT)) {
				if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
				    plSay("PLAYER_GUARD_ALARM_4", i);
				    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
				    Search.DeriTime += PLANING_DERI_ALARM;
				    Search.EscapeBits |=
					FAHN_ALARM | FAHN_ALARM_GUARD;
				}

				PD.guardKO[((Police)
					    dbGetObject(OL_NR
							(GetNthNode
							 (PersonsList,
							  i))))->LivingID -
					   BurglarsNr] = 3;

				break;
			    }
#endif
			}
		    }

		    if ((i >= BurglarsNr)
			||
			plIgnoreLock(ActionData
				     (PD.action, struct ActionClose *)->ItemId)
			||
			CHECK_STATE(lsGetObjectState
				    (ActionData
				     (PD.action, struct ActionOpen *)->ItemId),
				    Const_tcLOCK_UNLOCK_BIT)) {
			if ((i >= BurglarsNr)
			    ||
			    plIgnoreLock(ActionData
					 (PD.action,
					  struct ActionClose *)->ItemId)
			    ||
			    CHECK_STATE(lsGetObjectState
					(ActionData
					 (PD.action,
					  struct ActionClose *)->ItemId),
					Const_tcOPEN_CLOSE_BIT)) {
			    if (i < BurglarsNr) {
				PD.currLoudness[i] =
				    PLANING_LOUDNESS_OPEN_CLOSE;
				Search.WorkTime[i]++;
				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionClose *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);

#ifndef PLAN_IS_PERFECT
				if (!(Search.EscapeBits & FAHN_ALARM_ALARM)
				    &&
				    tcAlarmByTouch(ActionData
						   (PD.action,
						    struct ActionUse *)->
						   ItemId)) {
				    Search.DeriTime += PLANING_DERI_ALARM;
				    Search.EscapeBits |=
					FAHN_ALARM | FAHN_ALARM_ALARM;
				    plSay("PLAYER_ALARM", i);
				    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
				}
#endif
			    }

			    if (ActionEnded(plSys)) {
				if (i < BurglarsNr) {
				    PD.changeCount--;
				    lsSetObjectState(ActionData
						     (PD.action,
						      struct ActionClose *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);
				}

				lsSetObjectState(ActionData
						 (PD.action,
						  struct ActionClose *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 0);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (PD.action,
							     struct ActionClose
							     *)->ItemId), 0);

				plRefresh(ActionData
					  (PD.action,
					   struct ActionClose *)->ItemId);
				plDisplayInfo();
			    }
			} else {
			    Search.DeriTime += PLANING_DERI_IGNORE_ACTION;
			    IgnoreAction(plSys);

			    plSay("PLAYER_IGNORE_5", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			}
		    } else
			UnableToWork(i, ACTION_CLOSE);
		    break;

		case ACTION_CONTROL:
#ifndef PLAN_IS_PERFECT
		    if (tcGuardChecksObject
			((LSObject)
			 dbGetObject(ActionData
				     (PD.action,
				      struct ActionControl *)->ItemId))) {
			if (!(Search.EscapeBits & FAHN_ALARM_GUARD)) {
			    plSay("PLAYER_GUARD_ALARM_2", i);
			    inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
			    Search.DeriTime += PLANING_DERI_ALARM;
			    Search.EscapeBits |= FAHN_ALARM | FAHN_ALARM_GUARD;
			}

			PD.guardKO[((Police)
				    dbGetObject(OL_NR
						(GetNthNode(PersonsList, i))))->
				   LivingID - BurglarsNr] = 3;
		    }
#endif

		    if (((LSObject)
			 dbGetObject(ActionData(PD.action, struct ActionUse *)->
				     ItemId))->Type == Item_Stechuhr)
			 tcRefreshTimeClock(PD.bldId,
					    ActionData(PD.action,
						       struct ActionUse *)->
					    ItemId);
		    break;
		}
	    } else {
		PD.handlerEnded[i] = 1;
		livAnimate(Planing_Name[i], ANM_STAND, 0, 0);
	    }
	} else {
	    PD.currLoudness[i] = PLANING_LOUDNESS_STD;

	    if ((PD.handlerEnded[i] >= 2) && (PD.handlerEnded[i] <= 30)) {
		livAnimate(Planing_Name[i], ANM_ELEKTRO, 0, 0);
		PD.handlerEnded[i]++;
	    }

	    if (PD.handlerEnded[i] == 31) {
		plSay("PLAYER_VERLETZUNG", i);
		inpSetWaitTicks(INP_AS_FAST_AS_POSSIBLE);
		PD.handlerEnded[i]++;
	    }

	    if (PD.handlerEnded[i] == 32)
		livAnimate(Planing_Name[i], ANM_DUSEL, 0, 0);
	}
    }

    if (DoScrolling)
	lsScrollLandScape((ubyte) - 1);

    livDoAnims((AnimCounter++) % 2, 1);

    PD.ende = 1;

    for (i = 0; i < PLANING_NR_PERSONS; i++) {
	if (!PD.handlerEnded[i])
	    PD.ende = 0;
    }

    if (PD.ende)
	SetMenuTimeOutFunc(NULL);
}

S32 plPlayer(U32 objId, U32 actionTime, ubyte(*actionFunc) (U32, U32))
{
    FILE *fh = NULL;
    LIST *menu = txtGoKey(PLAN_TXT, "PLAYER_MENU"), *l;
    ubyte activ = 0, i;
    U32 timeLeft = 0, bitset, choice1, choice2;
    S32 ret = 0;

    plPrepareSys(0L, objId,
		 PLANING_INIT_PERSONSLIST | PLANING_HANDLER_ADD |
		 PLANING_HANDLER_OPEN | PLANING_GUARDS_LOAD |
		 PLANING_HANDLER_SET);
    plPrepareGfx(objId, LS_COLL_NORMAL,
		 PLANING_GFX_LANDSCAPE | PLANING_GFX_SPRITES |
		 PLANING_GFX_BACKGROUND);
    plPrepareRel();

    plPrepareData();

    AnimCounter = 0;

    if ((activ =
	 plOpen(objId, PLANING_OPEN_READ_BURGLARY, &fh)) == PLANING_OPEN_OK) {
	if (!(l = LoadSystem(fh, plSys))) {
	    if (!(l = plLoadTools(fh))) {
		PD.ende = 0;
		PD.timer = 0L;
		PD.realTime = 0L;
		PD.badPlaning = 0;
		PD.mood = plGetMood(0L);
		PD.bldId = objId;
		PD.bldObj = (Building) dbGetObject(objId);
		PD.changeCount = 0;
		PD.totalCount = lsGetObjectCount();
		PD.patrolCount = 0;
		PD.alarmTimer = 0L;
		PD.isItDark =
		    ((LSArea) dbGetObject(lsGetActivAreaID()))->uch_Darkness;
		PD.sndState = 1;
		PD.actionTime = actionTime;
		PD.actionFunc = actionFunc;

		/* Init data & search structure */
		for (i = 0; i < PLANING_NR_PERSONS; i++) {
		    PD.handlerEnded[i] = 1;
		    PD.currLoudness[i] = 0;
		    PD.unableToWork[i] = 0;

		    Search.Exhaust[i] = 0;

		    Search.WalkTime[i] = 0;
		    Search.WaitTime[i] = 0;
		    Search.WorkTime[i] = 0;
		    Search.KillTime[i] = 0;

		    Search.SpotTouchCount[i] = 0;
		}

		/* Init activ handler & load them */
		for (i = 0; i < PLANING_NR_PERSONS; i++) {
		    Search.GuyXPos[i] = -1;
		    Search.GuyYPos[i] = -1;
		}

		PD.maxTimer = 0L;

		for (i = 0; i < BurglarsNr; i++) {
		    PD.handlerEnded[i] = 0;
		    LoadHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));

		    PD.maxTimer = max(GetMaxTimer(plSys), PD.maxTimer);
		}

		for (i = 0; i < PLANING_NR_GUARDS; i++)
		    PD.guardKO[i] = 0;

		/* Init search structure */
		Search.BuildingId = objId;

		Search.TimeOfBurglary = 0;
		Search.TimeOfAlarm = 0;

		Search.DeriTime = 0;
		Search.CallValue = 0;
		Search.CallCount = 0;
		Search.WarningCount = 0;
		Search.EscapeBits = 0L;

		if (PD.bldId == Building_Starford_Kaserne)
		    sndPlaySound(PLANING_MUSIC_PLAYER_BEGIN_KASERNE, 0L);
		else
		    sndPlaySound(PLANING_MUSIC_PLAYER_BEGIN_STD, 0L);

                if (setup.Profidisk) {
		    if (objId == Building_Postzug) {
		        Environment env =
			    (Environment) dbGetObject(Environment_TheClou);

		        env->PostzugDone = 1;
		    }
                }

		/* start main loop */
		SetMenuTimeOutFunc(plPlayerAction);

		while (!PD.ende) {
		    bitset = BIT(PLANING_PLAYER_ESCAPE);

		    if ((CurrentPerson < BurglarsNr) ? BurglarsNr >
			1 : PersonsNr > 1)
			bitset += BIT(PLANING_PLAYER_PERSON_CHANGE);

		    if (has(Person_Matt_Stuvysunt, Tool_Funkgeraet))
			bitset += BIT(PLANING_PLAYER_RADIO_ALL) +
			    BIT(PLANING_PLAYER_RADIO_ONE);

		    plDisplayTimer(PD.realTime, 1);
		    plDisplayInfo();

		    ShowMenuBackground();

		    inpTurnFunctionKey(0);
		    inpTurnESC(0);

		    activ =
			Menu(menu, bitset, activ, NULL,
			     INP_AS_FAST_AS_POSSIBLE);

		    inpTurnFunctionKey(1);
		    inpTurnESC(1);

		    switch (activ) {
		    case PLANING_PLAYER_PERSON_CHANGE:
			plMessage("CHANGE_PERSON_2", PLANING_MSG_REFRESH);

			if (GamePlayMode & GP_GUARD_DESIGN) {
			    if (PersonsNr > 2)
				choice1 =
				    (U32) Bubble(PersonsList, CurrentPerson,
						 NULL, 0L);
			    else
				choice1 = ((CurrentPerson) ? 0L : 1L);
			} else {
			    if (BurglarsNr > 2)
				choice1 =
				    (U32) Bubble(BurglarsList, CurrentPerson,
						 NULL, 0L);
			    else
				choice1 = ((CurrentPerson) ? 0L : 1L);
			}

			if (choice1 != GET_OUT) {
			    if (livWhereIs(Planing_Name[choice1]) !=
				lsGetActivAreaID()) {
				lsDoneActivArea(livWhereIs
						(Planing_Name[choice1]));
				lsInitActivArea(livWhereIs
						(Planing_Name[choice1]),
						livGetXPos(Planing_Name
							   [choice1]),
						livGetYPos(Planing_Name
							   [choice1]),
						Planing_Name[choice1]);
			    }

			    plPrepareSys(choice1, 0, PLANING_HANDLER_SET);
			    lsSetActivLiving(Planing_Name[CurrentPerson],
					     (uword) - 1, (uword) - 1);
			}
			break;

		    case PLANING_PLAYER_RADIO_ALL:
			l = txtGoKey(PLAN_TXT, "PLAYER_RADIO_1");

			plMessage("PLAYER_RADIO_ALL", PLANING_MSG_REFRESH);

			SetPictID(((Person)
				   dbGetObject(OL_NR
					       (GetNthNode(BurglarsList, 0))))->
				  PictID);
			SetBubbleType(RADIO_BUBBLE);

			if ((choice1 = Bubble(l, 0, NULL, 0L)) != GET_OUT) {
			    if (choice1 < 2) {
				tcCalcCallValue(choice1, PD.realTime, 0L);
				Search.CallCount++;
			    }
			}

			RemoveList(l);
			l = NULL;
			break;

		    case PLANING_PLAYER_RADIO_ONE:
			if (BurglarsNr > 2) {
			    char exp[TXT_KEY_LENGTH];
			    NODE *node, *help;

			    plMessage("RADIO_1", PLANING_MSG_REFRESH);
			    SetPictID(((Person)
				       dbGetObject(OL_NR
						   (GetNthNode
						    (BurglarsList,
						     0))))->PictID);
			    node =
				UnLink(BurglarsList,
				       OL_NAME(GetNthNode(BurglarsList, 0)),
				       &help);

			    txtGetFirstLine(PLAN_TXT, "EXPAND_RADIO", exp);
			    ExpandObjectList(BurglarsList, exp);

			    choice1 = Bubble(BurglarsList, 0, NULL, 0L);

			    if (ChoiceOk(choice1, GET_OUT, BurglarsList))
				choice1 =
				    OL_NR(GetNthNode(BurglarsList, choice1));
			    else
				choice1 = GET_OUT;

			    Link(BurglarsList, node, help);
			    dbRemObjectNode(BurglarsList, 0L);
			} else
			    choice1 =
				CurrentPerson ?
				OL_NR(GetNthNode(BurglarsList, 0)) :
				OL_NR(GetNthNode(BurglarsList, 1));

			if (choice1 != GET_OUT) {
			    l = txtGoKey(PLAN_TXT, "PLAYER_RADIO_2");
			    SetPictID(((Person)
				       dbGetObject(OL_NR
						   (GetNthNode
						    (BurglarsList,
						     0))))->PictID);
			    SetBubbleType(RADIO_BUBBLE);

			    if ((choice2 =
				 (U32) Bubble(l, 0, NULL, 0L)) != GET_OUT) {
				if (choice2 < 3) {
				    tcCalcCallValue(choice2 + 2, PD.realTime,
						    choice1);
				    Search.CallCount++;
				}
			    }

			    RemoveList(l);
			    l = NULL;
			}
			break;

		    case PLANING_PLAYER_ESCAPE:
			Search.EscapeBits |= FAHN_ESCAPE;

			for (i = 0; i < PLANING_NR_PERSONS; i++)
			    PD.handlerEnded[i] = 1;
			break;

		    default:
			activ = 0;
			break;

		    }
		}

		SetMenuTimeOutFunc(NULL);
		inpSetWaitTicks(0);	/* normales Ausmaá */

		Search.LastAreaId = lsGetActivAreaID();
		Search.TimeOfBurglary = PD.realTime;

		if (!(Search.EscapeBits & FAHN_SURROUNDED)
		    && !(Search.EscapeBits & FAHN_ESCAPE)
		    && !plLivingsPosAtCar(objId)) {
		    plSay("PLAYER_NOT_FINISHED", 0);
		    Search.EscapeBits |= FAHN_ESCAPE;
		}

		for (i = 0; i < PersonsNr; i++)
		    livTurn(Planing_Name[i], LIV_DISABLED);	/* alle Sprites ausschalten! */

		for (i = 0; i < BurglarsNr; i++) {
		    register struct ObjectNode *n;

		    Search.GuyXPos[i] = livGetXPos(Planing_Name[i]);
		    Search.GuyYPos[i] = livGetYPos(Planing_Name[i]);

		    SetObjectListAttr(OLF_NORMAL, 0L);
		    AskAll(dbGetObject(OL_NR(GetNthNode(PersonsList, i))),
			   take_RelId, BuildObjectList);

		    for (n = (struct ObjectNode *) LIST_HEAD(ObjectList);
			 NODE_SUCC(n); n = (struct ObjectNode *) NODE_SUCC(n)) {
			if (has(Person_Matt_Stuvysunt, OL_NR(n))) {
			    U32 oldValue =
				hasGet(Person_Matt_Stuvysunt, OL_NR(n));
			    U32 newValue =
				GetP(dbGetObject
				     (OL_NR(GetNthNode(PersonsList, i))),
				     take_RelId, dbGetObject(OL_NR(n)));

			    hasSetP(Person_Matt_Stuvysunt, OL_NR(n),
				    oldValue + newValue);
			} else {
			    U32 newValue =
				GetP(dbGetObject
				     (OL_NR(GetNthNode(PersonsList, i))),
				     take_RelId, dbGetObject(OL_NR(n)));

			    hasSetP(Person_Matt_Stuvysunt, OL_NR(n), newValue);
			}
		    }
		}

		if (plCarTooFull()) {
		    plSay("PLAYER_LEAVE_LOOTS_1", 0);

		    while (plCarTooFull()) {
			U32 choice;

			hasAll(Person_Matt_Stuvysunt,
			       OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_NORMAL,
			       Object_Loot);

			if ((choice = Bubble(ObjectList, 0, 0L, 0L)) != GET_OUT)
			    hasUnSet(Person_Matt_Stuvysunt,
				     OL_NR(GetNthNode
					   (ObjectList, (U32) choice)));
			else
			    plSay("PLAYER_LEAVE_LOOTS_2", 0);
		    }
		}

		if ((!(Search.EscapeBits & FAHN_ALARM))
		    && (!(Search.EscapeBits & FAHN_QUIET_ALARM))
		    && (!(Search.EscapeBits & FAHN_ESCAPE)))
		    Search.EscapeBits |= FAHN_STD_ESCAPE;

		lsSetViewPort(0, 0);

		timeLeft = tcEscapeFromBuilding(Search.EscapeBits);

		ret = 1;
	    }
	}

	if (l) {
	    Bubble(l, 0, NULL, 0L);
	    RemoveList(l);
	}

	dskClose(fh);
    } else {
	if (activ == PLANING_OPEN_ERR_NO_PLAN)
	    plMessage("NO_PLAN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
    }

    if (Search.BuildingId == Building_Starford_Kaserne)
	Search.KaserneOk = tcIsLastBurglaryOk();

    plUnprepareRel();
    plUnprepareGfx();
    plUnprepareSys();

    RemoveList(menu);

    if (ret)
	return tcEscapeByCar(Search.EscapeBits, timeLeft);

    return 0;
}

/*
 * dataAppl.h
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * new animation module for the PANIC-System
 *
 * Rev   Date        Comment
 *  1   05-09-93
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_DATAAPPL
#define MODULE_DATAAPPL

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "data/datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data/dataapplh"
#endif

#include "data/objstd/tcdata.h"

#ifndef MODULE_EVIDENCE
#include "scenes/evidence.h"
#endif

#ifndef MODULE_LIVING
#include "living/living.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif


extern bool tcSpendMoney(U32 money, U8 breakAnim);

extern U32 GetObjNrOfLocation(U32 LocNr);
extern U32 GetObjNrOfBuilding(U32 LocNr);

extern void tcCalcCallValue(U32 callNr, U32 timer, U32 persId);
extern void tcPersonLearns(U32 pId);
extern void tcRefreshTimeClock(U32 buildId, U32 timerId);
extern U32 tcGetItemID(U32 itemType);
extern U32 tcGetPersOffer(Person person, U8 persCount);

extern S32 tcCalcEscapeTime(void);
extern bool tcKillTheGuard(U32 guyId, U32 buildingId);
extern bool tcAlarmByPowerLoss(U32 powerId);
extern bool tcAlarmByTouch(U32 lsoId);
extern bool tcGuardDetectsGuy(LIST * roomsList, U16 us_XPos, U16 us_YPos,
			      U8 uch_ViewDirection, char *puch_GuardName,
			      char *puch_LivingName);
extern S32 tcGetCarStrike(Car car);
extern S32 tcCalcEscapeOfTeam(void);
extern U32 tcGuyTellsAll(Person p);
extern U32 tcGuyCanEscape(Person p);
extern S32 tcGetCarTraderOffer(Car car);
extern S32 tcGetTeamMood(U32 * guyId, U32 timer);	/* ptr auf 4 U32s */
extern S32 tcGuyInAction(U32 persId, S32 exhaustion);
extern S32 tcGuyIsWaiting(U32 persId, S32 exhaustion);
extern S32 tcIsPlanPerfect(U32 timer);
extern S32 tcGetTrail(Person p, U8 which);
extern S32 tcGetDanger(U32 persId, U32 toolId, U32 itemId);
extern S32 tcGetToolLoudness(U32 persId, U32 toolId, U32 itemId);
extern S32 tcGetWalkLoudness(void);
extern S32 tcGetTotalLoudness(S32 loudp0, S32 loudp1, S32 loudp2, S32 loudp3);
extern bool tcAlarmByLoudness(Building b, S32 totalLoudness);
extern bool tcAlarmByRadio(Building b);
extern bool tcAlarmByMicro(U16 us_XPos, U16 us_YPos, S32 loudness);
extern bool tcAlarmByPatrol(U16 objChangedCount, U16 totalCount,
			    U8 patrolCount);
extern bool tcWatchDogWarning(U32 persId);
extern bool tcWrongWatchDogWarning(U32 persId);
extern S32 tcGetGuyState(U32 persId);
extern bool tcIsCarRecognised(Car car, U32 time);
extern bool tcGuardChecksObject(LSObject lso);
extern bool tcCheckTimeClocks(U32 builId);

extern S32 tcCalcMattsPart(void);

extern U32 tcGuyEscapes(void);
extern U32 tcGuyUsesTool(U32 persId, Building b, U32 toolId, U32 itemId);
extern U32 tcGuyUsesToolInPlayer(U32 persId, Building b, U32 toolId, U32 itemId,
				 U32 neededTime);

extern void tcInsertGuard(LIST * list, LIST * roomsList, U16 x, U16 y,
			  U16 width, U16 height, U32 guardId, U8 livId,
			  U32 areaId);

/* defines for Persons */
#define   tcPERSON_IS_ARRESTED          240	/* KnownToPolice */
#define   tcVALUE_OF_RING_OF_PATER      320

#define   tcSetPersKnown(p,v)           ((p)->Known = (v))
#define   tcChgPersPopularity(p, v)     ((p)->Popularity = (U8) ChangeAbs((p)->Popularity, v, 0, 255))

#define   tcGetPersHealth(p)            ((p)->Health)
#define   tcGetPersMood(p)              (((Person)p)->Mood)

#define   tcWeightPersCanCarry(p)       (((S32)(p)->Stamina + (S32)(p)->Strength) * 200)	/* 0 - 100000 (100 kg) in gramm ! */
#define   tcVolumePersCanCarry(p)       (((S32)(p)->Stamina + (S32)(p)->Strength) * 200)	/* in cm3 -> max 1 m3 fr eine Person */

#define   tcImprovePanic(p, v)          ((p)->Panic = (p)->Panic - ((p)->Panic / (v)))
#define   tcImproveKnown(p, v)          ((p)->Known = min(255, (v)))

/* defines for object Car */
#define   tcENGINE_PERC                 50	/* 50 % of costs of a car is engine   */
#define   tcBODY_PERC                   40	/* 40 % of costs of a car is bodywork */
#define   tcTYRE_PERC                   10	/* 10 % of costs of a car are tyres   */

#define   tcCalcCarState(car)           (car->State = ( (S32)(car->MotorState) + (S32)(car->BodyWorkState) + (S32)(car->TyreState)) /3)

#define   tcCostsPerEngineRepair(car)   (max(((car->Value * tcENGINE_PERC) / 25500), 2))
#define   tcCostsPerBodyRepair(car)     (max(((car->Value * tcBODY_PERC  ) / 25500), 2))
#define   tcCostsPerTyreRepair(car)     (max(((car->Value * tcTYRE_PERC  ) / 25500), 1))
#define   tcCostsPerTotalRepair(car)    (max(((car->Value) / 255), 5))
#define   tcColorCosts(car)             (max((Round(car->Value / 200,1)), 1))

#define   tcGetCarSpeed(car)            (car->Speed)
#define   tcGetCarPS(car)               (car->PS)

#define   tcRGetCarAge(car)             (max((GetDay /365) - (car->YearOfConstruction), 1))
#define   tcRGetCarValue(car)           (car->Value)

#define   tcGetCarPrice(car)            (Round(max(((car->Value) * (car->State)) / 255, 100), 1))
#define   tcGetCarTotalState(car)       (((S32)car->MotorState + (S32)car->BodyWorkState + (S32)car->TyreState) / 3)

#define   tcGetDealerPerc(d, p)         (min((p + (((S32)p * ((S32)127L - (S32)d->Known)) / (S32)1270)), 255))
#define   tcGetDealerOffer(v, p)        (((S32)v * (S32)p) / (S32)255)
#define   tcAddDealerSymp(d, v)         ((d)->Known = ChangeAbs ((d)->Known, v, 0, 255))

#define   tcSetCarMotorState(car,v)     {car->MotorState = (U8)ChangeAbs((S32)car->MotorState,(S32)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarBodyState(car,v)      {car->BodyWorkState = (U8)ChangeAbs((S32)car->BodyWorkState,(S32)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarTyreState(car,v)      {car->TyreState = (U8)ChangeAbs((S32)car->TyreState,(S32)v, 0, 255); tcCalcCarState(car);}

/* defines for object Player */
#define   tcGetPlayerMoney              (((Player)dbGetObject(Player_Player_1))->Money)
#define   tcSetPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = amount)
#define   tcAddPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = (tcGetPlayerMoney + amount))

/* defines for object Building */
#define   tcRGetGRate(bui)              (bui->GRate)
#define   tcRGetBuildStrike(bui)        (bui->Strike)

#define   tcGetBuildPoliceT(bui)        (bui->PoliceTime)
#define   tcGetBuildGRate(bui)          (bui->GRate)

U32 tcGetBuildValues(Building bui);

#define   tcAddBuildExactlyness(bui,v)  (bui->Exactlyness = (U8)ChangeAbs((S32)bui->Exactlyness,(S32)v, 0, 255))
#define   tcAddBuildStrike(bui,v)       (bui->Strike = (U8)ChangeAbs((S32)bui->Strike,(S32)v, 0, 255))

/* defines for object Tool */
#define   tcRGetDanger(tool)            (tool->Danger)
#define   tcRGetVolume(tool)            (tool->Volume)

#define   tcGetToolPrice(tool)          (tool->Value)
#define   tcGetToolTraderOffer(tool)    (max((tcGetToolPrice(tool) - (tcGetToolPrice(tool) / tcDerivation(10,20))), 1))

/* costs */
#define   tcCOSTS_FOR_HOTEL             10

/* Plan */
#define   tcEscapeTime(building)        (16400 / building->EscapeRouteLength)


#endif

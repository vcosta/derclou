/*
**	$Filename: story/story.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	story functions for "Der Clou!"
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

#ifndef MODULE_STORY
#define MODULE_STORY

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "present/interac.h"
#endif

#ifndef MODULE_PRESENT
#include "present/present.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog/dialog.h"
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
#include "data/dataappl.h"
#endif

#include "data/objstd/tcdata.h"

#ifndef MODULE_SCENES
#include "scenes/scenes.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "gameplay/gp.h"
#endif

#ifndef MODULE_GAMEPLAY_APP
#include "gameplay/gp_app.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_ORGANISATION
#include "organisa/organisa.h"
#endif

#ifndef MODULE_PLANING_PLANER
#include "planing/planer.h"
#endif

#ifndef MODULE_PLANING_PLAYER
#include "planing/player.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#ifndef MODULE_FX
#include "sound/fx.h"
#endif

#define tcLAST_BURGLARY_LEFT_CTRL_OBJ   440221L
#define tcLAST_BURGLARY_RIGHT_CTRL_OBJ  440228L

#define PHONE_PICTID    		     144
#define LETTER_PICTID         	  151

#define SCENE_NEW_GAME        45350912L
#define SCENE_CREDITS         46399488L

#define SCENE_GARAGE          16777216L
#define SCENE_PARKING         17039360L
#define SCENE_CARS_OFFICE      2621440L
#define SCENE_TOOLS            3145728L

#define SCENE_PARKER          19398656L
#define SCENE_MALOYA          17825792L
#define SCENE_POOLY           19136512L

#define SCENE_TOWER_OUT       15204352L
#define SCENE_AT_SABIEN       45088768L
#define SCENE_FREIFAHRT       21233664L
#define SCENE_ARRIVAL         22282240L
#define SCENE_STATION         21495808L
#define SCENE_HOTEL_REC       18612224L
#define SCENE_HOTEL_1ST_TIME  22544384L
#define SCENE_DANNER          22806528L
#define SCENE_GLUDO_MONEY     23068672L
#define SCENE_HOLLAND_STR     20185088L
#define SCENE_HOTEL           18612224L
#define SCENE_LISSON_GROVE    33292288L
#define SCENE_CARS_VANS       1048576L
#define SCENE_HOTEL_ROOM      2097152L
#define SCENE_FAT_MANS        2359296L
#define SCENE_MAMI_CALLS      23855104L
#define SCENE_FST_MEET_BRIGGS 24117248L
#define SCENE_CALL_FROM_POOLY 31457280L
#define SCENE_WALRUS          16515072L
#define SCENE_WATLING         20971520L
#define SCENE_THE_END          3932160L
#define SCENE_FAHNDUNG        24641536L
#define SCENE_1ST_BURG        24903680L
#define SCENE_GLUDO_SAILOR    25165824L
#define SCENE_CALL_BRIGGS     25427968L
#define SCENE_2ND_BURG        25690112L
#define SCENE_3RD_BURG        25952256L
#define SCENE_4TH_BURG        26214400L
#define SCENE_5TH_BURG        26476544L
#define SCENE_6TH_BURG        28573696L
#define SCENE_7TH_BURG        28835840L
#define SCENE_8TH_BURG        29097984L
#define SCENE_9TH_BURG        29360128L
#define SCENE_POLICE           3407872L
#define SCENE_ARRESTED_MATT   26738688L
#define SCENE_POOLY_AFRAID    27787264L
#define SCENE_MALOYA_AFRAID   28049408L
#define SCENE_PARKER_AFRAID   28311552L
#define SCENE_RAID            30932992L
#define SCENE_DART            31195136L
#define SCENE_MEETING_AGAIN   38273024L

#define SCENE_GLUDO_BURNS     27000832L
#define SCENE_MORNING         31981568L
#define SCENE_VISITING        32243712L
#define SCENE_A_DREAM         35127296L
#define SCENE_ROSENBLATT      35389440L
#define SCENE_BRIGGS_ANGRY    35651584L
#define SCENE_SABIEN_WALRUS   35913728L
#define SCENE_SABIEN_DINNER   36175872L
#define SCENE_TOMBOLA         36438016L
#define SCENE_PRESENT_HOTEL   36700160L
#define SCENE_INFO_TOWER      36962304L
#define SCENE_RAINY_EVENING   37224448L

#define SCENE_SABIEN_CALL     38010880L
#define SCENE_BIRTHDAY        38797312L
#define SCENE_WALK_WITH       39059456L
#define SCENE_AGENT           39321600L
#define SCENE_JAGUAR          44302336L
#define SCENE_THINK_OF        40370176L
#define SCENE_TERROR          40108032L
#define SCENE_CONFESSING      40632320L

#define SCENE_PRISON          44826624L

/* #define SCENE_SOUTH_WITH */
#define SCENE_SOUTH_WITHOUT   44040192L
#define SCENE_SOUTH_UNKNOWN   41418752L
#define SCENE_SOUTHHAMPTON    43778048L

#define SCENE_MISSED_DATE_0   37748736L
#define SCENE_MISSED_DATE_1   43515904L

#define SCENE_VILLA_INSIDE     8650752L
#define SCENE_KENWO_INSIDE     9175040L

#define SCENE_TANTE_INSIDE     5242880L
#define SCENE_JUWEL_INSIDE     5767168L
#define SCENE_ANTIQ_INSIDE     6291456L
#define SCENE_TRANS_INSIDE     6815744L
#define SCENE_SENIO_INSIDE     7340032L
#define SCENE_KENWO_INSIDE     9175040L
#define SCENE_HAMHO_INSIDE     9699328L
#define SCENE_OSTER_INSIDE    10223616L
#define SCENE_CHISW_INSIDE    10747904L
#define SCENE_SOTHE_INSIDE    11796480L
#define SCENE_BRITI_INSIDE    12320768L
#define SCENE_NATUR_INSIDE    12845056L
#define SCENE_NATIO_INSIDE    13369344L
#define SCENE_VICTO_INSIDE    13893632L
#define SCENE_BANKO_INSIDE    14417920L
#define SCENE_TOWER_INSIDE    14942208L

#define SCENE_KASERNE_INSIDE  47185920L
#define SCENE_KASERNE_OUTSIDE 47448064L

#define SCENE_PROFI_21        52428800
#define SCENE_PROFI_22        51380224
#define SCENE_PROFI_23        48496640
#define SCENE_PROFI_24        51904512
#define SCENE_PROFI_25        47972352
#define SCENE_PROFI_26        50855936
#define SCENE_PROFI_27        50069504
#define SCENE_PROFI_28        50593792

#define SCENE_PROFI_21_INSIDE 52166656
#define SCENE_PROFI_22_INSIDE 51118080
#define SCENE_PROFI_23_INSIDE 48234496
#define SCENE_PROFI_24_INSIDE 51642368
#define SCENE_PROFI_25_INSIDE 47710208
#define SCENE_PROFI_26_INSIDE	/* NO TRAIN INSIDE */
#define SCENE_PROFI_27_INSIDE 49807360
#define SCENE_PROFI_28_INSIDE 50331648

#define FACE_GLUDO_SAILOR     126
#define FACE_GLUDO_MAGIC      124

ubyte tcKarateOpa(U32 ul_ActionTime, U32 ul_BuildingId);

void tcTheAlmighty(U32 time);
void tcMattGoesTo(U32 locNr);
void tcMovePersons(U32 personCount, U32 time);
void tcMoveAPerson(U32 persID, U32 newLocID);
void tcAsTimeGoesBy(U32 untilMinute);
void tcAsDaysGoBy(U32 day, U32 stepSize);

void tcDoneFreeTicket(void);
void tcDoneMamiCalls(void);
void tcDoneArrival(void);
void tcDoneHotelReception(void);
void tcDoneDanner(void);
void tcDoneGludoMoney(void);
void tcDoneMeetBriggs(void);
void tcDoneAfterMeetBriggs(void);
void tcDone1stBurglary(void);
void tcDoneGludoAsSailor(void);
void tcDoneCallFromBriggs(void);
void tcDone2ndBurglary(void);
void tcDone3rdBurglary(void);
void tcDone4thBurglary(void);
void tcDone5thBurglary(void);
void tcDone6thBurglary(void);
void tcDone7thBurglary(void);
void tcDone8thBurglary(void);
void tcDone9thBurglary(void);
void tcDoneMattIsArrested(void);
void tcDoneGludoBurnsOffice(void);
void tcDoneDealerIsAfraid(void);
void tcDoneDartJager(void);
void tcDoneRaidInWalrus(void);
void tcDoneBeautifullMorning(void);
void tcDoneVisitingSabien(void);
void tcDoneADream(void);
void tcDoneCallFromPooly(void);
void tcMeetingRosenblatt(void);
void tcBriggsAngry(void);
void tcSabienInWalrus(void);
void tcSabienDinner(void);
void tcWalrusTombola(void);
void tcPresentInHotel(void);
void tcPoliceInfoTower(void);
void tcRainyEvening(void);
void tcDoneMissedDate(void);
void tcDoneSabienCall(void);
void tcDoneBirthday(void);
void tcWalkWithSabien(void);
void tcDoneAgent(void);
void tcDoneGoAndFetchJaguar(void);
void tcDoneThinkOfSabien(void);
void tcDoneTerror(void);
void tcDoneConfessingSabien(void);
void tcDoneSouthhamptonWithSabien(void);
void tcDoneSouthhamptonWithoutSabien(void);
void tcDoneSouthhamptonSabienUnknown(void);
void tcDoneSouthhampton(void);
void tcDonePrison(void);
void tcDoneMeetingAgain(void);
void tcInitTowerBurglary(void);
void tcDoneCredits(void);
void tcDoneMafia(void);
void tcDoneKaserne(void);
void tcInitPrison(void);

S32 tcIsLastBurglaryOk(void);
S32 tcDoTowerBurglary(void);
S32 tcIsDeadlock(void);
void tcCheckForBones(void);

U32 tcLastBurglary(void);

void tcCheckForDowning(void);

#endif

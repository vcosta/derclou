/*
**	$Filename: scenes/evidence.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-04-94
**
** functions for evidence for "Der Clou!"
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

#ifndef MODULE_EVIDENCE
#define MODULE_EVIDENCE

#include "theclou.h"

#include "list/list.h"
#include "text/text.h"
#include "dialog/dialog.h"
#include "present/interac.h"
#include "present/present.h"

#include "data/objstd/tcdata.h"

#include "data/relation.h"
#include "data/database.h"
#include "data/datacalc.h"
#include "data/dataappl.h"

#define FAHN_NOT_ESCAPED      1
#define FAHN_ESCAPED          2

#define FAHN_ALARM            (1)
#define FAHN_ALARM_LOUDN      (1<< 1)
#define FAHN_ALARM_RADIO      (1<< 2)
#define FAHN_ALARM_PATRO      (1<< 3)
#define FAHN_SURROUNDED       (1<< 4)
#define FAHN_ESCAPE           (1<< 5)
#define FAHN_QUIET_ALARM      (1<< 6)
#define FAHN_STD_ESCAPE       (1<< 7) /* wenn Einbruch glatt geht (Standardflucht) */
#define FAHN_ALARM_ALARM      (1<< 8)
#define FAHN_ALARM_TIMECLOCK  (1<< 9)
#define FAHN_ALARM_POWER      (1<<10)
#define FAHN_ALARM_TIMER      (1<<11)
#define FAHN_ALARM_MICRO      (1<<12)
#define FAHN_ALARM_GUARD      (1<<13)

struct Search {			/* wegen Arrays nicht in objects */
    word GuyXPos[4], GuyYPos[4];	/* letzte Position im Falle einer Flucht! */

    ubyte Exhaust[4];

    uword WalkTime[4];
    uword WaitTime[4];
    uword WorkTime[4];
    uword KillTime[4];

    U32 DeriTime;		/* Abweichung vom Soll */

    U32 TimeOfBurglary;		/* Zeitpunkt! */
    U32 TimeOfAlarm;		/* Zeitpunkt! */

    U32 BuildingId;		/* Einbruch */
    U32 LastAreaId;		/* zum Zeitpunkt der Flucht */

    U32 EscapeBits;

    word CallValue;		/* "Wert" der Funkspr., word!!! */
    uword CallCount;		/* Anzahl d. FUnkspr. */

    uword WarningCount;		/* Anzahl der Warnungen */
    ubyte SpotTouchCount[4];

    ubyte KaserneOk;		/* ob die Kaserne geschafft wurde! */
};

extern S32 tcCalcCarEscape(S32 timeLeft);
extern void tcForgetGuys(void);

extern struct Search Search;

#endif

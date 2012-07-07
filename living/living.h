/*
**	$Filename: living/living.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     13-04-94
**
**	"sprite" functions for "Der Clou!"
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

#ifndef MODULE_LIVING
#define MODULE_LIVING

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_BOB
#include "living/bob.h"
#endif

/* defines fÅr AnimPlayMode */
#define LIV_PM_NORMAL    	1
#define LIV_PM_REVERSE   	2

/* defines fÅr Action */
#define ANM_MOVE_UP 	   	((ubyte)0)
#define ANM_MOVE_DOWN	   ((ubyte)1)
#define ANM_MOVE_RIGHT	   ((ubyte)2)
#define ANM_MOVE_LEFT	   ((ubyte)3)
#define ANM_STAND          ((ubyte)9)
#define ANM_WORK_UP        ((ubyte)4)
#define ANM_WORK_DOWN      ((ubyte)5)
#define ANM_WORK_LEFT      ((ubyte)6)
#define ANM_WORK_RIGHT     ((ubyte)7)
#define ANM_DUSEL          ((ubyte)8)	/* for burglars */
#define ANM_ELEKTRO        ((ubyte)8)	/* for burglars */
#define ANM_WORK_CONTROL   ((ubyte)4)	/* for policemen */
#define ANM_DUSEL_POLICE   ((ubyte)5)	/* for policemen */

#define ANM_MAKE_CALL		((ubyte)9)	/* for burglars :MAKECALL */

#define LIV_ENABLED      	1
#define LIV_DISABLED     	0

extern void livInit(uword us_VisLScapeX, uword us_VisLScapeY,
		    uword us_VisLScapeWidth, uword us_VisLScapeHeight,
		    uword us_TotalLScapeWidth, uword us_TotalLScapeHeight,
		    ubyte uch_FrameCount, U32 ul_StartArea);

extern void livDone(void);

extern void livSetAllInvisible(void);

extern void livSetPos(char *uch_Name, uword XPos, uword YPos);
extern void livSetPlayMode(U32 playMode);
extern void livSetActivAreaId(U32 areaId);
extern void livRefreshAll(void);

extern void livLivesInArea(char *uch_Name, U32 areaId);
extern U32 livWhereIs(char *uch_Name);

extern uword livGetXPos(char *uch_Name);
extern uword livGetYPos(char *uch_Name);

extern ubyte livCanWalk(char *puch_Name);
extern ubyte livGetViewDirection(char *uch_Name);
extern ubyte livIsPositionInViewDirection(uword us_GXPos, uword us_GYPos,
					  uword us_XPos, uword us_YPos,
					  ubyte uch_ViewDirection);

extern void livTurn(char *puch_Name, ubyte uch_Status);	/* enabled or disabled */

extern void livSetVisLScape(uword us_VisLScapeX, uword us_VisLScapeY);

extern void livSetSpeed(char *uch_Name, word s_Speed);

extern void livAnimate(char *uch_Name, ubyte uch_Action, word s_XSpeed,
		       word s_YSpeed);
extern void livPrepareAnims(void);
extern void livDoAnims(ubyte uch_Play, ubyte uch_Move);
extern void livStopAll(void);

extern ubyte livGetOldAction(char *uch_Name);

/* uword spotInit(bitmap, uword 1 = 48, 2 = 32, 3 = 16 ); */
/* uword spotDone */
/* uword spotSetPos(uword, uword, uword); */

#endif

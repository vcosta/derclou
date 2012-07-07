/*
**	$Filename: landscap/spot.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     19-02-94
**
**	spot functions for "Der Clou!"
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

#ifndef MODULE_SPOT
#define MODULE_SPOT

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#define LS_SPOT_NAME					((ubyte*)"s")

#define LS_SPOT_SMALL_SIZE         16
#define LS_SPOT_MEDIUM_SIZE        32
#define LS_SPOT_LARGE_SIZE         48

#define LS_SPOT_ON                  1
#define LS_SPOT_OFF                 2

#define LS_ALL_VISIBLE_SPOTS        1
#define LS_ALL_INVISIBLE_SPOTS      2

struct SpotPosition {
    NODE Link;

    uword us_XPos;
    uword us_YPos;
};

struct Spot {
    NODE Link;

    uword us_Size;
    uword us_Speed;		/* secconds per move */

    U32 ul_CtrlObjId;		/* data : objId */

    U32 ul_AreaId;

    ubyte uch_Status;

    uword us_OldXPos;
    uword us_OldYPos;

    uword us_PosCount;

    LIST *p_positions;

    struct SpotPosition *p_CurrPos;	/* for fast access */
};

extern void lsInitSpots(void);
extern void lsDoneSpots(void);

extern void lsMoveAllSpots(U32 time);
extern void lsShowAllSpots(U32 time, U32 mode);
extern void lsShowSpot(struct Spot *s, U32 time);
extern void lsHideSpot(struct Spot *s);

extern void lsBlitSpot(uword us_Size, uword us_XPos, uword us_YPos,
		       ubyte visible);

extern void lsSetSpotStatus(U32 CtrlObjId, ubyte uch_Status);

extern void lsAddSpotPosition(struct Spot *spot, uword us_XPos, uword us_YPos);
extern void lsLoadSpots(U32 bldId, char *uch_FileName);
extern void lsWriteSpots(char *uch_FileName);
extern void lsFreeAllSpots(void);
extern void lsRemSpot(struct Spot *spot);

extern LIST *lsGetSpotList(void);

extern struct Spot *lsAddSpot(uword us_Size, uword us_Speed, U32 ul_CtrlObjId);
extern struct Spot *lsGetSpot(char *uch_Name);

#endif

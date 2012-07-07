/*
**      $Filename: planing/prepare.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.prepare interface for "Der Clou!"
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

#ifndef MODULE_PLANING_PREPARE
#define MODULE_PLANING_PREPARE

#include <stdio.h>
#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_ERROR
#include "error/error.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#include "data/objstd/tcdata.h"

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#ifndef MODULE_PLANING_MAIN
#include "planing/main.h"
#endif

#ifndef MODULE_PLANING_GRAPHICS
#include "planing/graphics.h"
#endif

#ifndef MODULE_PLANING_IO
#include "planing/io.h"
#endif

#ifndef MODULE_PLANING_PREPARE
#include "planing/prepare.h"
#endif

#ifndef MODULE_PLANING_SUPPORT
#include "planing/support.h"
#endif

#ifndef MODULE_PLANING_SYSTEM
#include "planing/system.h"
#endif

#ifndef MODULE_PLANING_SYNC
#include "planing/sync.h"
#endif

#ifndef MODULE_PLANING_GUARDS
#include "planing/guards.h"
#endif


/* Gfx prepare modes */
#define PLANING_GFX_LANDSCAPE  (1)
#define PLANING_GFX_SPRITES    (1<<1)
#define PLANING_GFX_BACKGROUND (1<<2)

/* Sys modes */
#define PLANING_INIT_PERSONSLIST    (1)
#define PLANING_HANDLER_OPEN        (1<<1)
#define PLANING_HANDLER_CLOSE       (1<<2)
#define PLANING_HANDLER_CLEAR       (1<<3)
#define PLANING_HANDLER_SET         (1<<4)
#define PLANING_HANDLER_ADD         (1<<5)
#define PLANING_GUARDS_LOAD         (1<<6)

/* Counts */
#define PLANING_NR_PERSONS          4
#define PLANING_NR_GUARDS           4
#define PLANING_NR_LOOTS            8
#define PLANING_NR_PLANS            NRBLINES-1

/* getting correct has loot relation */
#define hasLoot(current)            (((LSArea)dbGetObject(livWhereIs(Planing_Name[current])))->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET)


extern LIST *PersonsList;
extern LIST *BurglarsList;

extern ubyte PersonsNr;
extern ubyte BurglarsNr;
extern U32 CurrentPerson;

extern U32 Planing_Weight[PLANING_NR_PERSONS];
extern U32 Planing_Volume[PLANING_NR_PERSONS];

extern ubyte Planing_Loot[PLANING_NR_LOOTS];
extern ubyte Planing_Guard[PLANING_NR_GUARDS];

extern char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

extern LIST *Planing_GuardRoomList[PLANING_NR_GUARDS];

extern U32 Planing_BldId;



void plBuildHandler(NODE * n);
void plClearHandler(NODE * n);
void plCloseHandler(NODE * n);

void plPrepareData(void);
void plPrepareSprite(U32 livNr, U32 areaId);
void plPrepareNames(void);

void plPrepareGfx(U32 objId, ubyte landscapMode, ubyte prepareMode);
void plPrepareRel(void);
void plPrepareSys(U32 currPer, U32 objId, ubyte sysMode);

void plUnprepareGfx(void);
void plUnprepareRel(void);
void plUnprepareSys(void);
#endif

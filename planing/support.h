/*
**      $Filename: planing/support.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.support interface for "Der Clou!"
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

#ifndef MODULE_PLANING_SUPPORT
#define MODULE_PLANING_SUPPORT

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


/* Area defines */
#define PLANING_AREA_PERSON  3
#define PLANING_AREA_CAR     16

/* functions for bits */
#define BIT(x)               (UINT32_C(1)<<(x))
#define CHECK_STATE(v,b)     ((v) & BIT(b))

/* Alarms & power control */
#define PLANING_ALARM_Z3          3
#define PLANING_ALARM_X5          8
#define PLANING_ALARM_TOP3        15
#define PLANING_POWER             1

/* all times in 1/3 sec */
#define PLANING_CORRECT_TIME      3


U32 plGetNextLoot(void);

ubyte plLivingsPosAtCar(U32 bldId);
ubyte plAllInCar(U32 bldId);
ubyte plIsStair(U32 objId);

void plCorrectOpened(LSObject obj, ubyte open);
ubyte plIgnoreLock(U32 objId);

void plMove(U32 current, ubyte direction);
void plWork(U32 current);

LIST *plGetObjectsList(U32 current, ubyte addLootBags);
void plInsertGuard(LIST * list, U32 current, U32 guard);
ubyte plObjectInReach(U32 current, U32 objId);
#endif

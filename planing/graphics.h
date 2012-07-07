/*
**      $Filename: planing/graphics.h
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.graphics interface for "Der Clou!"
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

#ifndef MODULE_PLANING_GRAPHICS
#define MODULE_PLANING_GRAPHICS


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



/* Messages modes */
#define PLANING_MSG_REFRESH   1
#define PLANING_MSG_WAIT      2

/* RefreshRP */
#define PLANING_REFRESH_RP_WIDTH    200
#define PLANING_REFRESH_RP_HEIGHT   50
#define PLANING_REFRESH_RP_DEPTH    2

void plPrintInfo(char *person);
void plMessage(char *msg, ubyte flags);
void plPersonPict(U32 personId);
void plDisplayAbilities(void);
void plDisplayTimer(U32 time, ubyte doSpotsImmediatly);
void plDisplayInfo(void);
void plRefresh(U32 ItemId);
void plDrawWait(U32 sec);
ubyte plSay(char *msg, U32 persId);
#endif

/*
**      $Filename: planing/sync.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.sync interface for "Der Clou!"
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

#ifndef MODULE_PLANING_SYNC
#define MODULE_PLANING_SYNC

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

/* Sync modes */
#define PLANING_ANIMATE_NO     (1)
#define PLANING_ANIMATE_STD    (1<<1)
#define PLANING_ANIMATE_FOCUS  (1<<2)


void plSync(ubyte animate, U32 targetTime, U32 times, ubyte direction);
#endif

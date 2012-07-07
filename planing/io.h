/*
**      $Filename: planing/io.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.io interface for "Der Clou!"
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

#ifndef MODULE_PLANING_IO
#define MODULE_PLANING_IO

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


/* Extensions */
#define PLANING_PLAN_EXTENSION       ".pln"
#define PLANING_PLAN_LIST_EXTENSION  ".pll"

/* Disks */
#define PLANING_PLAN_DISK         	  0

/* Tool sequences in file */
#define PLANING_PLAN_TOOL_BEGIN_ID  "TOOB"	/* TOOl Begin */
#define PLANING_PLAN_TOOL_END_ID    "TOOE"	/* TOOl End */

/* open cmd */
#define PLANING_OPEN_READ_PLAN        0
#define PLANING_OPEN_WRITE_PLAN       1
#define PLANING_OPEN_READ_BURGLARY    2

/* open error */
#define PLANING_OPEN_OK               0
#define PLANING_OPEN_ERR_NO_CHOICE    1
#define PLANING_OPEN_ERR_NO_PLAN      2


ubyte plOpen(U32 objId, ubyte mode, FILE ** fh);
void plLoad(U32 objId);
void plSave(U32 objId);
void plSaveChanged(U32 objId);

LIST *plLoadTools(FILE * fh);
void plSaveTools(FILE * fh);
#endif

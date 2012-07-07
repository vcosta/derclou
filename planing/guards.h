/*
**      $Filename: planing/guards.h
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.guards interface for "Der Clou!"
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

#ifndef MODULE_PLANING_GUARDS
#define MODULE_PLANING_GUARDS

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

/* method definition */
#define GUARDS_DO_SAVE     1
#define GUARDS_DO_LOAD     2

/* disk definition */
#define GUARD_DISK         0
#define GUARD_EXTENSION    ".gua"
#define GUARD_DIRECTORY    DATA_DIRECTORY


struct System;

struct _GC;

/* main method */
void grdDo(FILE * fh, struct System *sys, LIST * PersonsList, U32 BurglarsNr,
	   U32 PersonsNr, ubyte grdAction);

/* support */
ubyte grdAddToList(U32 bldId, LIST * l);
ubyte grdDraw(struct _GC *gc, U32 bldId, U32 areaId);

/* con- & destructor */
ubyte grdInit(FILE ** fh, char *mode, U32 bldId, U32 areaId);
void grdDone(FILE * fh);
#endif

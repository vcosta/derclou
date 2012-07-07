/*
**	$Filename: dialog/talkAppl.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     07-04-94
**
**	dialog functions for "Der Clou!"
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

#ifndef MODULE_TALKAPPL
#define MODULE_TALKAPPL

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
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

extern void tcJobOffer(Person p);
extern void tcMyJobAnswer(Person p);
extern void tcPrisonAnswer(Person p);
extern void tcAbilityAnswer(U32 personID);

#endif

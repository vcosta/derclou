/*
**	$Filename: present/interac.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     05-04-94
**
**	interactiv presentations for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by K. Kazemi, H. Gaberschek
**	    All Rights Reserved.
**
*/
 /****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_INTERAC
#define MODULE_INTERAC

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#define X_OFFSET    112
#define NRBLINES    5U

#define BG_ACTIVE_COLOR   252
#define VG_ACTIVE_COLOR   254

#define BG_TXT_COLOR      252
#define VG_TXT_COLOR      254

#define BG_BAR_COLOR      230
#define VG_BAR_COLOR      230

#define TXT_MENU_TIMEOUT        (-2)

extern ubyte ChoiceOk(ubyte choice, ubyte exit, LIST * l);

extern ubyte Bubble(LIST * bubble, ubyte activ, void (*func) (ubyte),
		    U32 waitTime);
extern ubyte Menu(LIST * menu, U32 possibility, ubyte activ,
		  void (*func) (ubyte), U32 waitTime);

extern void RefreshMenu(void);

extern void SetBubbleType(uword type);
extern void SetPictID(uword PictID);
void SetMenuTimeOutFunc(void (*func) (void));

extern ubyte GetExtBubbleActionInfo(void);

#endif

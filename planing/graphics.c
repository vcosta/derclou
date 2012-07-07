/*
**      $Filename: planing/graphics.c
**      $Release:  1
**      $Revision: 0
**      $Date:     24-04-94
**
**      planing.graphics for "Der Clou!"
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

#include "base/base.h"

#include "planing/graphics.h"


void plPrintInfo(char *person)
{
    char info[80];

    sprintf(info, "%s", &person[1]);

    gfxSetRect(2, 320);
    gfxSetPens(m_gc, 249, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_gc, info, 12, GFX_PRINT_CENTER);
}

void plMessage(char *msg, ubyte flags)
{
    LIST *m = txtGoKey(PLAN_TXT, msg);

    if (flags & PLANING_MSG_REFRESH)
	ShowMenuBackground();

    if (m)
	plPrintInfo((char *) NODE_NAME(LIST_HEAD(m)));

    RemoveList(m);

    if (flags & PLANING_MSG_WAIT) {
	inpSetWaitTicks(140L);
	inpWaitFor(INP_LBUTTONP | INP_TIME);
	inpSetWaitTicks(0L);
    }
}

void plDisplayTimer(U32 time, ubyte doSpotsImmediatly)
{
    char info[80];
    U32 oldTimer = -1;

    if (!time)
	time = CurrentTimer(plSys) / PLANING_CORRECT_TIME;

    if (GamePlayMode & GP_GUARD_DESIGN) {
	sprintf(info, "x:%d, y:%d   %s %.2d:%.2d:%.2d %s",
		livGetXPos(Planing_Name[CurrentPerson]),
		livGetYPos(Planing_Name[CurrentPerson]), txtTimer,
		(U32) (time / 3600), (U32) ((time / 60) % 60),
		(U32) (time % 60), txtSeconds);

	gfxSetPens(m_gc, 0, 0, 0);
	gfxRectFill(m_gc, 120, 0, 320, 10);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
    } else {
	sprintf(info, "%s %.2d:%.2d:%.2d %s", txtTimer, (U32) (time / 3600),
		(U32) ((time / 60) % 60), (U32) (time % 60), txtSeconds);

	gfxSetPens(m_gc, 0, 0, 0);
	gfxRectFill(m_gc, 220, 0, 320, 10);

	gfxSetRect(2, 320);
	gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxPrint(m_gc, info, 2, GFX_PRINT_RIGHT);
    }

    if (doSpotsImmediatly || (oldTimer != CurrentTimer(plSys))) {
	oldTimer = CurrentTimer(plSys);
	lsMoveAllSpots(time);
    }
}

void plDisplayInfo(void)
{
    char info[80];

    dbGetObjectName(OL_NR(GetNthNode(PersonsList, CurrentPerson)), info);

    gfxSetPens(m_gc, 0, 0, 0);
    gfxRectFill(m_gc, 0, 0, 120, 10);

    gfxSetRect(2, 320);
    gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_gc, info, 2, GFX_PRINT_LEFT | GFX_PRINT_SHADOW);
}

ubyte plSay(char *msg, U32 persId)
{
    register LIST *l = txtGoKey(PLAN_TXT, msg);
    register ubyte choice;

    SetPictID(((Person) dbGetObject(OL_NR(GetNthNode(PersonsList, persId))))->
	      PictID);

    inpTurnESC(0);
    inpTurnFunctionKey(0);

    choice = Bubble(l, 0, NULL, 200);

    inpTurnFunctionKey(1);
    inpTurnESC(1);

    RemoveList(l);


    plDisplayTimer(0, 1);
    plDisplayInfo();

    return choice;
}

void plDrawWait(U32 sec)
{
    char time[10];

    sprintf(time, "%.2d:%.2d", (U32) (sec / 60), (U32) (sec % 60));

    gfxSetDrMd(m_gc, GFX_JAM_2);
    gfxSetPens(m_gc, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxSetRect(0, 320);
    gfxPrint(m_gc, time, 31, GFX_PRINT_CENTER);
    gfxSetDrMd(m_gc, GFX_JAM_1);
}

void plRefresh(U32 ItemId)
{
    register LSObject obj = (LSObject) dbGetObject(ItemId);

    if (lsIsLSObjectInActivArea(obj))
	lsFastRefresh(obj);

    if (lsGetStartArea() == lsGetActivAreaID())
	lsShowEscapeCar();	/* repaint car */

    lsRefreshAllLootBags();	/* repaint all LootBags */
}

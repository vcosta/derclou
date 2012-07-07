/*
**	$Filename: present/present.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	basic functions for "Der Clou!"
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

#include "present/present.h"
#include "present/present.ph"

struct PresentControl PresentControl = { NULL, 0, 0 };

#include "present/presenta.c"

void DrawPresent(LIST * present, U8 firstLine, GC *gc, U8 max)
{
    unsigned i, j, k;
    struct presentationInfo *p;
    char txt[70], s[10];

    gfxScreenFreeze();

    gfxSetPens(gc, 224, 224, 224);
    gfxRectFill(gc, 88, 3, 320, 49);

    if (firstLine) {
	gfxSetGC(gc);
	gfxShow(145, GFX_OVERLAY | GFX_NO_REFRESH, 0, 120, 5);
    }

    if ((max > (firstLine + NRBLINES))) {
	gfxSetGC(gc);
	gfxShow(146, GFX_OVERLAY | GFX_NO_REFRESH, 0, 120, 40);
    }

    for (i = firstLine, j = 4;
	 (max < firstLine + NRBLINES) ? i < max : i < firstLine + NRBLINES;
	 i++, j += 9) {
	p = (struct presentationInfo *) GetNthNode(present, i);

	strcpy(txt, NODE_NAME(p));

	switch (p->presentHow) {
	case PRESENT_AS_TEXT:
	    if (p->extendedText) {
		for (k = 0;
		     k <
		     (57 - strlen(NODE_NAME(p)) - strlen(p->extendedText)); k++)
		    strcat(txt, " ");

		strcat(txt, p->extendedText);
	    }
	    break;

	case PRESENT_AS_BAR:
	    gfxSetPens(gc, 250, 250, 251);
	    gfxRectFill(gc, 206, j, 316, j + 7);
	    gfxSetPens(gc, 251, 251, 251);
	    gfxRectFill(gc, 206, j,
			206 + ((316 - 206) * p->extendedNr) / p->maxNr, j + 7);

	    gfxSetPens(gc, 249, 252, 253);

	    gfxSetRect(206, 316 - 206);
	    sprintf(s, "%" PRIu32 " %%", (p->extendedNr * 100) / (p->maxNr));

	    gfxPrint(gc, s, j, GFX_PRINT_CENTER);
	    break;
	}

	gfxSetPens(gc, 252, 224, GFX_SAME_PEN);
	gfxPrintExact(gc, txt, 89, j + 1);

	gfxSetPens(gc, 254, 224, GFX_SAME_PEN);
	gfxPrintExact(gc, txt, 88, j);
    }

    gfxScreenThaw(gc, 88, 3, 228, 46);
}

U8 Present(U32 nr, char *presentationText,
	   void (*initPresentation) (U32, LIST *, LIST *))
{
    U8 firstVis = 0;
    U8 max = 0;

    U8 exit = 0;
    U32 action;

    LIST *presentationData = CreateList(), *list;

    SuspendAnim();

    gfxPrepareRefresh();

    inpTurnFunctionKey(0);

    if (dbIsObject(nr, Object_Person)) {
	Person obj = (Person) dbGetObject(nr);

	gfxShow(obj->PictID, GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP,
		0, -1, -1);
    } else if (dbIsObject(nr, Object_Evidence)) {
	Evidence e = dbGetObject(nr);

	gfxShow((uword) ((Person) dbGetObject(e->pers))->PictID,
		GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
    } else if (dbIsObject(nr, Object_Tool))
	gfxShow(((Tool) (dbGetObject(nr)))->PictID,
		GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);
    else if (dbIsObject(nr, Object_Loot))
	gfxShow(((Loot) (dbGetObject(nr)))->PictID,
		GFX_NO_REFRESH | GFX_OVERLAY | GFX_BLEND_UP, 0, -1, -1);

    gfxShow((uword) BIG_SHEET, GFX_NO_REFRESH | GFX_OVERLAY, 0, -1, -1);	/* nur die Farben ! */

    list = txtGoKey(PRESENT_TXT, presentationText);

    if (list)
	initPresentation(nr, presentationData, list);

    max = GetNrOfNodes(presentationData);

    gfxSetDrMd(u_gc, GFX_JAM_1);
    gfxSetFont(u_gc, bubbleFont);

    DrawPresent(presentationData, firstVis, u_gc, max);

    while (!exit) {
	action = inpWaitFor(INP_UP + INP_DOWN + INP_LBUTTONP + INP_RBUTTONP);

	if ((action & INP_ESC) || (action & INP_RBUTTONP))
	    exit = 1;

	if ((action & INP_LBUTTONP))
	    exit = 2;

	if ((action & INP_MOUSE)) {
	    U16 y;

	    gfxGetMouseXY(u_gc, NULL, &y);

	    while (y < 9 && firstVis > 0) {	/* Scroll up */
		firstVis--;
		DrawPresent(presentationData, firstVis, u_gc, max);

		gfxWaitTOS();

		gfxGetMouseXY(u_gc, NULL, &y);
	    }

	    while (y > 50 && y <= 59 && firstVis < (max - 5)) {	/* Scroll down */
		firstVis++;
		DrawPresent(presentationData, firstVis, u_gc, max);

		gfxWaitTOS();

		gfxGetMouseXY(u_gc, NULL, &y);
	    }
	} else {
	    if ((action & INP_UP)) {
		if (firstVis > 0) {
		    firstVis--;
		    DrawPresent(presentationData, firstVis, u_gc, max);
		}
	    }

	    if ((action & INP_DOWN)) {
		if ((max - NRBLINES > 0) && (firstVis < (max - NRBLINES))) {
		    firstVis++;
		    DrawPresent(presentationData, firstVis, u_gc, max);
		}
	    }
	}
    }

    RemoveList(list);
    RemoveList(presentationData);

    gfxRefresh();

    ContinueAnim();

    if (exit == 1)
	return GET_OUT;
    else
	return ((U8) (exit - 1));
}

static struct presentationInfo *AddPresentInfo(LIST * l, U32 max,
					       LIST * texts, U16 textNr)
{
    char *name = NULL;
    struct presentationInfo *p;

    if (textNr != ((U16) - 1))
	name = NODE_NAME(GetNthNode(texts, (U32) textNr));

    p = (struct presentationInfo *) CreateNode(l,
					       sizeof(struct presentationInfo),
					       name);

    p->maxNr = max;
    return p;
}

void AddPresentTextLine(LIST * l, const char *data, U32 max, LIST * texts,
			U16 textNr)
{
    struct presentationInfo *p;

    p = AddPresentInfo(l, max, texts, textNr);
    p->presentHow = PRESENT_AS_TEXT;

    if (data)
	strcpy(p->extendedText, data);
    else
	strcpy(p->extendedText, "");
}

/* XXX: ONLY FOR NUMBERIC VALUES! */
void AddPresentLine(LIST * l, U8 presentHow, U32 data, U32 max,
		    LIST * texts, U16 textNr)
{
    struct presentationInfo *p;

    p = AddPresentInfo(l, max, texts, textNr);
    p->presentHow = presentHow;

    if (presentHow == PRESENT_AS_TEXT)
	ErrorMsg(Internal_Error, ERROR_MODULE_PRESENT, 0);

    if (presentHow == PRESENT_AS_NUMBER) {
	sprintf(p->extendedText, "%" PRIu32, data);

	p->presentHow = PRESENT_AS_TEXT;
    }

    if (presentHow == PRESENT_AS_BAR)
	p->extendedNr = data;
}

void prSetBarPrefs(GC *gc, uword us_BarWidth,
		   uword us_BarHeight, ubyte uch_FCol, ubyte uch_BCol,
		   ubyte uch_TCol)
{
    PresentControl.gc = gc;
    PresentControl.us_BarWidth = us_BarWidth;
    PresentControl.us_BarHeight = us_BarHeight;
    PresentControl.uch_FCol = uch_FCol;
    PresentControl.uch_BCol = uch_BCol;
    PresentControl.uch_TCol = uch_TCol;
}

void prDrawTextBar(char *puch_Text, U32 ul_Value, U32 ul_Max,
		   uword us_XPos, uword us_YPos)
{
    GC *gc = PresentControl.gc;
    uword us_Width = PresentControl.us_BarWidth;
    uword us_Height = PresentControl.us_BarHeight;

    if (gc) {
	gfxSetRect(us_XPos, us_Width);
	gfxSetPens(gc, PresentControl.uch_BCol, GFX_SAME_PEN,
		   PresentControl.uch_FCol);

	gfxRectFill(gc, us_XPos, us_YPos, us_XPos + us_Width - 1,
		    us_YPos + us_Height - 1);

	us_Width = (uword) ((us_Width * ul_Value) / ul_Max);

	gfxSetPens(gc, PresentControl.uch_FCol, GFX_SAME_PEN, GFX_SAME_PEN);
	gfxRectFill(gc, us_XPos, us_YPos, us_XPos + us_Width - 1,
		    us_YPos + us_Height - 1);
	gfxSetPens(gc, PresentControl.uch_TCol, GFX_SAME_PEN, GFX_SAME_PEN);

	if (puch_Text) {
	    gfxSetDrMd(gc, GFX_JAM_1);
	    gfxPrint(gc, puch_Text, us_YPos + 2, GFX_PRINT_CENTER);
	}
    }
}

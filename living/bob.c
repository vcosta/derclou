/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "gfx/gfx.h"

#include "living/living.h"

#define	BOB_USED			1
#define	BOB_VISIBLE		2
#define	BOB_UPDATED		4

struct Bob {
    uword w, h;
    uword xsrc, ysrc;
    uword xdst, ydst;

    uword sx, sy;

    int flags;
};

#define	BOB_MAX		256

struct Bob list[BOB_MAX];

struct Bob bob_zero;

MemRastPort BobRP;

void gfxNCH4Refresh(void);

void BobInitLists(void)
{
    static bool init;

    if (!init) {
        gfxCollToMem(137, &BobRPInMem);
	init = true;
    }
}

static struct Bob *GetNthBob(uword BobID)
{
    if (BobID < BOB_MAX)
	return &list[BobID];
    else
	return NULL;
}

uword BobInit(uword width, uword height)
{
    struct Bob *bob;
    uword BobID;

    for (BobID = 0; BobID < BOB_MAX && list[BobID].flags != 0; BobID++);

    if (BobID < BOB_MAX) {
	bob = &list[BobID];

	*bob = bob_zero;
	bob->w = width;
	bob->h = height;
	bob->flags |= BOB_USED;
    }
    return BobID;
}

void BobDone(uword BobID)
{
    struct Bob *bob = GetNthBob(BobID);

    bob->flags = 0;
}

ubyte BobSet(uword BobID, uword xdst, uword ydst, uword xsrc, uword ysrc)
{
    struct Bob *bob = GetNthBob(BobID);

    bob->xsrc = xsrc;
    bob->ysrc = ysrc;

    bob->xdst = xdst;
    bob->ydst = ydst;
    return 1;
}

extern int ScrX, ScrY;

void BobVis(uword BobID)
{
    struct Bob *bob = GetNthBob(BobID);

    bob->flags |= BOB_VISIBLE;

    gfxNCH4Refresh();
}

void BobInVis(uword BobID)
{
    struct Bob *bob = GetNthBob(BobID);

    bob->flags &= ~BOB_VISIBLE;

    gfxNCH4Refresh();
}

void BobSetDarkness(ubyte darkness)
{
}

void BobDisplayLists(GC *gc)
{
    word i;

    gfxPrepareColl(137);

    for (i = 0; i < BOB_MAX; i++) {
	struct Bob *bob = &list[i];

	if ((bob->flags & BOB_VISIBLE)) {
            gfxBlit(gc, &BobRPInMem, bob->xsrc, bob->ysrc,
                    bob->xdst-ScrX, bob->ydst-ScrY, bob->w, bob->h, true);
	}
    }
}

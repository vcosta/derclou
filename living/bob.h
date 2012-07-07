/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef TC__BOB_H
#define TC__BOB_H

#include "SDL.h"

struct RastPort;

void BobInitLists(void);

uword BobInit(uword width, uword height);
void BobDone(uword BobID);

ubyte BobSet(uword BobID, uword xpos, uword ypos, uword xsrc, uword ysrc);

void BobVis(uword BobID);
void BobInVis(uword BobID);

void BobSetDarkness(ubyte darkness);
void BobDisplayLists(GC *gc);

#endif

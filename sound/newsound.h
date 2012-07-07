/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_NEWSOUND_H
#define TC_NEWSOUND_H

#include "theclou.h"

void sndInit(void);
void sndDone(void);

void sndPlaySound(char *name, U32 mode);
void sndStopSound(U8 dummy);
void sndFading(short int targetVol);

char *sndGetCurrSoundName(void);

#endif


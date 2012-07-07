/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef MODULE_CDROM
#define MODULE_CDROM

extern unsigned int CDRomInstalled;

#include "theclou.h"

int CDROM_Install(void);
void CDROM_UnInstall(void);

void CDROM_WaitForMedia(void);

void CDROM_PlayAudioSequence(U8 TrackNum, U32 StartOffset, U32 EndOffset);
void CDROM_StopAudioTrack(void);

#endif

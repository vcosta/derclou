/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_FX_H
#define TC_FX_H

#include "theclou.h"

#include "sound/buffer.h"

#define	SND_FREQUENCY				22050
#define SND_MAX_VOLUME                          255

#define SND_BUFFER_SIZE				65536
#define SND_SAMPLES                             1024


struct FXBase {
    uword us_AudioOk;

    SND_BUFFER *pSfxBuffer;
    SND_BUFFER *pMusicBuffer;
};

extern struct FXBase FXBase;

extern void sndInitFX(void);
extern void sndDoneFX(void);
extern void sndPrepareFX(const char *Name);
extern void sndPlayFX(void);

extern void InitAudio(void);
extern void RemoveAudio(void);

#endif


/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "SDL_mixer.h"

#include "disk/disk.h"

#include "sound/fx.h"
#include "sound/newsound.h"

#include "sound/hsc.h"

char currSoundName[DSK_PATH_MAX];


void sndInit(void)
{
    currSoundName[0] = '\0';

    hscInit();
}

void sndDone(void)
{
    SDL_LockAudio();

    hscDone();

    SDL_UnlockAudio();
}

void sndPlaySound(char *name, U32 mode)
{
    char path[DSK_PATH_MAX];

    if (strcmp(currSoundName, name) != 0) {
	strcpy(currSoundName, name);

	if (FXBase.us_AudioOk) {
	    dskBuildPathName(DISK_CHECK_FILE, SOUND_DIRECTORY, name, path);

            SDL_LockAudio();

	    hscLoad(path);

            SDL_UnlockAudio();
	}
    }
}

char *sndGetCurrSoundName(void)
{
    return currSoundName;
}


void sndFading(short int targetVol)
{
    if (FXBase.us_AudioOk) {
    }
}

void sndStopSound(U8 dummy)
{
    if (FXBase.us_AudioOk) {
        SDL_LockAudio();

        hscReset();

        SDL_UnlockAudio();
    }
}

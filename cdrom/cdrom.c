/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "cdrom/cdrom.h"

unsigned int CDRomInstalled = 0;

#include "SDL.h"
/*
static SDL_CD *cdrom;
*/
int CDROM_Install(void)
{/*
    if (SDL_InitSubSystem(SDL_INIT_CDROM) != 0) {
        printf("SDL_InitSubSystem: %s\n", SDL_GetError());
        return 0;
    }

    if (!(cdrom = SDL_CDOpen(0))) {
        printf("Couldn't open drive: %s\n", SDL_GetError());
        return 0;
    }

    CDRomInstalled = 1;*/
    return 1;
}

void CDROM_UnInstall(void)
{/*
    CDRomInstalled = 0;

    if (cdrom) {
        SDL_CDClose(cdrom);
        cdrom = NULL;
    }

    if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
        SDL_QuitSubSystem(SDL_INIT_CDROM);
    }*/
}

void CDROM_WaitForMedia(void)
{/*
    if (cdrom && CD_INDRIVE(SDL_CDStatus(cdrom))) {
    }*/
}

void CDROM_PlayAudioSequence(U8 TrackNum, U32 StartOffset, U32 EndOffset)
{/*
    if (cdrom && CD_INDRIVE(SDL_CDStatus(cdrom))) {
        SDL_CDPlayTracks(cdrom,TrackNum,StartOffset,1,EndOffset-StartOffset);
    }*/
}

void CDROM_StopAudioTrack(void)
{/*
    if (cdrom) {
        SDL_CDStop(cdrom);
    }*/
}

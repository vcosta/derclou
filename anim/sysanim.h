/*
 * newanim.c
 * (c) 1993 by Helmut Gaberschek & Kaweh Kazemi, ...and avoid panic by
 * All rights reserved.
 *
 * new animation module for the PANIC-System
 *
 * Rev   Date        Comment
 * 1     26.08.93    First implementation
 *
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/*
 * Aufbau der Anim.List Datei (Aufbau einer Zeile)
 *
 * 	PictureMode,PictsPerSecc,Pic1,AnimPic,PicAnzahl,Animphase
 *	Breite, Animphase H”he, Animphase offset, PlayMode
 *	XDest, YDest (als Offset zum 1. Bild)
 */

#ifndef MODULE_ANIM
#define MODULE_ANIM

#include "theclou.h"

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#include "gfx/gfx.h"

#ifndef MODULE_RANDOM
#include "random/random.h"
#endif

/* global functions */
extern void InitAnimHandler(void);
extern void CloseAnimHandler(void);

extern void PlayAnim(char *AnimID, U16 how_often, U32 mode);	/* -> docs vom 16.08.92 ! */
extern void StopAnim(void);

extern void GetAnim(char *AnimID, char *Dest);
extern void animator(void);

extern void SuspendAnim(void);
extern void ContinueAnim(void);

#endif

/*
**	$Filename: inphdl/inphdl.h
**	$Release:
**	$Revision:
**	$Date:
**
**	include file for input handling
**
**	(C) 1993 ...and avoid panic by
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/* defines fÅr  WaitFor */

#ifndef INPHDL_MODULE
#define INPHDL_MODULE

#include "theclou.h"

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_ANIM
#include "anim/sysanim.h"
#endif

#define INP_AS_FAST_AS_POSSIBLE		1

#define INP_UP                (1)
#define INP_DOWN              (1<<1)
#define INP_LEFT              (1<<2)
#define INP_RIGHT             (1<<3)
#define INP_ESC               (1<<4)   /* wird standardmÑ·ig gesetzt */

/* these two defines have been exchanged by kaweh */
#define INP_LBUTTONP          (1<<5)	/* left button pressed */
#define INP_LBUTTONR          (1<<6)	/* left button released */

#define INP_RBUTTONP          (1<<7)   /* right button pressed */
#define INP_RBUTTONR          (1<<8)   /* right button released */
#define INP_NO_ESC            (1<<10)  /* Esc Taste wird in WaitFor gesperrt */
#define INP_TIME              (1<<11)
#define INP_KEYBOARD          (1<<12)  /* look at IH.keyCode for further information */
#define INP_FUNCTION_KEY      (1<<13)
#define INP_SPACE             (1<<14)
#define INP_MOUSE             (1<<15)

#define INP_BUTTON      (INP_LBUTTONP+INP_RBUTTONP)
#define INP_MOVEMENT    (INP_UP+INP_DOWN+INP_LEFT+INP_RIGHT)
#define INP_ALL_MODES   (INP_UP+INP_DOWN+INP_LEFT+INP_RIGHT+INP_ESC+INP_LBUTTONP+INP_LBUTTONR+INP_RBUTTONP+INP_RBUTTONR+INP_TIME+INP_MOUSE+INP_KEYBOARD+INP_SPACE)

/* global functions */

extern void inpOpenAllInputDevs(void);
extern void inpCloseAllInputDevs(void);

extern void inpMousePtrOn(void);
extern void inpMousePtrOff(void);

extern S32 inpWaitFor(S32 l_Mask);	/* retourniert was passiert ist
					 * (siehe defines) */
extern void inpSetWaitTicks(U32 l_Ticks);

void inpTurnESC(bool us_NewStatus);	    /* 0 means off, 1 means on */
void inpTurnFunctionKey(bool us_NewStatus); /* 0 means off, 1 means on */
void inpTurnMouse(bool us_NewStatus);	    /* 0 means off, 1 means on */

extern void inpDelay(S32 l_Ticks);
extern void inpSetKeyRepeat(unsigned char rate);

extern void inpClearKbBuffer(void);

#endif

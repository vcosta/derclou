/*
**	$Filename: present\interac.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     05-04-94
**
**	interactiv presentation functions for "Der Clou!"
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

#define ACTIV_POSS   3L
#define INACTIV_POSS 2L
#define GHOSTED      1L

#define BG_INACTIVE_COLOR		252
#define VG_INACTIVE_COLOR		253

#define TXT_MAX_MENU_COLUMNS     8
#define TXT_1ST_MENU_LINE_Y     24
#define TXT_2ND_MENU_LINE_Y     36

#define TRUE				  1
#define FALSE				  0

#define INT_BUBBLE_WIDTH		 200

/* misc */
ubyte CurrentBubbleType = 3;
ubyte ExtBubbleActionInfo = 0;

uword ActivPersonPictID = MATT_PICTID;
uword MenuCoords[TXT_MAX_MENU_COLUMNS];

void (*MenuTimeOutFunc) (void);

/* refresh */
LIST *refreshMenu = NULL;
ubyte refreshActiv = 0;
U32 refreshPoss = 0;

/*
**	$Filename: organisa/organisa.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	functions for organisation of a burglary for "Der Clou!"
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

#ifndef MODULE_ORGANISATION
#define MODULE_ORGANISATION


#include "theclou.h"
#include "list/list.h"
#include "memory/memory.h"
#include "text/text.h"
#include "gfx/gfx.h"

/* Display Areas */

#define   ORG_DISP_ABILITIES   (1)
#define   ORG_DISP_TOOLS       (1<<1)

struct Organisation {
    U32 CarID;
    U32 DriverID;
    U32 BuildingID;

    ubyte GuyCount;
    ubyte PlacesInCar;

    struct RastPort *rp;
};

extern struct Organisation Organisation;

extern U32 tcOrganisation(void);
extern void tcResetOrganisation(void);

#endif

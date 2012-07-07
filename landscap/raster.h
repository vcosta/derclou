/*
**	$Filename: landscap/raster.h
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	landscape raster functions for "Der Clou!"
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

#ifndef MODULE_RASTER
#define MODULE_RASTER

#include "theclou.h"

#ifndef MODULE_DIALOG
#include "dialog/dialog.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#define LS_RASTER_DISP_WIDTH       320
#define LS_RASTER_DISP_HEIGHT      140

#define LS_RASTER_X_SIZE           4	/* Raster is used for collision */
#define LS_RASTER_Y_SIZE           4	/* dedection, in pixel */


NODE *lsGetSuccObject(NODE * start);
NODE *lsGetPredObject(NODE * start);
void lsFadeRasterObject(U32 areaID, LSObject lso, ubyte status);
void lsShowAllConnections(U32 areaID, NODE * node, ubyte perc);
void lsShowRaster(U32 areaID, ubyte perc);

uword lsGetRasterXSize(U32 areaID);
uword lsGetRasterYSize(U32 areaID);

#endif

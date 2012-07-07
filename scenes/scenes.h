/*
**	$Filename: scenes/scenes.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-04-94
**
**	some scenes for "Der Clou!"
**
**   (c) 1994 ...and avoid panic by, K. Kazemi, H. Gaberschek
**	    All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_SCENES
#define MODULE_SCENES

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "present/interac.h"
#endif

#ifndef MODULE_PRESENT
#include "present/present.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog/dialog.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "data/datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data/dataappl.h"
#endif

#include "data/objstd/tcdata.h"

#ifndef MODULE_GAMEPLAY_APP
#include "gameplay/gp_app.h"
#endif

#ifndef MODULE_RASTER
#include "landscap/raster.h"
#endif

extern uword CurrentBackground;

/* standard scene functions - look at scenes.c */

extern U32 Go(LIST * succ);
extern U32 tcTelefon(void);

extern void Look(U32 locNr);
extern void Information(void);
extern void tcWait(void);

/* taxi functions */

extern void AddTaxiLocation(U32 locNr);
extern void RemTaxiLocation(U32 locNr);

/* Done Funktionen */

void DoneHotelRoom(void);
void DoneTaxi(void);
void DoneGarage(void);
void DoneParking(void);
void DoneTools(void);
void DoneDealer(void);
void DoneInsideHouse(void);

void tcInitFahndung(void);
void tcDoneFahndung(void);

/* Hilfsfunktionen */

void Investigate(char *location);

void SetCarColors(ubyte index);

void tcSellCar(U32 ObjectID);
void tcColorCar(Car car);
void tcRepairCar(Car car, char *repairWhat);
void tcCarGeneralOverhoul(Car car);
void tcToolsShop(void);

S32 tcEscapeFromBuilding(U32 escBits);
S32 tcEscapeByCar(U32 escBits, S32 timeLeft);

U32 tcChooseCar(U32 backgroundNr);

LIST *tcMakeLootList(U32 containerID, U32 relID);

char *tcShowPriceOfCar(U32 nr, U32 type, void *data);
char *tcShowPriceOfTool(U32 nr, U32 type, void *data);

void tcDealerSays(Person dealer, ubyte textNr, S32 perc);
void tcDealerOffer(Person dealer, ubyte which);

void tcBuyCar(void);
void tcCarInGarage(U32 carID);

ubyte tcBuyTool(ubyte choice);
void tcSellTool(void);
ubyte tcShowTool(ubyte choice);
ubyte tcDescTool(ubyte choice);
void tcDealerDlg(void);

U32 tcStartEvidence(void);
U32 tcGoInsideOfHouse(U32 buildingID);

void tcShowLootInfo(Loot loot);
void tcInsideOfHouse(U32 buildingID, U32 areaID, ubyte perc);
void tcShowObjectData(U32 areaID, NODE * node, ubyte perc);

#endif

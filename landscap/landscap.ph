/*
**	$Filename: landscap\landscap.ph
**	$Release:  0
**	$Revision: 0.1
**	$Date:     10-02-94
**
**	landscap functions for "Der Clou!"
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

#define  LS_COLL16_MEM_RP       AnimRPInMem
#define  LS_COLL32_MEM_RP       AddRPInMem
#define  LS_COLL48_MEM_RP       LSObjectRPInMem

#define  LS_FLOOR_MEM_RP        LSFloorRPInMem

#define  LS_DOOR_REFRESH_MEM_RP LS_COLL16_MEM_RP

#define  LS_PREPARE_BUFFER      StdBuffer1
#define  LS_PREPARE_BUFFER_SIZE STD_BUFFER1_SIZE

#define  LS_LOOTBAG_BUFFER      StdBuffer1	// auch LIV_TEMPLATE_BUFFER „ndern!

#define  LS_LOOTBAG_X_OFFSET      0
#define  LS_LOOTBAG_Y_OFFSET    168

#define	LS_ESCAPE_CAR_X_OFFSET   14
#define  LS_ESCAPE_CAR_Y_OFFSET 168

/* folgenden defines bestimmen die Fl„che, die zu Kollisionskontrolle */
/* herangezogen wird , sie beziehen sich auf linke obere Ecke */

#define	LS_COLLIS_COLOR_0		  		1
#define	LS_COLLIS_COLOR_1		 		65
#define  LS_COLLIS_COLOR_2				0
#define  LS_COLLIS_COLOR_3				64

#define  LS_REFRESH_SHADOW_COLOR0	46
#define  LS_REFRESH_SHADOW_COLOR1  110

#define  FLOOR_DATA_EXTENSION     	(".lfd")
#define  OBJ_DATA_EXTENSION       	(".dat")
#define  OBJ_REL_EXTENSION        	(".rel")
#define  OBJ_GLOBAL_REL_EXTENSION 	(".gre")
#define  SPOT_DATA_EXTENSION      	(".spo")

/* some functions */
#define  LS_STD_COORDS            -30000	/* tritt nie auf! */

#define  FLOORS_PER_ROW_SBM  			10	/* in the Source Bitmap */
#define  WIDTH_OF_OBJECT_SBM 			288

/* collis defines */
#define  LS_COLLIS_LEFT           	1
#define  LS_COLLIS_RIGHT          	2
#define  LS_COLLIS_UP             	4
#define  LS_COLLIS_DOWN           	8

#define  LS_COLLIS_INVISIBLE			16	/* Subjekt sollte unsichtbar werden ! */

#define  LS_LEFT_BORDER_TOUCHED		1
#define  LS_RIGHT_BORDER_TOUCHED		2
#define  LS_UP_BORDER_TOUCHED			4
#define  LS_DOWN_BORDER_TOUCHED		8

#define  LS_SHOW_WALL                1
#define  LS_SHOW_OTHER_0             2
#define  LS_SHOW_OTHER_1             4
#define  LS_SHOW_REFRESH             8
#define  LS_SHOW_ALL						32
#define  LS_SHOW_DOOR					64
#define  LS_SHOW_SPECIAL				128

/* Floor defines ! */
#define  LS_FLOORS_PER_LINE       (LS_MAX_AREA_WIDTH/LS_FLOOR_X_SIZE)
#define  LS_FLOORS_PER_COLUMN     (LS_MAX_AREA_HEIGHT/LS_FLOOR_Y_SIZE)

#define  LS_NO_FLOOR(f)           ((f) & (1<<5))

#define  LS_IS_MICRO_ON_FLOOR(f)  ((f) & (1<<6))	/* wenn 7. Bit gesetzt */
#define  LS_SET_MICRO_ON_FLOOR(f) ((f) = ((f) | (1<<6)))
#define  LS_SET_OBJECT(f)         ((f) = ((f) | (1<<7)))

/* data fetch start */
#define	LS_VISIBLE_X_SIZE     	  320
#define  LS_VISIBLE_Y_SIZE         128

#define  LS_CENTER_X              (LS_VISIBLE_X_SIZE / 2)
#define  LS_CENTER_Y              (LS_VISIBLE_Y_SIZE / 2)

struct LandScape {
    U32 ul_BuildingID;
    U32 ul_AreaID;

    ubyte uch_ScrollSpeed;

    uword us_WindowXSize;	/* total Size of Window    */
    uword us_WindowYSize;

    uword us_WindowXPos;	/* linke, obere Ecke des des   */
    uword us_WindowYPos;	/* sichtbaren Windows ! */

    uword us_PersonXPos;	/* Offset zur linken, oberen Ecke */
    uword us_PersonYPos;	/* des sichtbaren Windows ! */

    ubyte uch_FloorsPerWindowColumn;	/* total window */
    ubyte uch_FloorsPerWindowLine;	/* total window */

    ubyte uch_CollMode;		/* "Plan" oder "echte Objekte", Ldesigner */

    char uch_ActivLiving[TXT_KEY_LENGTH];

    struct LSFloorSquare *p_CurrFloor;	/* holds floor and object information */
    struct LSFloorSquare *p_AllFloors[3];
    U32 ul_FloorAreaId[3];

    LIST *p_ObjectRetrieval;
    LIST *p_ObjectRetrievalLists[3];
    U32 ul_ObjectRetrievalAreaId[3];

    LIST *p_DoorRefreshList;

    uword us_LivingXSpeed;
    uword us_LivingYSpeed;

    word us_RasInfoScrollX;	/* bernimmt die Aufgabe der RasInfo Struct */
    word us_RasInfoScrollY;	/* des Amiga */

    uword us_EscapeCarBobId;

    uword us_DoorXOffset;
    uword us_DoorYOffset;

    ubyte uch_LivingAction;
    ubyte uch_ShowObjectMask;
};

struct LSFloorSquare {
    ubyte uch_FloorType;	/* 7 : object, 6 : Micro , siehe floor defines ! */
};

struct LSDoorRefreshNode	/* fr jede Tr existiert eine Node */
{
    NODE Link;

    LSObject lso;

    uword us_XOffset;
    uword us_YOffset;
};

extern struct LandScape *ls;

void lsRefreshStatue(LSObject lso);

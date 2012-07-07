/*
**	$Filename: data/database.c
**	$Release:  1
**	$Revision: 0
**	$Date:     09-03-94
**
**	database implementation for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**	All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

/* includes */
#include "base/base.h"

#include "data/database.h"
#include "data/database.ph"


/* public declarations */
LIST *ObjectList = NULL;
LIST *ObjectListPrivate = NULL;
U32 ObjectListWidth = 0L;
char *(*ObjectListPrevString) (U32, U32, void *) = NULL;
char *(*ObjectListSuccString) (U32, U32, void *) = NULL;


/* private declarations */
U32 ObjectListType = 0L;
U32 ObjectListFlags = OLF_NORMAL;

U8 ObjectLoadMode = DB_LOAD_MODE_STD;

LIST *objHash[OBJ_HASH_SIZE];
char decodeStr[11];


/* private functions - RELATION */
int dbCompare(KEY key1, KEY key2)
{
    return (key1 == key2);
}

char *dbDecode(KEY key)
{
    register struct dbObject *obj = dbGetObjectReal(key);

    sprintf(decodeStr, "%" PRIu32, obj->nr);

    return decodeStr;
}


KEY dbEncode(char *key)
{
    void *obj = dbGetObject(atol(key));

    if (!obj) {
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Could not find object <%" PRIu32 ">!", dbGetObjectReal(key)->nr);
    }

    return obj;
}

struct dbObject *dbFindRealObject(U32 realNr, U32 offset, U32 size)
{
    register struct dbObject *obj;
    register U8 objHashValue;

    for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	     NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	    if (obj->nr > offset) {
		if (size && (obj->nr > offset + size))
		    continue;

		if (obj->realNr == realNr)
		    return obj;
	    }
	}
    }

    return NULL;
}


/* public functions - OBJECTS */
void dbSetLoadObjectsMode(U8 mode)
{
    ObjectLoadMode = mode;
}

#define	DB_CHECK_SIZE(x) \
    if (sizeof(*(x)) != localSize) \
    { \
            DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE, \
            "Size mismatch (%d != %" PRIu32 ")", sizeof(*(x)), localSize); \
    }

static U32 dbGetStdDskSize(U32 type)
{
    switch (type) {
	/* tcMain */
    case Object_Person:
	return 25;
    case Object_Player:
	return 31;
    case Object_Car:
	return 24;
    case Object_Location:
	return 8;
    case Object_Ability:
	return 6;
    case Object_Item:
	return 22;
    case Object_Tool:
	return 9;
    case Object_Environment:
	return 7;
    case Object_London:
	return 1;
    case Object_Evidence:
	return 12;
    case Object_Loot:
	return 12;
    case Object_CompleteLoot:
	return 48;
    case Object_LSOLock:
	return 2;
    case Object_LSObject:
	return 21;

	/* tcBuild */
    case Object_Building:
	return 25;
    case Object_Police:
	return 3;
    case Object_LSArea:
	return 49;
    case Object_LSRoom:
	return 8;
    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown type #%" PRIu32, type);
	return 0;
    }
}

static U32 dbGetProfiDskSize(U32 type)
{
    switch (type) {
	/* tcMain */
    case Object_Person:
	return 25;
    case Object_Player:
	return 31;
    case Object_Car:
	return 24;
    case Object_Location:
	return 8;
    case Object_Ability:
	return 6;
    case Object_Item:
	return 22;
    case Object_Tool:
	return 9;
    case Object_Environment:
	return 8;
    case Object_London:
	return 1;
    case Object_Evidence:
	return 12;
    case Object_Loot:
	return 12;
    case Object_CompleteLoot:
	return 48;
    case Object_LSOLock:
	return 2;
    case Object_LSObject:
	return 21;

	/* tcBuild */
    case Object_Building:
	return 26;
    case Object_Police:
	return 3;
    case Object_LSArea:
	return 49;
    case Object_LSRoom:
	return 8;
    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown type #%" PRIu32, type);
	return 0;
    }
}

static U32 dbGetDskSize(U32 type)
{
    if (setup.Profidisk) {
        return dbGetProfiDskSize(type);
    } else {
        return dbGetStdDskSize(type);
    }
}


static U32 dbGetMemSize(U32 type)
{
    switch (type) {
	/* tcMain */
    case Object_Person:
	return Object_Person_Size;
    case Object_Player:
	return Object_Player_Size;
    case Object_Car:
	return Object_Car_Size;
    case Object_Location:
	return Object_Location_Size;
    case Object_Ability:
	return Object_Ability_Size;
    case Object_Item:
	return Object_Item_Size;
    case Object_Tool:
	return Object_Tool_Size;
    case Object_Environment:
	return Object_Environment_Size;
    case Object_London:
	return Object_London_Size;
    case Object_Evidence:
	return Object_Evidence_Size;
    case Object_Loot:
	return Object_Loot_Size;
    case Object_CompleteLoot:
	return Object_CompleteLoot_Size;
    case Object_LSOLock:
	return Object_LSOLock_Size;
    case Object_LSObject:
	return Object_LSObject_Size;

	/* tcBuild */
    case Object_Building:
	return Object_Building_Size;
    case Object_Police:
	return Object_Police_Size;
    case Object_LSArea:
	return Object_LSArea_Size;
    case Object_LSRoom:
	return Object_LSRoom_Size;
    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown type #%" PRIu32, type);
	return 0;
    }
}

static void
dbRWStdObject(void *obj, int RW, U32 type, U32 size, U32 localSize, FILE *fp)
{
    void (*U8_RW) (FILE * fp, U8 * x);
    void (*S8_RW) (FILE * fp, S8 * x);
    void (*U16LE_RW) (FILE * fp, U16 * x);
    void (*S16LE_RW) (FILE * fp, S16 * x);
    void (*U32LE_RW) (FILE * fp, U32 * x);
    void (*S32LE_RW) (FILE * fp, S32 * x);

    if (RW == 0) {
	U8_RW = dskRead_U8;
	S8_RW = dskRead_S8;
	U16LE_RW = dskRead_U16LE;
	S16LE_RW = dskRead_S16LE;
	U32LE_RW = dskRead_U32LE;
	S32LE_RW = dskRead_S32LE;
    } else {
	U8_RW = dskWrite_U8;
	S8_RW = dskWrite_S8;
	U16LE_RW = dskWrite_U16LE;
	S16LE_RW = dskWrite_S16LE;
	U32LE_RW = dskWrite_U32LE;
	S32LE_RW = dskWrite_S32LE;
    }

    switch (type) {
	/* tcMain */
    case Object_Person:
	{
	    Person x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Job = tmp;
	    } else {
		tmp = x->Job;
		(*U16LE_RW) (fp, &tmp);
	    }

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Sex = tmp;
	    } else {
		tmp = x->Sex;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*S8_RW) (fp, &x->Age);

	    (*U8_RW) (fp, &x->Health);
	    (*U8_RW) (fp, &x->Mood);

	    (*U8_RW) (fp, &x->Intelligence);
	    (*U8_RW) (fp, &x->Strength);
	    (*U8_RW) (fp, &x->Stamina);

	    (*U8_RW) (fp, &x->Loyality);
	    (*U8_RW) (fp, &x->Skill);
	    (*U8_RW) (fp, &x->Known);
	    (*U8_RW) (fp, &x->Popularity);
	    (*U8_RW) (fp, &x->Avarice);
	    (*U8_RW) (fp, &x->Panic);
	    (*U8_RW) (fp, &x->KnownToPolice);

	    (*U32LE_RW) (fp, &x->TalkBits);
	    (*U8_RW) (fp, &x->TalkFileID);

	    (*U8_RW) (fp, &x->OldHealth);
	}
	break;

    case Object_Player:
	{
	    Player x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->Money);

	    (*U32LE_RW) (fp, &x->StolenMoney);
	    (*U32LE_RW) (fp, &x->MyStolenMoney);

	    (*U8_RW) (fp, &x->NrOfBurglaries);
	    (*U8_RW) (fp, &x->JobOfferCount);

	    (*U8_RW) (fp, &x->MattsPart);

	    (*U32LE_RW) (fp, &x->CurrScene);
	    (*U32LE_RW) (fp, &x->CurrDay);
	    (*U32LE_RW) (fp, &x->CurrMinute);
	    (*U32LE_RW) (fp, &x->CurrLocation);
	}
	break;

    case Object_Car:
	{
	    Car x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Land = tmp;
	    } else {
		tmp = x->Land;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Value);
	    (*U16LE_RW) (fp, &x->YearOfConstruction);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->ColorIndex = tmp;
	    } else {
		tmp = x->ColorIndex;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U8_RW) (fp, &x->Strike);

	    (*U32LE_RW) (fp, &x->Capacity);
	    (*U8_RW) (fp, &x->PS);
	    (*U8_RW) (fp, &x->Speed);
	    (*U8_RW) (fp, &x->State);
	    (*U8_RW) (fp, &x->MotorState);
	    (*U8_RW) (fp, &x->BodyWorkState);
	    (*U8_RW) (fp, &x->TyreState);
	    (*U8_RW) (fp, &x->PlacesInCar);
	}
	break;

    case Object_Location:
	{
	    Location x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->LocationNr);
	    (*U16LE_RW) (fp, &x->OpenFromMinute);
	    (*U16LE_RW) (fp, &x->OpenToMinute);
	}
	break;

    case Object_Ability:
	{
	    Ability x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Name = tmp;
	    } else {
		tmp = x->Name;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Use);
	}
	break;

    case Object_Item:
	{
	    Item x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U16LE_RW) (fp, &x->OffsetFact);
	    (*U16LE_RW) (fp, &x->HExactXOffset);
	    (*U16LE_RW) (fp, &x->HExactYOffset);
	    (*U16LE_RW) (fp, &x->HExactWidth);
	    (*U16LE_RW) (fp, &x->HExactHeight);
	    (*U16LE_RW) (fp, &x->VExactXOffset);
	    (*U16LE_RW) (fp, &x->VExactYOffset);
	    (*U16LE_RW) (fp, &x->VExactWidth);
	    (*U16LE_RW) (fp, &x->VExactHeight);
	    (*U8_RW) (fp, &x->Size);
	    (*U8_RW) (fp, &x->ColorNr);
	}
	break;

    case Object_Tool:
	{
	    Tool x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);
	    (*U32LE_RW) (fp, &x->Value);
	    (*U8_RW) (fp, &x->Danger);
	    (*U8_RW) (fp, &x->Volume);
	    (*U8_RW) (fp, &x->Effect);
	}
	break;

    case Object_Environment:
	{
	    Environment x = obj;

	    DB_CHECK_SIZE(x);

	    (*U8_RW) (fp, &x->MattHasHotelRoom);
	    (*U8_RW) (fp, &x->MattHasIdentityCard);
	    (*U8_RW) (fp, &x->WithOrWithoutYou);
	    (*U8_RW) (fp, &x->MattIsInLove);
	    (*U8_RW) (fp, &x->SouthhamptonHappened);
	    (*U8_RW) (fp, &x->Present);
	    (*U8_RW) (fp, &x->FirstTimeInSouth);

            x->PostzugDone = 0;
	}
	break;

    case Object_London:
	{
	    London x = obj;

	    DB_CHECK_SIZE(x);

	    (*U8_RW) (fp, &x->Useless);
	}
	break;

    case Object_Evidence:
	{
	    Evidence x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->pers);
	    (*U8_RW) (fp, &x->Recognition);
	    (*U8_RW) (fp, &x->WalkTrail);
	    (*U8_RW) (fp, &x->WaitTrail);
	    (*U8_RW) (fp, &x->WorkTrail);
	    (*U8_RW) (fp, &x->KillTrail);
	    (*U8_RW) (fp, &x->CallTrail);
	    (*U8_RW) (fp, &x->PaperTrail);
	    (*U8_RW) (fp, &x->FotoTrail);
	}
	break;

    case Object_Loot:
	{
	    Loot x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Name = tmp;
	    } else {
		tmp = x->Name;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Volume);
	    (*U16LE_RW) (fp, &x->Weight);
	    (*U16LE_RW) (fp, &x->PictID);
	}
	break;

    case Object_CompleteLoot:
	{
	    CompleteLoot x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->Bild);
	    (*U32LE_RW) (fp, &x->Gold);
	    (*U32LE_RW) (fp, &x->Geld);
	    (*U32LE_RW) (fp, &x->Juwelen);
	    (*U32LE_RW) (fp, &x->Delikates);
	    (*U32LE_RW) (fp, &x->Statue);
	    (*U32LE_RW) (fp, &x->Kuriositaet);
	    (*U32LE_RW) (fp, &x->HistKunst);
	    (*U32LE_RW) (fp, &x->GebrauchsArt);
	    (*U32LE_RW) (fp, &x->Vase);
	    (*U32LE_RW) (fp, &x->TotalWeight);
	    (*U32LE_RW) (fp, &x->TotalVolume);
	}
	break;

    case Object_LSOLock:
	{
	    LSOLock x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }
	}
	break;

    case Object_LSObject:
	{
	    LSObject x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_OffsetFact);
	    (*U16LE_RW) (fp, &x->us_DestX);
	    (*U16LE_RW) (fp, &x->us_DestY);
	    (*U8_RW) (fp, &x->uch_ExactX);
	    (*U8_RW) (fp, &x->uch_ExactY);
	    (*U8_RW) (fp, &x->uch_ExactX1);
	    (*U8_RW) (fp, &x->uch_ExactY1);
	    (*U8_RW) (fp, &x->uch_Size);
	    (*U8_RW) (fp, &x->uch_Visible);
	    (*U8_RW) (fp, &x->uch_Chained);
	    (*U32LE_RW) (fp, &x->ul_Status);
	    (*U32LE_RW) (fp, &x->Type);
	}
	break;

	/* tcBuild */
    case Object_Building:
	{
	    Building x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->LocationNr);

	    (*U16LE_RW) (fp, &x->PoliceTime);
	    (*U16LE_RW) (fp, &x->GTime);

	    (*U8_RW) (fp, &x->Exactlyness);

	    (*U8_RW) (fp, &x->GRate);
	    (*U8_RW) (fp, &x->Strike);
	    (*U32LE_RW) (fp, &x->Values);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->EscapeRoute = tmp;
	    } else {
		tmp = x->EscapeRoute;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U8_RW) (fp, &x->EscapeRouteLength);
	    (*U8_RW) (fp, &x->RadioGuarding);
	    (*U8_RW) (fp, &x->MaxVolume);
	    (*U8_RW) (fp, &x->GuardStrength);

	    (*U16LE_RW) (fp, &x->CarXPos);
	    (*U16LE_RW) (fp, &x->CarYPos);

	    x->DiskId = 0;
	}
	break;

    case Object_Police:
	{
	    Police x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);
	    (*U8_RW) (fp, &x->LivingID);
	}
	break;

    case Object_LSArea:
	{
	    LSArea x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_Coll16ID);
	    (*U16LE_RW) (fp, &x->us_Coll32ID);
	    (*U16LE_RW) (fp, &x->us_Coll48ID);

	    (*U16LE_RW) (fp, &x->us_PlanColl16ID);
	    (*U16LE_RW) (fp, &x->us_PlanColl32ID);
	    (*U16LE_RW) (fp, &x->us_PlanColl48ID);

	    (*U16LE_RW) (fp, &x->us_FloorCollID);
	    (*U16LE_RW) (fp, &x->us_PlanFloorCollID);

	    (*U16LE_RW) (fp, &x->us_Width);
	    (*U16LE_RW) (fp, &x->us_Height);
	    (*U32LE_RW) (fp, &x->ul_ObjectBaseNr);

	    (*U8_RW) (fp, &x->uch_Darkness);

	    (*U16LE_RW) (fp, &x->us_StartX0);
	    (*U16LE_RW) (fp, &x->us_StartX1);
	    (*U16LE_RW) (fp, &x->us_StartX2);
	    (*U16LE_RW) (fp, &x->us_StartX3);
	    (*U16LE_RW) (fp, &x->us_StartX4);
	    (*U16LE_RW) (fp, &x->us_StartX5);

	    (*U16LE_RW) (fp, &x->us_StartY0);
	    (*U16LE_RW) (fp, &x->us_StartY1);
	    (*U16LE_RW) (fp, &x->us_StartY2);
	    (*U16LE_RW) (fp, &x->us_StartY3);
	    (*U16LE_RW) (fp, &x->us_StartY4);
	    (*U16LE_RW) (fp, &x->us_StartY5);
	}
	break;

    case Object_LSRoom:
	{
	    LSRoom x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_LeftEdge);
	    (*U16LE_RW) (fp, &x->us_TopEdge);
	    (*U16LE_RW) (fp, &x->us_Width);
	    (*U16LE_RW) (fp, &x->us_Height);
	}
	break;

    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown type #%" PRIu32, type);
	break;
    }
}

static void
dbRWProfiObject(void *obj, int RW, U32 type, U32 size, U32 localSize, FILE *fp)
{
    void (*U8_RW) (FILE * fp, U8 * x);
    void (*S8_RW) (FILE * fp, S8 * x);
    void (*U16LE_RW) (FILE * fp, U16 * x);
    void (*S16LE_RW) (FILE * fp, S16 * x);
    void (*U32LE_RW) (FILE * fp, U32 * x);
    void (*S32LE_RW) (FILE * fp, S32 * x);

    if (RW == 0) {
	U8_RW = dskRead_U8;
	S8_RW = dskRead_S8;
	U16LE_RW = dskRead_U16LE;
	S16LE_RW = dskRead_S16LE;
	U32LE_RW = dskRead_U32LE;
	S32LE_RW = dskRead_S32LE;
    } else {
	U8_RW = dskWrite_U8;
	S8_RW = dskWrite_S8;
	U16LE_RW = dskWrite_U16LE;
	S16LE_RW = dskWrite_S16LE;
	U32LE_RW = dskWrite_U32LE;
	S32LE_RW = dskWrite_S32LE;
    }

    switch (type) {
	/* tcMain */
    case Object_Person:
	{
	    Person x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Job = tmp;
	    } else {
		tmp = x->Job;
		(*U16LE_RW) (fp, &tmp);
	    }

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Sex = tmp;
	    } else {
		tmp = x->Sex;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*S8_RW) (fp, &x->Age);

	    (*U8_RW) (fp, &x->Health);
	    (*U8_RW) (fp, &x->Mood);

	    (*U8_RW) (fp, &x->Intelligence);
	    (*U8_RW) (fp, &x->Strength);
	    (*U8_RW) (fp, &x->Stamina);

	    (*U8_RW) (fp, &x->Loyality);
	    (*U8_RW) (fp, &x->Skill);
	    (*U8_RW) (fp, &x->Known);
	    (*U8_RW) (fp, &x->Popularity);
	    (*U8_RW) (fp, &x->Avarice);
	    (*U8_RW) (fp, &x->Panic);
	    (*U8_RW) (fp, &x->KnownToPolice);

	    (*U32LE_RW) (fp, &x->TalkBits);
	    (*U8_RW) (fp, &x->TalkFileID);

	    (*U8_RW) (fp, &x->OldHealth);
	}
	break;

    case Object_Player:
	{
	    Player x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->Money);

	    (*U32LE_RW) (fp, &x->StolenMoney);
	    (*U32LE_RW) (fp, &x->MyStolenMoney);

	    (*U8_RW) (fp, &x->NrOfBurglaries);
	    (*U8_RW) (fp, &x->JobOfferCount);

	    (*U8_RW) (fp, &x->MattsPart);

	    (*U32LE_RW) (fp, &x->CurrScene);
	    (*U32LE_RW) (fp, &x->CurrDay);
	    (*U32LE_RW) (fp, &x->CurrMinute);
	    (*U32LE_RW) (fp, &x->CurrLocation);
	}
	break;

    case Object_Car:
	{
	    Car x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Land = tmp;
	    } else {
		tmp = x->Land;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Value);
	    (*U16LE_RW) (fp, &x->YearOfConstruction);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->ColorIndex = tmp;
	    } else {
		tmp = x->ColorIndex;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U8_RW) (fp, &x->Strike);

	    (*U32LE_RW) (fp, &x->Capacity);
	    (*U8_RW) (fp, &x->PS);
	    (*U8_RW) (fp, &x->Speed);
	    (*U8_RW) (fp, &x->State);
	    (*U8_RW) (fp, &x->MotorState);
	    (*U8_RW) (fp, &x->BodyWorkState);
	    (*U8_RW) (fp, &x->TyreState);
	    (*U8_RW) (fp, &x->PlacesInCar);
	}
	break;

    case Object_Location:
	{
	    Location x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->LocationNr);
	    (*U16LE_RW) (fp, &x->OpenFromMinute);
	    (*U16LE_RW) (fp, &x->OpenToMinute);
	}
	break;

    case Object_Ability:
	{
	    Ability x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Name = tmp;
	    } else {
		tmp = x->Name;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Use);
	}
	break;

    case Object_Item:
	{
	    Item x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U16LE_RW) (fp, &x->OffsetFact);
	    (*U16LE_RW) (fp, &x->HExactXOffset);
	    (*U16LE_RW) (fp, &x->HExactYOffset);
	    (*U16LE_RW) (fp, &x->HExactWidth);
	    (*U16LE_RW) (fp, &x->HExactHeight);
	    (*U16LE_RW) (fp, &x->VExactXOffset);
	    (*U16LE_RW) (fp, &x->VExactYOffset);
	    (*U16LE_RW) (fp, &x->VExactWidth);
	    (*U16LE_RW) (fp, &x->VExactHeight);
	    (*U8_RW) (fp, &x->Size);
	    (*U8_RW) (fp, &x->ColorNr);
	}
	break;

    case Object_Tool:
	{
	    Tool x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);
	    (*U32LE_RW) (fp, &x->Value);
	    (*U8_RW) (fp, &x->Danger);
	    (*U8_RW) (fp, &x->Volume);
	    (*U8_RW) (fp, &x->Effect);
	}
	break;

    case Object_Environment:
	{
	    Environment x = obj;

	    DB_CHECK_SIZE(x);

	    (*U8_RW) (fp, &x->MattHasHotelRoom);
	    (*U8_RW) (fp, &x->MattHasIdentityCard);
	    (*U8_RW) (fp, &x->WithOrWithoutYou);
	    (*U8_RW) (fp, &x->MattIsInLove);
	    (*U8_RW) (fp, &x->SouthhamptonHappened);
	    (*U8_RW) (fp, &x->Present);
	    (*U8_RW) (fp, &x->FirstTimeInSouth);
	    (*U8_RW) (fp, &x->PostzugDone);
	}
	break;

    case Object_London:
	{
	    London x = obj;

	    DB_CHECK_SIZE(x);

	    (*U8_RW) (fp, &x->Useless);
	}
	break;

    case Object_Evidence:
	{
	    Evidence x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->pers);
	    (*U8_RW) (fp, &x->Recognition);
	    (*U8_RW) (fp, &x->WalkTrail);
	    (*U8_RW) (fp, &x->WaitTrail);
	    (*U8_RW) (fp, &x->WorkTrail);
	    (*U8_RW) (fp, &x->KillTrail);
	    (*U8_RW) (fp, &x->CallTrail);
	    (*U8_RW) (fp, &x->PaperTrail);
	    (*U8_RW) (fp, &x->FotoTrail);
	}
	break;

    case Object_Loot:
	{
	    Loot x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Name = tmp;
	    } else {
		tmp = x->Name;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U32LE_RW) (fp, &x->Volume);
	    (*U16LE_RW) (fp, &x->Weight);
	    (*U16LE_RW) (fp, &x->PictID);
	}
	break;

    case Object_CompleteLoot:
	{
	    CompleteLoot x = obj;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->Bild);
	    (*U32LE_RW) (fp, &x->Gold);
	    (*U32LE_RW) (fp, &x->Geld);
	    (*U32LE_RW) (fp, &x->Juwelen);
	    (*U32LE_RW) (fp, &x->Delikates);
	    (*U32LE_RW) (fp, &x->Statue);
	    (*U32LE_RW) (fp, &x->Kuriositaet);
	    (*U32LE_RW) (fp, &x->HistKunst);
	    (*U32LE_RW) (fp, &x->GebrauchsArt);
	    (*U32LE_RW) (fp, &x->Vase);
	    (*U32LE_RW) (fp, &x->TotalWeight);
	    (*U32LE_RW) (fp, &x->TotalVolume);
	}
	break;

    case Object_LSOLock:
	{
	    LSOLock x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->Type = tmp;
	    } else {
		tmp = x->Type;
		(*U16LE_RW) (fp, &tmp);
	    }
	}
	break;

    case Object_LSObject:
	{
	    LSObject x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_OffsetFact);
	    (*U16LE_RW) (fp, &x->us_DestX);
	    (*U16LE_RW) (fp, &x->us_DestY);
	    (*U8_RW) (fp, &x->uch_ExactX);
	    (*U8_RW) (fp, &x->uch_ExactY);
	    (*U8_RW) (fp, &x->uch_ExactX1);
	    (*U8_RW) (fp, &x->uch_ExactY1);
	    (*U8_RW) (fp, &x->uch_Size);
	    (*U8_RW) (fp, &x->uch_Visible);
	    (*U8_RW) (fp, &x->uch_Chained);
	    (*U32LE_RW) (fp, &x->ul_Status);
	    (*U32LE_RW) (fp, &x->Type);
	}
	break;

	/* tcBuild */
    case Object_Building:
	{
	    Building x = obj;
	    U16 tmp;

	    DB_CHECK_SIZE(x);

	    (*U32LE_RW) (fp, &x->LocationNr);

	    (*U16LE_RW) (fp, &x->PoliceTime);
	    (*U16LE_RW) (fp, &x->GTime);

	    (*U8_RW) (fp, &x->Exactlyness);

	    (*U8_RW) (fp, &x->GRate);
	    (*U8_RW) (fp, &x->Strike);
	    (*U32LE_RW) (fp, &x->Values);

	    if (RW == 0) {
		(*U16LE_RW) (fp, &tmp);
		x->EscapeRoute = tmp;
	    } else {
		tmp = x->EscapeRoute;
		(*U16LE_RW) (fp, &tmp);
	    }

	    (*U8_RW) (fp, &x->EscapeRouteLength);
	    (*U8_RW) (fp, &x->RadioGuarding);
	    (*U8_RW) (fp, &x->MaxVolume);
	    (*U8_RW) (fp, &x->GuardStrength);

	    (*U16LE_RW) (fp, &x->CarXPos);
	    (*U16LE_RW) (fp, &x->CarYPos);

	    (*U8_RW) (fp, &x->DiskId);
	}
	break;

    case Object_Police:
	{
	    Police x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->PictID);
	    (*U8_RW) (fp, &x->LivingID);
	}
	break;

    case Object_LSArea:
	{
	    LSArea x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_Coll16ID);
	    (*U16LE_RW) (fp, &x->us_Coll32ID);
	    (*U16LE_RW) (fp, &x->us_Coll48ID);

	    (*U16LE_RW) (fp, &x->us_PlanColl16ID);
	    (*U16LE_RW) (fp, &x->us_PlanColl32ID);
	    (*U16LE_RW) (fp, &x->us_PlanColl48ID);

	    (*U16LE_RW) (fp, &x->us_FloorCollID);
	    (*U16LE_RW) (fp, &x->us_PlanFloorCollID);

	    (*U16LE_RW) (fp, &x->us_Width);
	    (*U16LE_RW) (fp, &x->us_Height);
	    (*U32LE_RW) (fp, &x->ul_ObjectBaseNr);

	    (*U8_RW) (fp, &x->uch_Darkness);

	    (*U16LE_RW) (fp, &x->us_StartX0);
	    (*U16LE_RW) (fp, &x->us_StartX1);
	    (*U16LE_RW) (fp, &x->us_StartX2);
	    (*U16LE_RW) (fp, &x->us_StartX3);
	    (*U16LE_RW) (fp, &x->us_StartX4);
	    (*U16LE_RW) (fp, &x->us_StartX5);

	    (*U16LE_RW) (fp, &x->us_StartY0);
	    (*U16LE_RW) (fp, &x->us_StartY1);
	    (*U16LE_RW) (fp, &x->us_StartY2);
	    (*U16LE_RW) (fp, &x->us_StartY3);
	    (*U16LE_RW) (fp, &x->us_StartY4);
	    (*U16LE_RW) (fp, &x->us_StartY5);
	}
	break;

    case Object_LSRoom:
	{
	    LSRoom x = obj;

	    DB_CHECK_SIZE(x);

	    (*U16LE_RW) (fp, &x->us_LeftEdge);
	    (*U16LE_RW) (fp, &x->us_TopEdge);
	    (*U16LE_RW) (fp, &x->us_Width);
	    (*U16LE_RW) (fp, &x->us_Height);
	}
	break;

    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown type #%" PRIu32, type);
	break;
    }
}

static void
dbRWObject(void *obj, int RW, U32 type, U32 size, U32 localSize, FILE *fp)
{
    S32 start;


    start = ftell(fp);

    if (setup.Profidisk) {
        dbRWProfiObject(obj, RW, type, size, localSize, fp);
    } else {
        dbRWStdObject(obj, RW, type, size, localSize, fp);
    }

    if ((U32) (ftell(fp) - start) != size) {
	ErrorMsg(Disk_Defect, ERROR_MODULE_DATABASE, 1);
    }
}


U8 dbLoadAllObjects(char *fileName, U16 diskId)
{
    FILE *fh;

    if ((fh = dskOpen(fileName, "rb"))) {
	U32 realNr = 1;
	struct dbObjectHeader objHd;

	while (!feof(fh)) {
	    int ch;

	    objHd.nr = 0;
	    objHd.type = 0;
	    objHd.size = 0;

	    dskRead_U32LE(fh, &objHd.nr);
	    dskRead_U32LE(fh, &objHd.type);
	    dskRead_U32LE(fh, &objHd.size);

	    if ((objHd.nr != (U32) - 1) && (objHd.type != (U32) - 1)
		&& (objHd.size != (U32) - 1)) {
		LIST *list = NULL;
		void *obj;
		char *name = NULL;
		U32 localSize;

		if (ObjectLoadMode) {
		    if ((list = txtGoKey(OBJECTS_TXT, NULL)))	/* MOD: old version GoNextKey */
			name = NODE_NAME(LIST_HEAD(list));
		}

		localSize = dbGetMemSize(objHd.type);

		if (!
		    (obj =
		     dbNewObject(objHd.nr, objHd.type, localSize, name,
				 realNr++))) {
		    dskClose(fh);
		    dbDeleteAllObjects(0L, 0L);
		    return 0;
		}

		if (objHd.size != dbGetDskSize(objHd.type)) {
                    DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
                      "Expected #%" PRIu32 " of size %" PRIu32 " got %" PRIu32,
                      objHd.type, dbGetDskSize(objHd.type), objHd.size);
                }

		if (list)
		    RemoveList(list);

		dbRWObject(obj, 0, objHd.type, objHd.size, localSize, fh);
	    }

	    if ((ch = fgetc(fh)) != EOF)
		ungetc(ch, fh);
	}

	dskClose(fh);
	return 1;
    }

    return 0;
}

U8 dbSaveAllObjects(char *fileName, U32 offset, U32 size, U16 diskId)
{
    FILE *fh;
    register struct dbObject *obj;
    register U32 realNr = 1;
    register U32 dbSize = dbGetObjectCountOfDB(offset, size);

    if ((fh = dskOpen(fileName, "wb"))) {
	while (realNr <= dbSize) {
	    if ((obj = dbFindRealObject(realNr++, offset, size))) {
		struct dbObjectHeader objHd;
		U32 localSize;

		objHd.nr = obj->nr;
		objHd.type = obj->type;
		objHd.size = dbGetDskSize(obj->type);

		localSize = NODE_SIZE(obj) - sizeof(struct dbObject);

		dskWrite_U32LE(fh, &objHd.nr);
		dskWrite_U32LE(fh, &objHd.type);
		dskWrite_U32LE(fh, &objHd.size);

		dbRWObject(obj + 1, 1, objHd.type, objHd.size, localSize, fh);
	    }
	}

	dskClose(fh);
	return 1;
    }

    return 0;
}

void dbDeleteAllObjects(U32 offset, U32 size)
{
    register struct dbObject *obj, *pred;
    register U8 objHashValue;

    for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
	for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	     NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	    if (obj->nr > offset) {
		if (size && (obj->nr > offset + size))
		    continue;

		pred = (struct dbObject *) NODE_PRED(obj);
		RemNode(obj);	/* MOD: old version Remove */
		FreeNode(obj);
		obj = pred;
	    }
	}
    }
}

U32 dbGetObjectCountOfDB(U32 offset, U32 size)
{
    U32 count = 0;
    struct dbObject *obj;
    U8 i;

    for (i = 0; i < OBJ_HASH_SIZE; i++) {
	for (obj = (struct dbObject *) LIST_HEAD(objHash[i]); NODE_SUCC(obj);
	     obj = (struct dbObject *) NODE_SUCC(obj)) {
	    if ((obj->nr > offset) && (obj->nr < (offset + size)))
		count++;
	}
    }

    return count;
}


/* public functions - OBJECT */
void *dbNewObject(U32 nr, U32 type, U32 size, char *name, U32 realNr)
{
    register struct dbObject *obj;
    register U8 objHashValue = dbGetObjectHashNr(nr);

    if (!
	(obj =
	 (struct dbObject *) CreateNode(objHash[objHashValue],
					sizeof(struct dbObject) + size, name)))
	return NULL;

    obj->nr = nr;
    obj->type = type;
    obj->realNr = realNr;

    return dbGetObjectKey(obj);
}

void dbDeleteObject(U32 nr)
{
    register struct dbObject *obj = NULL;
    register U8 objHashValue = dbGetObjectHashNr(nr);

    for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	 NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	if (obj->nr == nr) {
	    UnSetAll((KEY) (obj + 1), NULL);
	    RemNode(obj);
	    FreeNode(obj);
	    return;
	}
    }
}

void *dbGetObject(U32 nr)
{
    register struct dbObject *obj;
    register U8 objHashValue = dbGetObjectHashNr(nr);

    for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	 NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	if (obj->nr == nr)
	    return dbGetObjectKey(obj);
    }

    return NULL;
}

U32 dbGetObjectNr(void *key)
{
    return dbGetObjectReal(key)->nr;
}

char *dbGetObjectName(U32 nr, char *objName)
{
    register struct dbObject *obj;
    register U8 objHashValue = dbGetObjectHashNr(nr);

    for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	 NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	if (obj->nr == nr) {
	    strcpy(objName, NODE_NAME(obj));
	    return objName;
	}
    }

    return NULL;
}

void *dbIsObject(U32 nr, U32 type)
{
    register struct dbObject *obj;
    register U8 objHashValue = dbGetObjectHashNr(nr);

    for (obj = (struct dbObject *) LIST_HEAD(objHash[objHashValue]);
	 NODE_SUCC(obj); obj = (struct dbObject *) NODE_SUCC(obj)) {
	if (obj->nr == nr) {
	    if (obj->type == type)
		return dbGetObjectKey(obj);

	    break;
	}
    }

    return NULL;
}

/* public prototypes - OBJECTNODE */
struct ObjectNode *dbAddObjectNode(LIST * objectList, U32 nr, U32 flags)
{
    struct ObjectNode *n = NULL;
    struct dbObject *obj = dbGetObjectReal(dbGetObject(nr));
    char name[TXT_KEY_LENGTH], *namePtr;

    name[0] = EOS;

    if (flags & OLF_INSERT_STAR)
	strcpy(name, "*");

    if (flags & OLF_INCLUDE_NAME) {
	char *succString = NULL;

	if ((flags & OLF_ADD_PREV_STRING) && ObjectListPrevString)
	    strcat(name,
		   ObjectListPrevString(obj->nr, obj->type,
					dbGetObjectKey(obj)));

	strcat(name, NODE_NAME(obj));
	namePtr = name;

	if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
	    succString =
		ObjectListSuccString(obj->nr, obj->type, dbGetObjectKey(obj));

	if ((flags & (OLF_ADD_SUCC_STRING | OLF_ALIGNED)) && ObjectListWidth) {
	    register U8 i, len = strlen(name) + strlen(succString);

	    if (flags & OLF_INSERT_STAR)
		len--;

	    for (i = len; i < ObjectListWidth; i++)
		strcat(name, " ");
	}

	if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
	    strcat(name, succString);
    } else
	namePtr = NULL;

    if ((n =
	 (struct ObjectNode *) CreateNode(objectList, sizeof(*n), namePtr))) {
	n->nr = obj->nr;
	n->type = obj->type;
	n->data = dbGetObjectKey(obj);
    }

    return n;
}

void dbRemObjectNode(LIST * objectList, U32 nr)
{
    struct ObjectNode *n = dbHasObjectNode(objectList, nr);

    if (n) {
	RemNode(n);
	FreeNode(n);
    }
}

struct ObjectNode *dbHasObjectNode(LIST * objectList, U32 nr)
{
    struct ObjectNode *n;

    for (n = (struct ObjectNode *) LIST_HEAD(objectList); NODE_SUCC(n);
	 n = (struct ObjectNode *) NODE_SUCC(n)) {
	if (OL_NR(n) == nr)
	    return n;
    }

    return NULL;
}

void SetObjectListAttr(U32 flags, U32 type)
{
    ObjectListType = type;
    ObjectListFlags = flags;

    if (ObjectListFlags & OLF_PRIVATE_LIST)
	ObjectListPrivate = CreateList();
    else {
	RemoveList(ObjectList);
	ObjectList = CreateList();
    }
}

void BuildObjectList(void *key)
{
    register struct dbObject *obj = dbGetObjectReal(key);
    register LIST *list;

    if (!ObjectListType || (obj->type == ObjectListType)) {
	if (ObjectListFlags & OLF_PRIVATE_LIST)
	    list = ObjectListPrivate;
	else
	    list = ObjectList;

	dbAddObjectNode(list, obj->nr, ObjectListFlags);
    }
}

void ExpandObjectList(LIST * objectList, char *expandItem)
{
    register struct ObjectNode *objNode;

    if (!
	(objNode =
	 (struct ObjectNode *) CreateNode(objectList, sizeof(*objNode),
					  expandItem)))
	ErrorMsg(Internal_Error, ERROR_MODULE_DATABASE, 2);

    objNode->nr = 0;
    objNode->type = 0;
    objNode->data = NULL;
}


S16 dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2)
{
    if (obj1->nr > obj2->nr)
	return -1;

    if (obj1->nr < obj2->nr)
	return 1;

    return 0;
}

void dbSortPartOfList(LIST * l, struct ObjectNode *start,
		      struct ObjectNode *end,
		      S16(*processNode) (struct ObjectNode *,
					  struct ObjectNode *))
{
    LIST *newList = CreateList();
    register struct ObjectNode *n, *n1, *startPred;
    S32 i, j;

    if (start == (struct ObjectNode *) LIST_HEAD(l))
	startPred = 0L;
    else
	startPred = (struct ObjectNode *) NODE_PRED(start);

    for (n = start, i = 1; n != end;
	 n = (struct ObjectNode *) NODE_SUCC(n), i++);

    n = start;

    for (j = 0; j < i; j++) {
	n1 = n;
	n = (struct ObjectNode *) NODE_SUCC(n);

	RemNode(n1);
	AddHeadNode(newList, n1);
    }

    dbSortObjectList(&newList, processNode);

    for (n = (struct ObjectNode *) LIST_HEAD(newList); NODE_SUCC(n);) {
	n1 = n;
	n = (struct ObjectNode *) NODE_SUCC(n);

	RemNode(n1);
	AddNode(l, n1, startPred);

	startPred = n1;
    }

    RemoveList(newList);
}

S32 dbSortObjectList(LIST ** objectList,
		     S16(*processNode) (struct ObjectNode *,
					 struct ObjectNode *))
{
    register LIST *newList;
    register struct ObjectNode *n1, *n2, *pred, *new;
    S32 i = 0;

    if (!LIST_EMPTY(*objectList)) {
	newList = CreateList();

	for (n1 = (struct ObjectNode *) LIST_HEAD(*objectList); NODE_SUCC(n1);
	     n1 = (struct ObjectNode *) NODE_SUCC(n1), i++) {
	    pred = 0;

	    if (!LIST_EMPTY(newList)) {
		for (n2 = (struct ObjectNode *) LIST_HEAD(newList);
		     !pred && NODE_SUCC(n2);
		     n2 = (struct ObjectNode *) NODE_SUCC(n2)) {
		    if (processNode(n1, n2) >= 0)
			pred = n2;
		}
	    }

	    new =
		(struct ObjectNode *) CreateNode(NULL, sizeof(*new),
						 NODE_NAME(n1));
	    new->nr = n1->nr;
	    new->type = n1->type;
	    new->data = n1->data;

	    if (pred) {
		if (pred == (struct ObjectNode *) LIST_HEAD(newList))
		    AddHeadNode(newList, new);
		else
		    AddNode(newList, new, NODE_PRED(pred));
	    } else
		AddTailNode(newList, new);
	}

	if (!LIST_EMPTY(newList)) {
	    RemoveList(*objectList);
	    *objectList = newList;
	} else
	    RemoveList(newList);
    }

    return i;
}


/* public prototypes */
void dbInit(void)
{
    register U8 objHashValue;

    if (!(ObjectList = CreateList()))
	ErrorMsg(No_Mem, ERROR_MODULE_DATABASE, 3);

    for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
	if (!(objHash[objHashValue] = CreateList()))
	    ErrorMsg(No_Mem, ERROR_MODULE_DATABASE, 4);
    }

    CompareKey = dbCompare;
    EncodeKey = dbEncode;
    DecodeKey = dbDecode;
}

void dbDone(void)
{
    register U8 objHashValue;

    for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++) {
	if (objHash[objHashValue])
	    RemoveList(objHash[objHashValue]);
    }

    if (ObjectList)
	RemoveList(ObjectList);
}

static U32 getKeyStd(KeyConflictE key)
{
    switch (key) {
    case _Environment_TheClou:
        return UINT32_C(  20);
    case _Player_Player_1:
        return UINT32_C(  21);
    case _London_London_1:
        return UINT32_C(  22);
    case _London_Jail:
        return UINT32_C(  23);
    case _London_Escape:
        return UINT32_C(  24);
    case _Evidence_Evidence_1:
        return UINT32_C(  25);
    case _CompleteLoot_LastLoot:
        return UINT32_C(  27);
    case _Person_Old_Matt:
        return UINT32_C(  28);

    case _Location_Holland_Street:
        return UINT32_C(  77);
    case _Location_Fat_Mans_Pub:
        return UINT32_C(  81);
    case _Location_Cars_Vans_Office:
        return UINT32_C(  82);
    case _Location_Tools_Shop:
        return UINT32_C(  83);
    case _Location_Policestation:
        return UINT32_C(  84);
    case _Location_Highgate_Out:
        return UINT32_C(  98);
    case _Location_Hotel:
        return UINT32_C( 124);
    case _Location_Walrus:
        return UINT32_C( 125);
    case _Location_Parker:
        return UINT32_C( 128);
    case _Location_Maloya:
        return UINT32_C( 129);
    case _Location_Pooly:
        return UINT32_C( 130);
    case _Location_Nirvana:
        return UINT32_C( 133);

    case _Ability_Autos:
        return UINT32_C( 140);
    case _Ability_Schloesser:
        return UINT32_C( 141);
    case _Ability_Sprengstoff:
        return UINT32_C( 142);
    case _Ability_Safes:
        return UINT32_C( 143);
    case _Ability_Elektronik:
        return UINT32_C( 144);
    case _Ability_Aufpassen:
        return UINT32_C( 145);
    case _Ability_Kampf:
        return UINT32_C( 146);

    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown trouble key #%" PRIu32, key);
        return 0;
    }
}

static U32 getKeyProfi(KeyConflictE key)
{
    switch (key) {
    case _Environment_TheClou:
        return UINT32_C(  28);
    case _Player_Player_1:
        return UINT32_C(  29);
    case _London_London_1:
        return UINT32_C(  30);
    case _London_Jail:
        return UINT32_C(  31);
    case _London_Escape:
        return UINT32_C(  32);
    case _Evidence_Evidence_1:
        return UINT32_C(  33);
    case _CompleteLoot_LastLoot:
        return UINT32_C(  35);
    case _Person_Old_Matt:
        return UINT32_C(  36);

    case _Location_Holland_Street:
        return UINT32_C(  95);
    case _Location_Fat_Mans_Pub:
        return UINT32_C(  99);
    case _Location_Cars_Vans_Office:
        return UINT32_C( 100);
    case _Location_Tools_Shop:
        return UINT32_C( 101);
    case _Location_Policestation:
        return UINT32_C( 102);
    case _Location_Highgate_Out:
        return UINT32_C( 116);
    case _Location_Hotel:
        return UINT32_C( 142);
    case _Location_Walrus:
        return UINT32_C( 143);
    case _Location_Parker:
        return UINT32_C( 146);
    case _Location_Maloya:
        return UINT32_C( 147);
    case _Location_Pooly:
        return UINT32_C( 148);
    case _Location_Nirvana:
        return UINT32_C( 151);

    case _Ability_Autos:
        return UINT32_C( 173);
    case _Ability_Schloesser:
        return UINT32_C( 174);
    case _Ability_Sprengstoff:
        return UINT32_C( 175);
    case _Ability_Safes:
        return UINT32_C( 176);
    case _Ability_Elektronik:
        return UINT32_C( 177);
    case _Ability_Aufpassen:
        return UINT32_C( 178);
    case _Ability_Kampf:
        return UINT32_C( 179);

    default:
	DebugMsg(ERR_ERROR, ERROR_MODULE_DATABASE,
            "Unknown trouble key #%" PRIu32, key);
        return 0;
    }
}

U32 getKey(KeyConflictE key)
{
    if (setup.Profidisk) {
        return getKeyProfi(key);
    } else {
        return getKeyStd(key);
    }
}

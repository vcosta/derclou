/*
**	$Filename: data\database.ph
**	$Release:  1
**	$Revision: 0
**	$Date:     09-03-94
**
**	database private definitions for "Der Clou!"
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

#ifndef MODULE_DATABASE_PH
#define MODULE_DATABASE_PH

/* includes */


/* private defines */
#define dbGetObjectReal(key)  (((struct dbObject *)key)-1)
#define dbGetObjectKey(obj)   ((void *)(obj+1))
#define dbGetObjectHashNr(nr) ((U8)(nr % OBJ_HASH_SIZE))

#define EOS                   ((char)'\0')


/* private definitions */
extern LIST *objHash[OBJ_HASH_SIZE];
extern char decodeStr[11];

extern U32 ObjectListType;
extern U32 ObjectListFlags;


/* private prototypes - RELATION */
int dbCompare(KEY key1, KEY key2);
char *dbDecode(KEY key);
KEY dbEncode(char *key);

/* private prototypes - OBJECT */
struct dbObject *dbFindRealObject(U32 realNr, U32 offset, U32 size);
#endif

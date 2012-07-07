/*
**	$Filename: text/text.h
**	$Release:  1
**	$Revision: 0
**	$Date:     10-03-94
**
**	text interface for "Der Clou!"
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

#ifndef MODULE_TEXT
#define MODULE_TEXT

/* includes */

#ifndef __CTYPE_H
#include <ctype.h>
#endif

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error/error.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif


/* public defines */
typedef enum {
    TXT_LANG_GERMAN,
    TXT_LANG_ENGLISH,
    TXT_LANG_FRENCH,
    TXT_LANG_SPANISH,
    TXT_LANG_SLOWAKISCH,
    TXT_LANG_LAST
} TxtLanguageE;

#define TXT_KEY_LENGTH     256


#define txtGetFirstLine(id, key, dest) (txtGetNthString(id, key, 0, dest))


/* public prototypes - TEXT */
void txtInit(char lang);
void txtDone(void);

void txtLoad(U32 textId);
void txtUnLoad(U32 textId);

void txtPrepare(U32 textId);
void txtUnPrepare(U32 textId);

void txtReset(U32 textId);


/* public prototypes - KEY */
char *txtGetKey(U16 keyNr, char *key);
U32 txtGetKeyAsULONG(U16 keyNr, char *key);

LIST *txtGoKey(U32 textId, const char *key);
LIST *txtGoKeyAndInsert(U32 textId, char *key, ...);

bool txtKeyExists(U32 textId, const char *key);
U32 txtCountKey(char *key);


/* public prototypes - STRING */
char *txtGetString(U32 textId, const char *key, char *dest);
char *txtGetNthString(U32 textId, const char *key, U32 nth, char *dest);
void txtPutCharacter(LIST * list, uword pos, U8 c);

#endif

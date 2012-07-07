/*
**	$Filename: text/text.ph
**	$Release:  1
**	$Revision: 0
**	$Date:     10-03-94
**
**	text private definitions for "Der Clou!"
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

#ifndef MODULE_TXT_PH
#define MODULE_TXT_PH

/* includes */
#include "theclou.h"
#include "disk/disk.h"
#include "list/list.h"
#include "base/base.h"


/* private defines */
#define TXT_DISK_ID     0
#define TXT_LIST        "texts.lst"
#define TXT_SUFFIX      ".txt"

#define TXT_BUFFER_LOAD    StdBuffer0
#define TXT_BUFFER_WORK    StdBuffer1

#define TXT_CHAR_EOL             '\n'
#define TXT_CHAR_EOS             '\0'
#define TXT_CHAR_MARK            '#'
#define TXT_CHAR_REMARK          ';'
#define TXT_CHAR_KEY_SEPERATOR   ','
#define TXT_CHAR_EOF             '^'

#define TXT_XOR_VALUE				0x75


/* private structures */
struct TextControl {
    LIST *tc_Texts;

    char tc_Language;
};

struct Text {
    NODE txt_Link;

    char *txt_Handle;
    char *txt_LastMark;

    size_t length;
};


/* private gobals definition */
extern char *txtLanguageMark[];
extern struct TextControl *txtBase;
extern char keyBuffer[];


/* private prototypes - LINE */
static char *txtGetLine(struct Text *txt, ubyte lineNr);

#endif

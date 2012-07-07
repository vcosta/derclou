/*
**	$Filename: text\text.c
**	$Release:  1
**	$Revision: 0
**	$Date:     10-03-94
**
**	text implementation for "Der Clou!"
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

#ifndef MODULE_TXT
#define MODULE_TXT

/* public header(s) */
#include "text/text.h"

/* private header(s) */
#include "text/text.eh"
#include "text/text.ph"

/* private globals declaration */
char *txtLanguageMark[TXT_LANG_LAST] = {
    "d", "e", "f", "s", "d"
};

struct TextControl *txtBase = NULL;
char keyBuffer[TXT_KEY_LENGTH];


/* private functions */
static char *txtGetLine(struct Text *txt, U8 lineNr)
{
    U8 i;
    char *line = NULL;

    if (txt && txt->txt_LastMark && lineNr) {
	line = txt->txt_LastMark;
	i = 0;

	while (i < lineNr) {
	    if (*line == TXT_CHAR_EOF)
		return NULL;

	    if (i > 0 && (*line == TXT_CHAR_MARK))
		return NULL;

	    if (*line == TXT_CHAR_EOS) {
		line++;		/* skip second EOS */
		i++;

		if (*line == TXT_CHAR_EOF)
		    return NULL;

		/* skip comments */
		while (*(line + 1) == TXT_CHAR_REMARK) {
		    while (*(++line) != TXT_CHAR_EOS);
		    line++;	/* skip second EOS */
		}
	    }

	    line++;
	}

	if (*line == TXT_CHAR_EOF)
	    return NULL;
	if (*line == TXT_CHAR_MARK)
	    return NULL;
    }

    return line;
}

/*  public functions - TEXT */
void txtInit(char lang)
{
    char txtListPath[DSK_PATH_MAX];

    if ((txtBase = TCAllocMem(sizeof(*txtBase), 0))) {
	txtBase->tc_Texts = CreateList();
	txtBase->tc_Language = lang;

	dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, TXT_LIST, txtListPath);

	if (ReadList(txtBase->tc_Texts, sizeof(struct Text), txtListPath)) {
	    U32 i, nr;

            nr = GetNrOfNodes(txtBase->tc_Texts);

	    for (i=0; i<nr; i++) {
		txtLoad(i);
	    }
	} else {
	    ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_READING_LIST);
	}
    } else {
	ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_FAILED_BASE);
    }
}

void txtDone(void)
{
    if (txtBase) {
	U32 i, nr;

        nr = GetNrOfNodes(txtBase->tc_Texts);

	for (i=0; i<nr; i++) {
	    txtUnLoad(i);
        }

	RemoveList(txtBase->tc_Texts);

	TCFreeMem(txtBase, sizeof(*txtBase));
    }
}

void txtLoad(U32 textId)
{
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt) {
	if (!txt->txt_Handle) {
	    char txtFile[DSK_PATH_MAX];
	    char txtPath[DSK_PATH_MAX];
	    U8 *ptr, *text;
	    size_t length;

            sprintf(txtFile, "%s%c%s",
                NODE_NAME(txt),
                txtBase->tc_Language,
                TXT_SUFFIX);

	    dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, txtFile, txtPath);

	    length = dskFileLength(txtPath);
            txt->length = length;

	    /* loading text into buffer */
	    text = dskLoad(txtPath);

	    /* correcting text */
	    for (ptr=text; length--; ptr++) {

		*ptr ^= TXT_XOR_VALUE;

                switch (*ptr) {
                case '\r':
                case '\n':
                    *ptr = '\0';
                    break;

                default:
                    break;
                }
	    }

	    /* save text into xms */
	    if (text) {
                txt->txt_Handle = malloc(txt->length+1);
                memcpy(txt->txt_Handle, text, txt->length);
                free(text);

		/* let's play safe here... */
		txt->txt_Handle[txt->length] = TXT_CHAR_EOF;
		txt->length++;
	    } else
		ErrorMsg(No_Mem, ERROR_MODULE_TXT, ERR_TXT_NO_MEM);
	}
    }
}

void txtUnLoad(U32 textId)
{
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt) {
	if (txt->txt_Handle) {
	    free(txt->txt_Handle);
	}

	txt->txt_Handle = NULL;
	txt->txt_LastMark = NULL;
	txt->length = 0;
    }
}

void txtPrepare(U32 textId)
{
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt) {
	memcpy(TXT_BUFFER_WORK, txt->txt_Handle, txt->length);
	txt->txt_LastMark = TXT_BUFFER_WORK;
    }
}

void txtUnPrepare(U32 textId)
{
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt)
	txt->txt_LastMark = NULL;
}

void txtReset(U32 textId)
{
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt)
	txt->txt_LastMark = TXT_BUFFER_WORK;
}


/* public functions - KEY */
char *txtGetKey(U16 keyNr, char *key)
{
    uword i;

    if (!key)
	return NULL;

    for (i = 1; i < keyNr; i++) {
	while (*key && (*key != TXT_CHAR_KEY_SEPERATOR))
	    key++;

	if (!*key)
	    return NULL;
	else
	    key++;
    }

    while (isspace(*key))
	key++;

    if (!*key)
	return NULL;

    for (i = 0;
	 (i < TXT_KEY_LENGTH) && *key && (*key != TXT_CHAR_KEY_SEPERATOR); i++)
	keyBuffer[i] = *key++;

    keyBuffer[i] = TXT_CHAR_EOS;
    return keyBuffer;
}

U32 txtGetKeyAsULONG(U16 keyNr, char *key)
{
    char *res = txtGetKey(keyNr, key);

    if (res)
	return ((U32) atoi(res));
    else
	return ((U32) - 1);
}

LIST *txtGoKey(U32 textId, const char *key)
{
    LIST *txtList = NULL;
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt) {
	char *LastMark = NULL;

	/* MOD: 08-04-94 hg
         * if no key was given take the next one
	 * -> last position is temporarily saved in LastMark because
	 * txt->LastMark is modified in txtPrepare!
	 *
	 * Special case: no key, text never used !!
	 */
	if ((!key) && (txt->txt_LastMark))
	    LastMark = txt->txt_LastMark;

	txtPrepare(textId);

	/* Explanation for +1: LastMark points to the last key
	   -> without "+1" the same key would be returned */
	if (!key && LastMark)
	    txt->txt_LastMark = LastMark + 1;

	for (; *txt->txt_LastMark != TXT_CHAR_EOF; txt->txt_LastMark++) {
	    if (*txt->txt_LastMark == TXT_CHAR_MARK) {
		U8 found = 1;

		if (key) {
		    char mark[TXT_KEY_LENGTH];

		    strcpy(mark, txt->txt_LastMark + 1);

		    if (strcmp(key, mark) != 0)
			found = 0;
		}

		if (found) {
		    U8 i = 1;
		    char *line;

		    txtList = CreateList();

		    while ((line = txtGetLine(txt, i++)))
			CreateNode(txtList, 0, line);
		    break;
		}
	    }
	}

    }

    if (!txtList) {
	DebugMsg(ERR_ERROR, ERROR_MODULE_TXT, "NOT FOUND KEY '%s'", key);
    }

    return txtList;
}

LIST *txtGoKeyAndInsert(U32 textId, char *key, ...)
{
    va_list argument;
    LIST *txtList = CreateList(), *originList = NULL;
    NODE *node;

    va_start(argument, key);

    originList = txtGoKey(textId, key);

    for (node = LIST_HEAD(originList); NODE_SUCC(node); node = NODE_SUCC(node)) {
	U8 i;
	char originLine[256], txtLine[256];

	strcpy(originLine, NODE_NAME(node));
	strcpy(txtLine, NODE_NAME(node));

	for (i = 2; i < strlen(originLine); i++) {
	    if (originLine[i - 2] == '%') {
		sprintf(txtLine, originLine, va_arg(argument, U32));
		i = strlen(originLine) + 1;
	    }
	}

	CreateNode(txtList, 0, txtLine);
    }

    RemoveList(originList);

    return txtList;
}

bool txtKeyExists(U32 textId, const char *key)
{
    bool found = false;
    struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

    if (txt && key) {
	txtPrepare(textId);

	/* after txtPrepare txt_LastMark points to TXT_BUFFER_PREPARE in every case */
	for (; *txt->txt_LastMark != TXT_CHAR_EOF; txt->txt_LastMark++) {
	    if (*txt->txt_LastMark == TXT_CHAR_MARK) {
		char mark[TXT_KEY_LENGTH];

		strcpy(mark, txt->txt_LastMark + 1);

		if (strcmp(key, mark) == 0) {
		    found = true;
		    break;
		}
	    }
	}
    }

    return found;
}

U32 txtCountKey(char *key)
{
    U32 i = strlen(key), j, k;

    for (j = 0, k = 0; j < i; j++) {
	if (key[j] == TXT_CHAR_KEY_SEPERATOR)
	    k++;
    }

    return k + 1;
}


/* functions - STRING */
char *txtGetNthString(U32 textId, const char *key, U32 nth, char *dest)
{
    LIST *txtList = txtGoKey(textId, key);
    void *src;

    if ((src = GetNthNode(txtList, nth))) {
	strcpy(dest, NODE_NAME(src));
    } else {
	strcpy(dest, "");
    }

    RemoveList(txtList);

    return dest;
}

void txtPutCharacter(LIST * list, uword pos, U8 c)
{
    NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	NODE_NAME(node)[pos] = c;
}

#endif

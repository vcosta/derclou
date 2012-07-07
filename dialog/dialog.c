/*
**	$Filename: dialog/dialog.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     07-04-94
**
**	dialog functions for "Der Clou!"
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

#include "dialog/dialog.h"
#include "dialog/talkappl.h"

#define DLG_NO_SPEECH	((U32) -1)
U32 StartFrame = DLG_NO_SPEECH;
U32 EndFrame = DLG_NO_SPEECH;

struct DynDlgNode {
    NODE Link;

    ubyte KnownBefore;		/* wie gut Sie bekannt sein mÅssen */
    ubyte KnownAfter;		/* wie gut Sie danach bekannt sind ! */
};


static LIST *PrepareQuestions(LIST * keyWords, U32 talkBits, ubyte textID)
{
    LIST *questionList = 0L, *preparedList = CreateList();
    LIST *stdQuestionList = txtGoKey(BUSINESS_TXT, "STD_QUEST");
    NODE *n;
    char question[TXT_KEY_LENGTH];
    ubyte r, i;

    questionList = txtGoKey((U32) textID, "QUESTIONS");

    for (n = (NODE *) LIST_HEAD(keyWords); NODE_SUCC((NODE *) n);
	 n = (NODE *) NODE_SUCC(n)) {
	r = (ubyte) CalcRandomNr(0L, 6L);

	sprintf(question, NODE_NAME(GetNthNode(questionList, r)),
		NODE_NAME((NODE *) n));

	CreateNode(preparedList, 0L, question);
    }

    for (i = 0; i < 32; i++) {
	if (talkBits & (1 << i)) {
	    strcpy(question, NODE_NAME(GetNthNode(stdQuestionList, i)));
	    CreateNode(preparedList, 0L, question);
	}
    }

    txtGetFirstLine(BUSINESS_TXT, "Bye_says_Matt", question);
    CreateNode(preparedList, 0L, question);

    RemoveList(stdQuestionList);
    RemoveList(questionList);

    return (preparedList);
}

static LIST *ParseTalkText(LIST * origin, LIST * bubble, ubyte known)
{
    LIST *keyWords;
    NODE *n, *keyNode;
    char line[TXT_KEY_LENGTH], key[TXT_KEY_LENGTH], keyWord[TXT_KEY_LENGTH];
    char *mem, *start;
    ubyte line_pos = 0, key_pos;
    char snr[10], snr1[10];
    ubyte nr, nr1;
    U32 i;

    keyWords = CreateList();

    for (n = (NODE *) LIST_HEAD(origin); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	line_pos = key_pos = 0;

	start = NODE_NAME(n);
	mem = start;

	while (mem < start + strlen(start)) {
	    if (*mem != '[')
		line[line_pos++] = *(mem++);
	    else {
		mem++;		/* Klammer Åberspringen ! */
		key_pos = 0;

		while ((*mem) != ']')
		    key[key_pos++] = *(mem++);

		key[key_pos++] = EOS;

		mem++;		/* Klammer Åberspringen ! */

		for (i = 0; i < 3; i++) {	/* Zahlen entfernen */
		    snr[i] = key[i];
		    snr1[i] = key[key_pos - 4 + i];
		}

		for (i = 3; i < (strlen(key) - 3); i++)	/* keyWord */
		    keyWord[i - 3] = key[i];

		/* umwandeln ! */
		snr[3] = EOS;
		snr1[3] = EOS;
		keyWord[strlen(key) - 6] = EOS;

		nr = (ubyte) atol(snr);
		nr1 = (ubyte) atol(snr1);

		/* keyword einfÅgen */
		for (i = 0; i < strlen(keyWord); i++)
		    line[line_pos++] = keyWord[i];

		if (known >= nr) {
		    keyNode =
			CreateNode(keyWords, sizeof(struct DynDlgNode),
				   keyWord);

		    ((struct DynDlgNode *) keyNode)->KnownBefore = nr;
		    ((struct DynDlgNode *) keyNode)->KnownAfter = nr1;
		}
	    }
	}

	line[line_pos++] = EOS;

	(void) CreateNode(bubble, 0L, line);
    }

    return (keyWords);
}

void DynamicTalk(U32 Person1ID, U32 Person2ID, ubyte TalkMode)
{
    char *Extension[4] = { "_UNKNOWN", "_KNOWN", "_FRIENDLY", "_BUSINESS" };
    char *Standard = "STANDARD";
    U8 known = 0;
    Person p1 = (Person) dbGetObject(Person1ID);
    Person p2 = (Person) dbGetObject(Person2ID);
    char key[TXT_KEY_LENGTH], name[TXT_KEY_LENGTH];
    U8 choice = 0, max = 1, i, quit, stdcount = 0, j, gencount = 0, textID;
    LIST *origin = 0L, *questions = 0L, *bubble = CreateList(), *keyWords;
    struct DynDlgNode *n;

    tcChgPersPopularity(p1, 5);	/* Bekanntheit steigt sehr gering */

    /* je nach Bekanntheitsgrad wird Matt begrÅ·t ! */
    dbGetObjectName(Person2ID, name);
    strcpy(key, name);

    if (TalkMode & DLG_TALKMODE_BUSINESS) {
	knowsSet(Person1ID, Person2ID);
	known = 3;		/* Business */
    } else {
	if (!(knows(Person1ID, Person2ID))) {
	    known = 0;
	    knowsSet(Person1ID, Person2ID);
	} else
	    known = 1;		/* MOD - kein "FRIENDLY mehr mîglich!" */
    }

    strcat(key, Extension[known]);

    if (p2->TalkFileID)
	textID = TALK_1_TXT;
    else
	textID = TALK_0_TXT;

    if (!(txtKeyExists(textID, key))) {
	strcpy(key, Standard);

	strcat(key, Extension[known]);
    }

    do {
	origin = txtGoKey(textID, key);
	keyWords = ParseTalkText(origin, bubble, p2->Known);
	questions = PrepareQuestions(keyWords, p2->TalkBits, textID);

	if (choice < (max - stdcount)) {
	    SetPictID(p2->PictID);
	    Bubble(bubble, 0, 0L, 0L);
	}

	SetPictID(MATT_PICTID);
	choice = Bubble(questions, 0, 0L, 0L);

	quit = max = GetNrOfNodes(questions) - 1;

	for (i = 0, stdcount = 0; i < 32; i++)	/* Std Fragen zÑhlen */
	    if (p2->TalkBits & (1 << i))
		stdcount++;

	gencount = max - stdcount;

	if (choice < gencount) {
	    n = (struct DynDlgNode *) GetNthNode(keyWords, (U32) choice);

	    strcpy(key, name);
	    strcat(key, "_");
	    strcat(key, NODE_NAME((NODE *) n));

	    if (n->KnownAfter > p2->Known)
		p2->Known = n->KnownAfter;
	}

	if (choice >= gencount && choice < quit) {
	    for (i = 0; i < 32; i++)	/* beim 1. gesetzten Bit anfangen ! */
		if (1 << i & p2->TalkBits)
		    break;

	    for (j = 0; (i < 32) && (j != (choice - gencount)); i++)
		if (1 << i & p2->TalkBits)
		    j++;

	    switch (i) {
	    case 0:
		tcJobOffer(p2);
		tcChgPersPopularity(p1, 10);
		break;
	    case 1:
		tcMyJobAnswer(p2);
		break;
	    case 2:
		tcPrisonAnswer(p2);
		break;
	    case 3:
		tcAbilityAnswer(Person2ID);
		break;
	    default:
		break;
	    }
	}

	RemoveList(keyWords);
	RemoveList(origin);
	RemoveList(questions);
	RemoveNode(bubble, NULL);
    }
    while (choice != quit);

    RemoveList(bubble);
}

void PlayFromCDROM(void)
{
    if ((StartFrame != DLG_NO_SPEECH) && (EndFrame != DLG_NO_SPEECH)) {
        sndFading(16);
        CDROM_PlayAudioSequence(2, StartFrame, EndFrame);
    }
}

ubyte Say(U32 TextID, ubyte activ, uword Person, const char *text)
{
    LIST *bubble;
    ubyte choice;

    if (setup.CDRom) {
        bubble = txtGoKey(TextID, text);

        if (Person != (uword) - 1)
            SetPictID(Person);

        /* speech output must be started out of the bubble because
           after starting it there must be no access to the CDROM
           (neither pictures or text, nor any other directory) or
           speech would be interrupted */

        if (txtKeyExists(CDROM_TXT, text)) {
            char keys[TXT_KEY_LENGTH];

            txtGetFirstLine(CDROM_TXT, text, keys);

            StartFrame =
                (txtGetKeyAsULONG(1, keys) * 60L +
                 txtGetKeyAsULONG(2, keys)) * 75L + txtGetKeyAsULONG(3, keys);
            EndFrame =
                (txtGetKeyAsULONG(4, keys) * 60L +
                 txtGetKeyAsULONG(5, keys)) * 75L + txtGetKeyAsULONG(6, keys);

            choice = Bubble(bubble, activ, NULL, 0L);
        } else {
            StartFrame = DLG_NO_SPEECH;
            EndFrame = DLG_NO_SPEECH;

            choice = Bubble(bubble, activ, NULL, 0L);
        }

        if (setup.CDAudio) {
            CDROM_StopAudioTrack();
            sndFading(0);
        }

        StartFrame = DLG_NO_SPEECH;
        EndFrame = DLG_NO_SPEECH;

        RemoveList(bubble);
    } else {
        bubble = txtGoKey(TextID, text);

        if (Person != (uword) - 1)
	    SetPictID(Person);

        choice = Bubble(bubble, activ, NULL, 0L);
        RemoveList(bubble);
    }

    return (choice);
}


U32 Talk(void)
{
    U32 succ_event_nr = 0L, locNr, personID;
    LIST *bubble;
    ubyte choice;
    char helloFriends[TXT_KEY_LENGTH];

    inpTurnESC(0);

    locNr = GetObjNrOfLocation(GetLocation);

    if (locNr) {
	hasAll(locNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	       Object_Person);
	bubble = ObjectListPrivate;

	if (!(LIST_EMPTY(bubble))) {
	    inpTurnESC(1);

	    txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", helloFriends);
	    ExpandObjectList(bubble, helloFriends);

	    if (ChoiceOk((choice = Bubble(bubble, 0, 0L, 0L)), GET_OUT, bubble)) {
		personID =
		    ((struct ObjectNode *) GetNthNode(bubble, (U32) choice))->
		    nr;

		inpTurnESC(0);

		if (PersonWorksHere(personID, locNr))
		    DynamicTalk(Person_Matt_Stuvysunt, personID,
				DLG_TALKMODE_BUSINESS);
		else
		    DynamicTalk(Person_Matt_Stuvysunt, personID,
				DLG_TALKMODE_STANDARD);
	    }
	} else
	    Say(BUSINESS_TXT, 0, MATT_PICTID, "NOBODY HERE");

	RemoveList(bubble);
    }

    inpTurnESC(1);

    return (succ_event_nr);
}

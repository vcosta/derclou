/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

struct DynDlgNode {
    NODE Link;

    ubyte KnownBefore;		/* wie gut Sie bekannt sein mssen */
    ubyte KnownAfter;		/* wie gut Sie danach bekannt sind ! */
};

U32 Talk(void)
{
    U32 succ_event_nr = 0L, locNr, personID;
    LIST *bubble;
    ubyte choice, helloFriends[KEY_LENGTH];

    TurnESC(0);

    locNr = GetObjNrOfLocation(GetLocation);

    if (locNr) {
	hasAll(locNr, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR,
	       Object_Person);
	bubble = ObjectListPrivate;

	if (!(LIST_EMPTY(bubble))) {
	    TurnESC(1);

	    GetFirstLine(BUSINESS_TXT, "NO_CHOICE", helloFriends);
	    ExpandObjectList(bubble, helloFriends);

	    if (ChoiceOk((choice = Bubble(bubble, 0, 0L, 0L)), GET_OUT, bubble)) {
		personID =
		    ((struct ObjectNode *) GetNthNode(bubble, (U32) choice))->
		    nr;

		TurnESC(0);

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

    TurnESC(1);

    return (succ_event_nr);
}

void DynamicTalk(U32 Person1ID, U32 Person2ID, ubyte TalkMode)
{
    ubyte *Extension[4] = { "_UNKNOWN", "_KNOWN", "_FRIENDLY", "_BUSINESS" };
    ubyte *Standard = "STANDARD", known = 0;
    Person p1 = (Person) dbGetObject(Person1ID);
    Person p2 = (Person) dbGetObject(Person2ID);
    ubyte key[KEY_LENGTH], name[KEY_LENGTH], choice = 0, max =
	1, i, quit, stdcount = 0, j, gencount = 0, textID;
    LIST *origin = 0L, *questions = 0L, *bubble = CreateList(0L, 0), *keyWords;
    struct DynDlgNode *n;

    /* for speedup */

    if (p1->PictID)
	PreparePict(p1->PictID);

    if (p2->PictID)
	PreparePict(p2->PictID);

    SetBubblePictMode(DO_NOT_FREE);

    tcChgPersPopularity(p1, 5);	/* Bekanntheit steigt sehr gering */

    /* je nach Bekanntheitsgrad wird Matt begrát ! */

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
	    known = 1;		/* MOD - kein "FRIENDLY mehr m”glich!" */
    }

    strcat(key, Extension[known]);

    if (p2->TalkFileID)
	textID = TALK_1_TXT;
    else
	textID = TALK_0_TXT;

    if (!(KeyExists(textID, key))) {
	strcpy(key, Standard);

	strcat(key, Extension[known]);
    }

    do {
	origin = GoKey(textID, key);
	keyWords = ParseTalkText(origin, bubble, p2->Known);
	questions = PrepareQuestions(keyWords, p2->TalkBits, textID);

	if (choice < (max - stdcount)) {
	    SetPictID(p2->PictID);
	    Bubble(bubble, 0, 0L, 0L);
	}

	SetPictID(MATT_PICTID);
	choice = Bubble(questions, 0, 0L, 0L);

	quit = max = GetNrOfNodes(questions) - 1;

	for (i = 0, stdcount = 0; i < 32; i++)	/* Std Fragen z„hlen */
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

    if (p1->PictID)
	UnPreparePict(p1->PictID);

    if (p2->PictID)
	UnPreparePict(p2->PictID);

    SetBubblePictMode(0);

    RemoveList(bubble);
}

LIST *PrepareQuestions(LIST * keyWords, U32 talkBits, ubyte textID)
{
    LIST *questionList = 0L, *preparedList = CreateList(0L, 0);
    LIST *stdQuestionList = GoKey(BUSINESS_TXT, "STD_QUEST");
    NODE *n;
    ubyte question[KEY_LENGTH], r, i;

    questionList = GoKey((U32) textID, "QUESTIONS");

    for (n = (NODE *) LIST_HEAD(keyWords); NODE_SUCC((NODE *) n);
	 n = (NODE *) NODE_SUCC(n)) {
	r = (ubyte) CalcRandomNr(0L, 6L);

	sprintf(question, NODE_NAME(GetNthNode(questionList, r)),
		NODE_NAME((NODE *) n));

	CreateNode(preparedList, 0L, question, 0);
    }

    for (i = 0; i < 32; i++) {
	if (talkBits & (1 << i)) {
	    strcpy(question, NODE_NAME(GetNthNode(stdQuestionList, i)));
	    CreateNode(preparedList, 0L, question, 0);
	}
    }

    GetFirstLine(BUSINESS_TXT, "Bye_says_Matt", question);
    CreateNode(preparedList, 0L, question, 0);

    RemoveList(stdQuestionList);
    RemoveList(questionList);

    return (preparedList);
}

LIST *ParseTalkText(LIST * origin, LIST * bubble, ubyte known)
{
    LIST *keyWords;
    NODE *n, *keyNode;
    ubyte line[KEY_LENGTH], line_pos = 0, *mem, key[KEY_LENGTH], key_pos;
    ubyte snr[10], snr1[10], nr, nr1, keyWord[KEY_LENGTH];
    U32 i;

    keyWords = CreateList(0L, 0);

    for (n = (NODE *) LIST_HEAD(origin); NODE_SUCC(n);
	 n = (NODE *) NODE_SUCC(n)) {
	line_pos = key_pos = 0;

	mem = NODE_NAME(n);

	while (mem < (NODE_NAME(n) + strlen(NODE_NAME(n)))) {
	    if (*mem != '[')
		line[line_pos++] = *(mem++);
	    else {
		mem++;		/* Klammer berspringen ! */
		key_pos = 0;

		while ((*mem) != ']')
		    key[key_pos++] = *(mem++);

		key[key_pos++] = EOS;

		mem++;		/* Klammer berspringen ! */

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

		/* keyword einfgen */

		for (i = 0; i < strlen(keyWord); i++)
		    line[line_pos++] = keyWord[i];

		if (known >= nr) {
		    keyNode =
			CreateNode(keyWords, sizeof(struct DynDlgNode),
				   keyWord, 0);

		    ((struct DynDlgNode *) keyNode)->KnownBefore = nr;
		    ((struct DynDlgNode *) keyNode)->KnownAfter = nr1;
		}
	    }
	}

	line[line_pos++] = EOS;

	(void) CreateNode(bubble, 0L, line, 0);
    }

    return (keyWords);
}

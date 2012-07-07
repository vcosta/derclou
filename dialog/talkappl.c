/*
**	$Filename: dialog/talkAppl.c
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

#include "dialog/talkappl.h"

void tcJobOffer(Person p)
{
    LIST *bubble;
    ubyte choice;
    S32 part;
    Player player = dbGetObject(Player_Player_1);

    if (!(join(Person_Matt_Stuvysunt, dbGetObjectNr(p))))
	player->JobOfferCount++;

    choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "PERS_ANZ");

    part = tcGetPersOffer(p, (choice + 2));

    bubble = txtGoKeyAndInsert(BUSINESS_TXT, "JOB_ANSWER", (U32) part, NULL);

    SetPictID(p->PictID);
    Bubble(bubble, 0, 0L, 0L);
    RemoveList(bubble);

    choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "NEW_THEEF");

    if (choice == 0) {
	joinSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));
	hasSet(Person_Matt_Stuvysunt, dbGetObjectNr(p));	/* Matt hat jetzt Daten von ihm */
	p->TalkBits &= (0xffffffffL - (1 << Const_tcTALK_JOB_OFFER));	/* Joboffer l”schen! */
    } else {
	p->Known = CalcValue(p->Known, 0, 255, 0, 20);
	Say(BUSINESS_TXT, 0, p->PictID, "MY_OFFER");
    }
}

void tcMyJobAnswer(Person p)
{
    LIST *bubble = CreateList();
    LIST *jobs = txtGoKey(OBJECTS_ENUM_TXT, "enum_JobE");
    char line[TXT_KEY_LENGTH], job[TXT_KEY_LENGTH], temp[TXT_KEY_LENGTH];

    strcpy(job, NODE_NAME(GetNthNode(jobs, p->Job)));

    if (strcmp(job, NODE_NAME(GetNthNode(jobs, 10)))) {
	txtGetFirstLine(BUSINESS_TXT, "MY_JOB_IS", temp);
	sprintf(line, temp, job);
    } else
	txtGetFirstLine(BUSINESS_TXT, "NO_JOB", line);

    SetPictID(p->PictID);
    CreateNode(bubble, 0L, line);
    Bubble(bubble, 0, 0L, 0L);

    RemoveList(jobs);
    RemoveList(bubble);
}

void tcPrisonAnswer(Person p)
{
    LIST *bubble = CreateList();
    LIST *source = txtGoKey(BUSINESS_TXT, "IN_PRISON_ANSWER");
    char line[TXT_KEY_LENGTH];

    strcpy(line, NODE_NAME(GetNthNode(source, p->KnownToPolice / 52)));

    SetPictID(p->PictID);
    CreateNode(bubble, 0L, line);
    Bubble(bubble, 0, 0, 0);

    RemoveList(source);
    RemoveList(bubble);
}

void tcAbilityAnswer(U32 personID)
{
    char name[TXT_KEY_LENGTH];
    LIST *bubble;
    Person p = dbGetObject(personID);

    dbGetObjectName(personID, name);

    bubble = txtGoKey(ABILITY_TXT, name);

    SetPictID(p->PictID);

    Bubble(bubble, 0, 0, 0);

    RemoveList(bubble);
}

/*
**	$Filename: scenes/dealer.c
**	$Release:
**	$Revision:
**	$Date:
**
** "dealer" functions for "Der Clou!"
**
**	(C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "scenes/scenes.h"

void tcDealerDlg(void)
{
    U32 locNr = GetObjNrOfLocation(GetLocation);
    Person dealer = NULL;
    ubyte dealerNr, choice = 0;

    if (locNr == Location_Parker) {
        dealer = dbGetObject(Person_Helen_Parker);
        knowsSet(Person_Matt_Stuvysunt, Person_Helen_Parker);
        dealerNr = 2;
    } else if (locNr == Location_Maloya) {
        dealer = dbGetObject(Person_Frank_Maloya);
        knowsSet(Person_Matt_Stuvysunt, Person_Frank_Maloya);
        dealerNr = 0;
    } else if (locNr == Location_Pooly) {
        dealer = dbGetObject(Person_Eric_Pooly);
        knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);
        dealerNr = 1;
    } else {
        return;
    }

    while ((choice != 2) && (choice != GET_OUT)) {
	choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_QUEST");

	switch (choice) {
	case 0:		/* womit ? */
            if (locNr == Location_Parker) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_PARKER");
            } else if (locNr == Location_Maloya) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_MALOYA");
            } else if (locNr == Location_Pooly) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_POOLY");
            }
	    break;
	case 1:		/* offer */
	    hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);

	    if (LIST_EMPTY(ObjectList)) {
		Say(BUSINESS_TXT, 0, dealer->PictID, "NO_LOOT");
		AddVTime(17);
	    } else
		tcDealerOffer(dealer, dealerNr);
	    break;
	case 2:
	default:
	    break;
	}
    }

    AddVTime(11);
    ShowTime(2);
}

void tcDealerOffer(Person dealer, ubyte which)
{
    ubyte Price[3][10] = { {70, 150, 220, 90, 210, 110, 200, 0, 190, 80},	/* maloya */
    {120, 200, 180, 220, 79, 110, 0, 0, 110, 200},	/* pooly */
    {220, 66, 0, 110, 0, 220, 0, 212, 20, 130}
    };				/* parker */
    CompleteLoot comp = dbGetObject(CompleteLoot_LastLoot);

    RemoveList(tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

    if (comp->Bild)
	tcDealerSays(dealer, 0, (S32) Price[which][0]);
    if (comp->Gold)
	tcDealerSays(dealer, 1, (S32) Price[which][1]);
    if (comp->Geld)
	tcDealerSays(dealer, 2, (S32) Price[which][2]);
    if (comp->Juwelen)
	tcDealerSays(dealer, 3, (S32) Price[which][3]);
    if (comp->Delikates)
	tcDealerSays(dealer, 4, (S32) Price[which][4]);
    if (comp->Statue)
	tcDealerSays(dealer, 5, (S32) Price[which][5]);
    if (comp->Kuriositaet)
	tcDealerSays(dealer, 6, (S32) Price[which][6]);
    if (comp->HistKunst)
	tcDealerSays(dealer, 7, (S32) Price[which][7]);
    if (comp->GebrauchsArt)
	tcDealerSays(dealer, 8, (S32) Price[which][8]);
    if (comp->Vase)
	tcDealerSays(dealer, 9, (S32) Price[which][9]);
}

void tcDealerSays(Person dealer, ubyte textNr, S32 perc)
{
    LIST *lootNames = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootE");
    LIST *specialLoot = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootNameE");
    LIST *dealerText = txtGoKey(BUSINESS_TXT, "DEALER_OFFER");
    LIST *dealerOffer = CreateList();
    char line[TXT_KEY_LENGTH];
    ubyte symp, i;
    struct ObjectNode *n;
    Person others[3];
    Player player = dbGetObject(Player_Player_1);

    others[0] = dbGetObject(Person_Frank_Maloya);
    others[1] = dbGetObject(Person_Eric_Pooly);
    others[2] = dbGetObject(Person_Helen_Parker);

    if (perc == 0) {
	sprintf(line, NODE_NAME(GetNthNode(dealerText, 4)),
		NODE_NAME(GetNthNode(lootNames, (U32) textNr)));
	CreateNode(dealerOffer, 0L, line);

	CreateNode(dealerOffer, 0L, NODE_NAME(GetNthNode(dealerText, 5)));

	SetPictID(dealer->PictID);
	Bubble(dealerOffer, 0, 0L, 0L);
    } else {
	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);
	perc = tcGetDealerPerc(dealer, perc);

	for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n)) {
	    Loot loot = OL_DATA(n);
	    U32 price = hasGet(Person_Matt_Stuvysunt, OL_NR(n)), offer;

	    offer = tcGetDealerOffer(price, perc);
	    offer = max(offer, 1);

	    RemoveNode(dealerOffer, NULL);

	    if ((loot->Type) == textNr) {
		if (loot->Name) {
		    symp = 10;

		    sprintf(line, NODE_NAME(GetNthNode(dealerText, 2)),
			    NODE_NAME(GetNthNode
				      (specialLoot, (U32) loot->Name)));
		    CreateNode(dealerOffer, 0L, line);

		    sprintf(line, NODE_NAME(GetNthNode(dealerText, 3)), offer);
		    CreateNode(dealerOffer, 0L, line);
		} else {
		    symp = 1;

		    sprintf(line, NODE_NAME(GetNthNode(dealerText, 0)),
			    NODE_NAME(GetNthNode(lootNames, (U32) textNr)));
		    CreateNode(dealerOffer, 0L, line);

		    sprintf(line, NODE_NAME(GetNthNode(dealerText, 1)), price,
			    offer);
		    CreateNode(dealerOffer, 0L, line);
		}

		SetPictID(dealer->PictID);
		Bubble(dealerOffer, 0, 0L, 0L);

		if (!(Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_ANSWER"))) {
		    S32 mattsMoney;

		    hasUnSet(Person_Matt_Stuvysunt, OL_NR(n));

		    mattsMoney = max(((offer * (player->MattsPart)) / 100), 1);

		    tcAddDealerSymp(dealer, symp);
		    tcAddPlayerMoney(mattsMoney);

		    player->StolenMoney += offer;
		    player->MyStolenMoney += mattsMoney;

		    for (i = 0; i < 3; i++)
			if (dealer != others[i])
			    tcAddDealerSymp((others[i]), (symp * (-1)));
		}
	    }
	}
    }

    RemoveList(specialLoot);
    RemoveList(dealerOffer);
    RemoveList(dealerText);
    RemoveList(lootNames);
}

LIST *tcMakeLootList(U32 containerID, U32 relID)
{
    NODE *n;
    Loot loot;
    CompleteLoot comp = dbGetObject(CompleteLoot_LastLoot);
    char data[TXT_KEY_LENGTH];
    U32 value;
    LIST *out = CreateList();
    LIST *loots;
    LIST *lootE = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootE");
    LIST *lootNameE = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootNameE");

    /* Listen initialisieren */

    SetObjectListAttr(OLF_PRIVATE_LIST, Object_Loot);
    AskAll(dbGetObject(containerID), relID, BuildObjectList);
    loots = ObjectListPrivate;

    comp->Bild = comp->Gold = comp->Geld = comp->Juwelen = 0;
    comp->Delikates = comp->Statue = comp->Kuriositaet = 0;
    comp->HistKunst = comp->GebrauchsArt = comp->Vase = 0;

    comp->TotalWeight = comp->TotalVolume = 0;

    /* Liste durcharbeiten */

    if (!(LIST_EMPTY(loots))) {
	for (n = (NODE *) LIST_HEAD(loots); NODE_SUCC(n);
	     n = (NODE *) NODE_SUCC(n)) {
	    if (OL_TYPE(n) == Object_Loot) {
		loot = OL_DATA(n);

		value = GetP(dbGetObject(containerID), relID, loot);

		switch (loot->Type) {
		case Ein_Bild:
		    comp->Bild += value;
		    break;
		case Gold:
		    comp->Gold += value;
		    break;
		case Geld:
		    comp->Geld += value;
		    break;
		case Juwelen:
		    comp->Juwelen += value;
		    break;
		case Delikatessen:
		    comp->Delikates += value;
		    break;
		case Eine_Statue:
		    comp->Statue += value;
		    break;
		case Eine_Kuriositaet:
		    comp->Kuriositaet += value;
		    break;
		case Eine_Vase:
		    comp->Vase += value;
		    break;
		case Ein_historisches_Kunstobjekt:
		    comp->HistKunst += value;
		    break;
		case Gebrauchsartikel:
		    comp->GebrauchsArt += value;
		    break;
		default:
		    break;
		}

		comp->TotalWeight += loot->Weight;
		comp->TotalVolume += loot->Volume;

		if (loot->Name)
		    strcpy(data, NODE_NAME(GetNthNode(lootNameE, loot->Name)));
		else
		    strcpy(data, NODE_NAME(GetNthNode(lootE, loot->Type)));

		CreateNode(out, 0L, data);

		sprintf(data, "%" PRIu32, value);
		CreateNode(out, 0L, data);

		sprintf(data, "%" PRIu32, loot->Volume);
		CreateNode(out, 0L, data);

		sprintf(data, "%" PRIu16, loot->Weight);
		CreateNode(out, 0L, data);
	    }
	}
    }

    RemoveList(lootE);
    RemoveList(lootNameE);
    RemoveList(loots);

    return (out);
}

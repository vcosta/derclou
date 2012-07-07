/*
**      $Filename: planing/system.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.system for "Der Clou!"
**
** (c) 1994 ...and avoid panic by, Kaweh Kazemi
**      All Rights Reserved.
**
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define FILE_SYSTEM_ID      "SYS "	/* SYStem plan start */
#define FILE_HANDLER_ID     "HAND"	/* HANDler x needed  */
#define FILE_ACTION_LIST_ID "ACLI"	/* ACtionLIst for handler x started */
#define FILE_ACTION_ID      "ACTI"	/* ACTIon */

#define SYS_MAX_MEMORY_SIZE   1024L*25L

#include "planing/system.h"

size_t sysUsedMem = 0;

size_t sizeofAction[] = {
    0,				/* first index is not used yet */
    sizeof(struct ActionGo),
    0,
    sizeof(struct ActionSignal),
    sizeof(struct ActionWaitSignal),
    sizeof(struct ActionUse),
    sizeof(struct ActionTake),
    sizeof(struct ActionDrop),
    sizeof(struct ActionOpen),
    sizeof(struct ActionClose),
    sizeof(struct ActionControl)
};

size_t plGetUsedMem(void)
{
    return sysUsedMem;
}

struct Handler *FindHandler(struct System *sys, U32 id)
{
    register struct Handler *h;

    if (sys) {
	for (h = (struct Handler *) LIST_HEAD(sys->Handlers); NODE_SUCC(h);
	     h = (struct Handler *) NODE_SUCC(h)) {
	    if (h->Id == id)
		return h;
	}
    }

    return NULL;
}

struct System *InitSystem(void)
{
    register struct System *sys =
	(struct System *) TCAllocMem(sizeof(*sys), 0);

    if (sys) {
	sys->Handlers = CreateList();
	sys->Signals = CreateList();

	sys->ActivHandler = NULL;

	if (!sys->Handlers || !sys->Signals) {
	    CloseSystem(sys);
	    sys = NULL;
	}
    }

    return sys;
}

void CloseSystem(struct System *sys)
{
    if (sys) {
	RemoveList(sys->Handlers);
	RemoveList(sys->Signals);

	TCFreeMem(sys, sizeof(*sys));
    }
}

void SetActivHandler(struct System *sys, U32 id)
{
    register struct Handler *h;

    if ((h = FindHandler(sys, id)))
	sys->ActivHandler = (NODE *) h;
    else
	sys->ActivHandler = NULL;
}

void SaveSystem(FILE * fh, struct System *sys)
{
    register struct Handler *h;

    if (fh) {
	fprintf(fh, FILE_SYSTEM_ID "\r\n");

	for (h = (struct Handler *) LIST_HEAD(sys->Handlers); NODE_SUCC(h);
	     h = (struct Handler *) NODE_SUCC(h))
	    fprintf(fh, FILE_HANDLER_ID "\r\n%" PRIu32 "\r\n", h->Id);
    }
}

LIST *LoadSystem(FILE * fh, struct System *sys)
{
    register LIST *l = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_1");
    register U8 foundAll = 1, knowsSomebody = 1, handlerNr = 0;
    char buffer[64];

    if (fh) {
	if (dskGetLine(buffer, sizeof(buffer), fh)
	    && strcmp(buffer, FILE_SYSTEM_ID) == 0) {
	    while (dskGetLine(buffer, sizeof(buffer), fh)
		   && strcmp(buffer, FILE_HANDLER_ID) == 0) {
		U32 id;

		if (fscanf(fh, "%" SCNu32 "\r\n", &id) == 1
		    && !dbIsObject(id, Object_Police)) {
		    handlerNr++;

		    if (!FindHandler(sys, id)) {
			if (knows(Person_Matt_Stuvysunt, id)) {
			    knowsSomebody++;
			    dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
			}

			foundAll = 0;
		    }
		}
	    }
	}
    }

    if (foundAll) {
	RemoveList(l);
	l = NULL;
    } else {
	LIST *extList = NULL;
	NODE *n;

	if (knowsSomebody == 1)
	    extList = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_3");
	else {
	    if ((handlerNr - knowsSomebody) > 1)
		extList =
		    txtGoKeyAndInsert(PLAN_TXT, "SYSTEM_GUYS_MISSING_2",
				      (U32) (handlerNr - knowsSomebody));
	    else if (handlerNr - knowsSomebody)
		extList = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_4");
	}

	if (extList) {
	    for (n = LIST_HEAD(extList); NODE_SUCC(n); n = NODE_SUCC(n))
		CreateNode(l, 0, NODE_NAME(n));

	    RemoveList(extList);
	}
    }

    return l;
}

struct Handler *InitHandler(struct System *sys, U32 id, U32 flags)
{
    register struct Handler *h = NULL;

    if (sys && !FindHandler(sys, id)) {
	if ((h =
	     (struct Handler *) CreateNode(sys->Handlers, sizeof(*h), NULL))) {
	    h->Id = id;
	    h->Timer = 0L;
	    h->Flags = flags;
	    h->CurrentAction = NULL;

	    if (!(h->Actions = CreateList())) {
		RemNode(h);
		FreeNode(h);
		h = NULL;
	    }

	    sys->ActivHandler = (NODE *) h;
	}
    }

    return h;
}

void CloseHandler(struct System *sys, U32 id)
{
    register struct Handler *h;

    if ((h = FindHandler(sys, id))) {
	CorrectMem(h->Actions);

	if (h->Actions)
	    RemoveList(h->Actions);

	RemNode(h);
	FreeNode(h);
    }
}

struct Handler *ClearHandler(struct System *sys, U32 id)
{
    register struct Handler *h;

    if ((h = FindHandler(sys, id))) {
	CorrectMem(h->Actions);

	if (h->Actions)
	    RemoveList(h->Actions);

	if (!(h->Actions = CreateList())) {
	    RemNode(h);
	    FreeNode(h);
	    h = NULL;
	}

	h->Timer = 0L;
	h->CurrentAction = NULL;
    }

    return h;
}

ubyte IsHandlerCleared(struct System * sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if (LIST_EMPTY(h->Actions))
	    return 1;
    }

    return 0;
}

void SaveHandler(FILE * fh, struct System *sys, U32 id)
{
    register struct Handler *h;
    register struct Action *a;

    if (fh && sys && (h = FindHandler(sys, id))) {
	fprintf(fh, FILE_ACTION_LIST_ID "\r\n%" PRIu32 "\r\n", id);

	for (a = (struct Action *) LIST_HEAD(h->Actions); NODE_SUCC(a);
	     a = (struct Action *) NODE_SUCC(a)) {
	    fprintf(fh, FILE_ACTION_ID "\r\n%" PRIu16 "\r\n%" PRIu16 "\r\n",
		    a->Type, a->TimeNeeded);

	    switch (a->Type) {
	    case ACTION_GO:
		fprintf(fh, "%" PRIu16 "\r\n",
			ActionData(a, struct ActionGo *)->Direction);
		break;

	    case ACTION_USE:
	    case ACTION_TAKE:
	    case ACTION_DROP:
		fprintf(fh, "%" PRIu32 "\r\n%" PRIu32 "\r\n",
			ActionData(a, struct ActionUse *)->ToolId, ActionData(a,
									      struct
									      ActionUse
									      *)->
			ItemId);
		break;

	    case ACTION_OPEN:
	    case ACTION_CLOSE:
	    case ACTION_CONTROL:
	    case ACTION_SIGNAL:
	    case ACTION_WAIT_SIGNAL:
		fprintf(fh, "%" PRIu32 "\r\n",
			ActionData(a, struct ActionOpen *)->ItemId);
		break;
	    }
	}
    }
}

ubyte LoadHandler(FILE * fh, struct System *sys, U32 id)
{
    register struct Action *a;
    U16 type;
    U16 time;
    U16 value16;
    U32 value32;
    char buffer[64];

    if (fh && sys && (FindHandler(sys, id))) {
	rewind(fh);

	while (dskGetLine(buffer, sizeof(buffer), fh)) {
	    if (strcmp(buffer, FILE_ACTION_LIST_ID) == 0) {
		U32 rid;

		if (fscanf(fh, "%" SCNu32 "\r\n", &rid) == 1 && id == rid) {
		    SetActivHandler(sys, id);

		    while (dskGetLine(buffer, sizeof(buffer), fh)
			   && (strcmp(buffer, FILE_ACTION_ID) == 0)) {
			fscanf(fh, "%" SCNu16 "\r\n", &type);
			fscanf(fh, "%" SCNu16 "\r\n", &time);

			if (type) {
			    a = InitAction(sys, type, 0L, 0L, time);

			    switch (type) {
			    case ACTION_GO:
				fscanf(fh, "%" SCNu16 "\r\n", &value16);
				ActionData(a, struct ActionGo *)->Direction =
				    value16;
				break;

			    case ACTION_USE:
			    case ACTION_TAKE:
			    case ACTION_DROP:
				fscanf(fh, "%" SCNu32 "\r\n", &value32);
				ActionData(a, struct ActionUse *)->ToolId =
				    value32;

				fscanf(fh, "%" SCNu32 "\r\n", &value32);
				ActionData(a, struct ActionUse *)->ItemId =
				    value32;
				break;

			    case ACTION_OPEN:
			    case ACTION_CLOSE:
			    case ACTION_CONTROL:
			    case ACTION_WAIT_SIGNAL:
			    case ACTION_SIGNAL:
				fscanf(fh, "%" SCNu32 "\r\n", &value32);
				ActionData(a, struct ActionOpen *)->ItemId =
				    value32;
				break;
			    }
			} else
			    return 0;
		    }

		    GoFirstAction(sys);
		    return 1;
		}
	    }
	}
    }

    return 0;
}

struct Action *InitAction(struct System *sys, uword type, U32 data1, U32 data2,
			  U32 time)
{
    register struct Handler *h;
    register struct Action *a = NULL;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((sysUsedMem + sizeof(struct Action) + sizeofAction[type]) <=
	    SYS_MAX_MEMORY_SIZE) {
	    sysUsedMem += sizeof(struct Action) + sizeofAction[type];

	    if ((a =
		 (struct Action *) CreateNode(h->Actions,
					      sizeof(struct Action) +
					      sizeofAction[type], NULL))) {
		a->Type = type;
		a->TimeNeeded = time;
		a->Timer = time;

		h->Timer += time;
		h->CurrentAction = (NODE *) a;

		switch (type) {
		case ACTION_GO:
		    ActionData(a, struct ActionGo *)->Direction = (uword) data1;
		    break;

		case ACTION_USE:
		case ACTION_TAKE:
		case ACTION_DROP:
		    ActionData(a, struct ActionUse *)->ItemId = data1;
		    ActionData(a, struct ActionUse *)->ToolId = data2;
		    break;

		case ACTION_SIGNAL:
		case ACTION_WAIT_SIGNAL:
		case ACTION_OPEN:
		case ACTION_CLOSE:
		case ACTION_CONTROL:
		    ActionData(a, struct ActionOpen *)->ItemId = data1;
		    break;
		}
	    }
	}
    }

    return a;
}

struct Action *CurrentAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler))
	return (struct Action *) h->CurrentAction;

    return NULL;
}

struct Action *GoFirstAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if (!LIST_EMPTY(h->Actions)) {
	    h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
	    ((struct Action *) h->CurrentAction)->Timer = 0;
	    h->Timer = 0;
	} else
	    h->CurrentAction = NULL;

	return (struct Action *) h->CurrentAction;
    }

    return NULL;
}

struct Action *GoLastAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if (!LIST_EMPTY(h->Actions)) {
	    h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
	    ((struct Action *) h->CurrentAction)->Timer =
		((struct Action *) h->CurrentAction)->TimeNeeded;
	    h->Timer = GetMaxTimer(sys);
	} else
	    h->CurrentAction = NULL;

	return (struct Action *) h->CurrentAction;
    }

    return NULL;
}

struct Action *NextAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((struct Action *) h->CurrentAction) {
	    if (((struct Action *) h->CurrentAction)->Timer ==
		((struct Action *) h->CurrentAction)->TimeNeeded) {
		if (NODE_SUCC(NODE_SUCC(h->CurrentAction))) {
		    h->CurrentAction = (NODE *) NODE_SUCC(h->CurrentAction);

		    ((struct Action *) h->CurrentAction)->Timer = 1;
		    h->Timer++;
		} else {
		    if (h->Flags & SHF_AUTOREVERS) {
			h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
			((struct Action *) h->CurrentAction)->Timer = 1;
			h->Timer++;
		    } else {
			h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
			((struct Action *) h->CurrentAction)->Timer =
			    ((struct Action *) h->CurrentAction)->TimeNeeded;
			h->Timer = GetMaxTimer(sys);

			return NULL;
		    }
		}
	    } else {
		((struct Action *) h->CurrentAction)->Timer++;
		h->Timer++;
	    }
	}

	return (struct Action *) h->CurrentAction;
    }

    return NULL;
}

struct Action *PrevAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((struct Action *) h->CurrentAction) {
	    if (((struct Action *) h->CurrentAction)->Timer == 1) {
		if (NODE_PRED(NODE_PRED(h->CurrentAction))) {
		    h->CurrentAction = (NODE *) NODE_PRED(h->CurrentAction);

		    ((struct Action *) h->CurrentAction)->Timer =
			((struct Action *) h->CurrentAction)->TimeNeeded;
		    h->Timer--;
		} else {
		    if ((h->Flags & SHF_AUTOREVERS) && h->Timer) {
			h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
			((struct Action *) h->CurrentAction)->Timer =
			    ((struct Action *) h->CurrentAction)->TimeNeeded;
			h->Timer--;
		    } else {
			h->CurrentAction = (NODE *) LIST_HEAD(h->Actions);
			((struct Action *) h->CurrentAction)->Timer = 0;
			h->Timer = 0;

			return NULL;
		    }
		}
	    } else {
		((struct Action *) h->CurrentAction)->Timer--;
		h->Timer--;
	    }
	}

	return (struct Action *) h->CurrentAction;
    }

    return NULL;
}

ubyte ActionStarted(struct System * sys)
{
    register struct Handler *h;
    register struct Action *a;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((a = (struct Action *) h->CurrentAction)) {
	    if (a->Timer == 1)
		return 1;
	}
    }

    return 0;
}

ubyte ActionEnded(struct System * sys)
{
    register struct Handler *h;
    register struct Action *a;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((a = (struct Action *) h->CurrentAction)) {
	    if (a->Timer == a->TimeNeeded)
		return 1;
	}
    }

    return 0;
}

void RemLastAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if (!LIST_EMPTY(h->Actions)) {
	    if (GetNrOfNodes(h->Actions) > 1) {
		NODE *n;

		n = (NODE *) RemTailNode(h->Actions);
		sysUsedMem -= NODE_SIZE(n);
		FreeNode(n);

		h->Timer = GetMaxTimer(sys);
		h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
		((struct Action *) h->CurrentAction)->Timer =
		    ((struct Action *) h->CurrentAction)->TimeNeeded;
	    } else
		ClearHandler(sys, h->Id);
	}
    }
}

void IgnoreAction(struct System *sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if ((struct Action *) h->CurrentAction) {
	    if (NODE_SUCC(NODE_SUCC(h->CurrentAction))) {
		h->CurrentAction = (NODE *) NODE_SUCC(h->CurrentAction);

		((struct Action *) h->CurrentAction)->Timer = 0;
	    } else {
		h->CurrentAction = (NODE *) LIST_TAIL(h->Actions);
		((struct Action *) h->CurrentAction)->Timer =
		    ((struct Action *) h->CurrentAction)->TimeNeeded;
	    }
	}
    }
}

struct plSignal *InitSignal(struct System *sys, U32 sender, U32 receiver)
{
    register struct plSignal *s = NULL;

    if (sys) {
	if ((s =
	     (struct plSignal *) CreateNode(sys->Signals, sizeof(*s), NULL))) {
	    s->SenderId = sender;
	    s->ReceiverId = receiver;
	}
    }

    return s;
}

void CloseSignal(struct plSignal *s)
{
    if (s) {
	RemNode(s);
	FreeNode(s);
    }
}

struct plSignal *IsSignal(struct System *sys, U32 sender, U32 receiver)
{
    register struct plSignal *s;

    if (sys) {
	for (s = (struct plSignal *) LIST_HEAD(sys->Signals); NODE_SUCC(s);
	     s = (struct plSignal *) NODE_SUCC(s)) {
	    if ((s->SenderId == sender) && (s->ReceiverId == receiver))
		return s;
	}
    }

    return NULL;
}

U32 CurrentTimer(struct System * sys)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler))
	return h->Timer;

    return 0L;
}

void IncCurrentTimer(struct System *sys, U32 time, ubyte alsoTime)
{
    register struct Handler *h;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	if (h->CurrentAction) {
	    ((struct Action *) h->CurrentAction)->TimeNeeded += time;

	    if (alsoTime) {
		((struct Action *) h->CurrentAction)->Timer += time;
		h->Timer += time;
	    }
	}
    }
}

U32 GetMaxTimer(struct System *sys)
{
    register struct Handler *h;
    register struct Action *a;
    register U32 time = 0;

    if (sys && (h = (struct Handler *) sys->ActivHandler)) {
	for (a = (struct Action *) LIST_HEAD(h->Actions); NODE_SUCC(a);
	     a = (struct Action *) NODE_SUCC(a))
	    time += a->TimeNeeded;
    }

    return time;
}

void ResetMem(void)
{
    sysUsedMem = 0;
}

void CorrectMem(LIST * l)
{
    NODE *n;

    for (n = LIST_HEAD(l); NODE_SUCC(n); n = NODE_SUCC(n))
	sysUsedMem -= NODE_SIZE(n);
}

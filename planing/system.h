/*
**      $Filename: planing/system.h
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.system interface for "Der Clou!"
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

#ifndef MODULE_PLANING_SYSTEM
#define MODULE_PLANING_SYSTEM

#include <stdio.h>

#include "theclou.h"

#include "memory/memory.h"
#include "error/error.h"
#include "disk/disk.h"
#include "list/list.h"
#include "data/database.h"
#include "data/relation.h"

#include "data/objstd/tcdata.h"

#include "text/text.h"
#include "landscap/landscap.h"
#include "planing/main.h"


/* System main structure - like the kernel in an OS */
struct System {
    LIST *Handlers;		/* Pointer to all started handlers */
    LIST *Signals;		/* Pointer to signal in the system */

    NODE *ActivHandler;
};

struct System *InitSystem(void);	/* Initialize system for use */
void CloseSystem(struct System *sys);	/* Close all system immedietly */
void SetActivHandler(struct System *sys, U32 id);

void SaveSystem(FILE * fh, struct System *sys);
LIST *LoadSystem(FILE * fh, struct System *sys);


/* System Handler Flags */
#define SHF_NORMAL              0
#define SHF_AUTOREVERS          1

/* Handler structure - like a task in an OS */
struct Handler {
    NODE Link;			/* Link to next handler */

    U32 Id;			/* ID of handler (all handlers will be identified with their ID and
				   not through pointers, which will save global data) */

    U32 Timer;			/* Handler time in seconds/3 */

    U32 Flags;			/* Handler flags */

    LIST *Actions;		/* Action table */
    NODE *CurrentAction;	/* Current action */
};


struct Handler *InitHandler(struct System *sys, U32 id, U32 flags);	/* Initialize handler         */
void CloseHandler(struct System *sys, U32 id);	/* Close Handler              */
struct Handler *ClearHandler(struct System *sys, U32 id);	/* Clear Handlers action list */
struct Handler *FindHandler(struct System *sys, U32 id);

ubyte IsHandlerCleared(struct System *sys);

void SaveHandler(FILE * fh, struct System *sys, U32 id);
ubyte LoadHandler(FILE * fh, struct System *sys, U32 id);

size_t plGetUsedMem(void);

/* here we are at the real part - the actions */
#define ACTION_GO                1
#define ACTION_WAIT              2
#define ACTION_SIGNAL            3
#define ACTION_WAIT_SIGNAL       4
#define ACTION_USE               5
#define ACTION_TAKE              6
#define ACTION_DROP              7
#define ACTION_OPEN              8
#define ACTION_CLOSE             9
#define ACTION_CONTROL          10

struct Action {
    NODE Link;

    uword Type;

    uword TimeNeeded;		/* times in seconds/3 */
    uword Timer;
};

/* Type : ACTION_GO 
   Figure will go in one direction for x steps */
struct ActionGo {
    uword Direction;
};

#define DIRECTION_NO    0
#define DIRECTION_LEFT  1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP    4
#define DIRECTION_DOWN  8


/* Type : ACTION_WAIT
   Figure waits for x seconds 
   Does not need an extended data structure */

/* Type : ACTION_SIGNAL
   Figure sends out a signal of a special type to a receiver */
struct ActionSignal {
    U32 ReceiverId;
};

/* Type : ACTION_WAIT_SIGNAL
   Figure waits until it receives a signal of a special type from a special sender */
struct ActionWaitSignal {
    U32 SenderId;
};

/* Type : ACTION_USE */
struct ActionUse {
    U32 ItemId;
    U32 ToolId;
};

/* Type : ACTION_TAKE */
struct ActionTake {
    U32 ItemId;
    U32 LootId;
};

/* Type : ACTION_DROP */
struct ActionDrop {
    U32 ItemId;
    U32 LootId;
};

/* Type : ACTION_OPEN */
struct ActionOpen {
    U32 ItemId;
};

/* Type : ACTION_CLOSE */
struct ActionClose {
    U32 ItemId;
};

/* Type : ACTION_CONTROL */
struct ActionControl {
    U32 ItemId;
};


#define ActionData(ac,type)      ((type)(ac+1))

struct Action *InitAction(struct System *sys, uword type, U32 data1, U32 data2,
			  U32 time);
struct Action *CurrentAction(struct System *sys);
struct Action *GoFirstAction(struct System *sys);
struct Action *GoLastAction(struct System *sys);
struct Action *NextAction(struct System *sys);
struct Action *PrevAction(struct System *sys);
ubyte ActionStarted(struct System *sys);
ubyte ActionEnded(struct System *sys);
void RemLastAction(struct System *sys);
void IgnoreAction(struct System *sys);


/* Signal structure - to make it possible to communicate between handlers, our small attempt of an message system */
#define SIGNAL_HURRY_UP    1	/* come on, get on */
#define SIGNAL_DONE        2	/* well, my work is done */
#define SIGNAL_ESCAPE      3	/* f..., police is coming, let's go */

struct plSignal {
    NODE Link;

    U32 SenderId;
    U32 ReceiverId;
};

struct plSignal *InitSignal(struct System *sys, U32 sender, U32 receiver);
void CloseSignal(struct plSignal *sig);
struct plSignal *IsSignal(struct System *sys, U32 sender, U32 receiver);


U32 CurrentTimer(struct System *sys);
void IncCurrentTimer(struct System *sys, U32 time, ubyte alsoTime);
U32 GetMaxTimer(struct System *sys);

void ResetMem(void);
void CorrectMem(LIST * l);
#endif

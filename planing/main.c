/*
**      $Filename: planing/main.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.main for "Der Clou!"
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

#include "planing/main.h"


struct System *plSys = NULL;

char txtTooLoud[20];
char txtTimer[20];
char txtWeight[20];
char txtSeconds[20];


/* System functions */
void plInit(void)
{
    LIST *l;

    /* Get texts */
    l = txtGoKey(PLAN_TXT, "TXT_TOO_LOUD");
    sprintf(txtTooLoud, "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_TIMER");
    sprintf(txtTimer, "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_WEIGHT");
    sprintf(txtWeight, "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    l = txtGoKey(PLAN_TXT, "TXT_SECONDS");
    sprintf(txtSeconds, "%s", NODE_NAME(LIST_HEAD(l)));
    RemoveList(l);

    plSys = InitSystem();
}

void plDone(void)
{
    CloseSystem(plSys);
}

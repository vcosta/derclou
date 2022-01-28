/*
**      $Filename: planing/io.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.io for "Der Clou!"
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

#include "base/base.h"

#include "planing/io.h"


struct IOData {
    NODE io_Link;
    ubyte io_Data;
};


static char *Planing_Open[3] = { "rb", "wb", "rb" };


/* Loading & saving functions */
void plSaveTools(FILE * fh)
{
    if (fh) {
	struct ObjectNode *n;

	hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

	fprintf(fh, PLANING_PLAN_TOOL_BEGIN_ID);
	fprintf(fh, "\r\n");

	for (n = (struct ObjectNode *) LIST_HEAD(ObjectList); NODE_SUCC(n);
	     n = (struct ObjectNode *) NODE_SUCC(n))
	    fprintf(fh, "%" PRIu32 "\r\n", OL_NR(n));

	fprintf(fh, PLANING_PLAN_TOOL_END_ID);
	fprintf(fh, "\r\n");
    }
}

LIST *plLoadTools(FILE * fh)
{
    register LIST *l = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_1");
    register ubyte foundAll = 1, canGet = 2, toolsNr = 0;
    char buffer[64];
    U32 id;

    buffer[0] = '\0';

    if (fh) {
	while (dskGetLine(buffer, sizeof(buffer), fh)
	       && strcmp(buffer, PLANING_PLAN_TOOL_END_ID) != 0) {
	    if (sscanf(buffer, "%" SCNu32 "\r\n", &id) == 1) {
		toolsNr++;

		if (!has(Person_Matt_Stuvysunt, id)) {
		    if (has(Person_Mary_Bolton, id)) {
			canGet++;
			dbAddObjectNode(l, id, OLF_INCLUDE_NAME);
		    }

		    foundAll = 0;
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

	if (canGet == 2)
	    extList = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_3");
	else {
	    if ((toolsNr - canGet) > 1)
		extList =
		    txtGoKeyAndInsert(PLAN_TXT, "SYSTEM_TOOLS_MISSING_2",
				      (U32) (toolsNr - canGet));
	    else if (toolsNr - canGet)
		extList = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_4");
	}

	if (extList) {
	    for (n = LIST_HEAD(extList); NODE_SUCC(n); n = NODE_SUCC(n))
		CreateNode(l, 0, NODE_NAME(n));

	    RemoveList(extList);
	}

    }

    return l;
}

ubyte plOpen(U32 objId, ubyte mode, FILE ** fh)
{
    if (GamePlayMode & GP_GUARD_DESIGN) {
	if (grdInit(fh, Planing_Open[mode], objId, lsGetActivAreaID()))
	    return PLANING_OPEN_OK;
    } else {
	register LIST *PlanList;
	register ubyte i;
	FILE *pllFh;
	U32 pllData = 0;
	char pllPath[DSK_PATH_MAX], pllPath2[DSK_PATH_MAX],
	    name1[TXT_KEY_LENGTH], name2[TXT_KEY_LENGTH], exp[TXT_KEY_LENGTH];

	dbGetObjectName(lsGetActivAreaID(), name1);
	name1[strlen(name1) - 1] = '\0';

	/* MOD 25-04-94 HG - new paths on pc */
	sprintf(name2, "%s%s", name1, PLANING_PLAN_LIST_EXTENSION);

	dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2, pllPath);

	if ((pllFh = dskOpen(pllPath, "rb"))) {
	    fscanf(pllFh, "%" SCNu32, &pllData);
	    dskClose(pllFh);

	    if ((mode == PLANING_OPEN_WRITE_PLAN) || pllData) {
		sprintf(name2, "MODE_%d", mode);
		plMessage(name2, PLANING_MSG_REFRESH);

		PlanList = CreateList();

		dbGetObjectName(objId, name1);

		exp[0] = EOS;

		for (i = 0; i < PLANING_NR_PLANS; i++) {
		    if ((mode == PLANING_OPEN_WRITE_PLAN)
			|| (pllData & (1L << i))) {
			struct IOData *data;

			if (mode == PLANING_OPEN_WRITE_PLAN) {
			    if (pllData & (1L << i))
				txtGetFirstLine(PLAN_TXT, "ATTENTION_1", exp);
			    else
				txtGetFirstLine(PLAN_TXT, "ATTENTION_2", exp);
			}

			sprintf(name2, "*%s Plan %d    %s", name1, i + 1, exp);

			if ((data =
			     CreateNode(PlanList, sizeof(struct IOData),
					name2)))
			    data->io_Data = i;
		    }
		}

		sprintf(name2, "EXPAND_MODE_%d", mode);
		txtGetFirstLine(PLAN_TXT, name2, exp);
		ExpandObjectList(PlanList, exp);

		i = Bubble(PlanList, 0, NULL, 0L);

		if (ChoiceOk(i, GET_OUT, PlanList)) {
		    struct IOData *data;

		    if ((data = GetNthNode(PlanList, i)))
			i = data->io_Data;

		    dbGetObjectName(lsGetActivAreaID(), name1);
		    name1[strlen(name1) - 1] = '\0';

		    sprintf(name2, "%s%d%s", name1, i + 1,
			    PLANING_PLAN_EXTENSION);

		    if(!dskBuildPathName(DISK_CHECK_FILE, DATADISK, name2, pllPath2)) {
		        /* allow for uninitialized (non-existent) save files. */
		        dskBuildPathName(DISK_CHECK_DIR, DATADISK, name2, pllPath2);
		    }

		    *fh = dskOpen(pllPath2, Planing_Open[mode]);

		    if (mode == PLANING_OPEN_WRITE_PLAN) {
			pllData |= 1L << i;

			if ((pllFh = dskOpen(pllPath, "wb"))) {
			    fprintf(pllFh, "%" PRIu32, pllData);
			    dskClose(pllFh);
			}
		    }

		    if (*fh) {
			RemoveList(PlanList);
			return PLANING_OPEN_OK;
		    } else
			ErrorMsg(Disk_Defect, ERROR_MODULE_PLANING, 0);
		}

		RemoveList(PlanList);

		return PLANING_OPEN_ERR_NO_CHOICE;
	    }

	    return PLANING_OPEN_ERR_NO_PLAN;
	}
    }

    ErrorMsg(Disk_Defect, ERROR_MODULE_PLANING, 0);
    return 0;
}

void plSave(U32 objId)
{
    FILE *fh = NULL;

    if (plOpen(objId, PLANING_OPEN_WRITE_PLAN, &fh) == PLANING_OPEN_OK) {
	if (GamePlayMode & GP_GUARD_DESIGN) {
	    grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr,
		  GUARDS_DO_SAVE);
	} else {
	    ubyte i;

	    SaveSystem(fh, plSys);
	    plSaveTools(fh);

	    for (i = 0; i < BurglarsNr; i++)
		SaveHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
	}

	dskClose(fh);
    }
}

void plSaveChanged(U32 objId)
{
    if (PlanChanged) {
	LIST *l = txtGoKey(PLAN_TXT, "PLAN_CHANGED");

	inpTurnESC(0);

	if (Bubble(l, 0, NULL, 0L) == 0) {
	    inpTurnESC(1);

	    if (!plAllInCar(objId))
		plSay("PLAN_NOT_FINISHED", 0);

	    plSave(objId);
	} else
	    inpTurnESC(1);

	RemoveList(l);
    }
}

void plLoad(U32 objId)
{
    FILE *fh = NULL;
    ubyte ret;

    if (objId == Building_Starford_Kaserne)
	while ((ret =
		plOpen(objId, PLANING_OPEN_READ_PLAN, &fh)) != PLANING_OPEN_OK);
    else
	ret = plOpen(objId, PLANING_OPEN_READ_PLAN, &fh);

    if (ret == PLANING_OPEN_OK) {
	if (GamePlayMode & GP_GUARD_DESIGN)
	    grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr,
		  GUARDS_DO_LOAD);
	else {
	    LIST *l = NULL;
	    ubyte i;
	    ubyte goon = 1;

	    if ((l = LoadSystem(fh, plSys))) {
		inpTurnESC(0);
		Bubble(l, 0, NULL, 0L);
		inpTurnESC(1);
		RemoveList(l);

		goon = 0;
		l = NULL;
	    }

	    if ((l = plLoadTools(fh))) {
		inpTurnESC(0);
		Bubble(l, 0, NULL, 0L);
		inpTurnESC(1);
		RemoveList(l);

		goon = 0;
	    }

	    if (goon) {
		for (i = 0; i < BurglarsNr; i++)
		    LoadHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
	    }
	}

	dskClose(fh);
    } else {
	if (ret == PLANING_OPEN_ERR_NO_PLAN)
	    plMessage("NO_PLAN", PLANING_MSG_REFRESH | PLANING_MSG_WAIT);
    }
}

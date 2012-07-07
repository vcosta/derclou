/*
**      $Filename: planing/sync.c
**      $Release:  1
**      $Revision: 0
**      $Date:     23-04-94
**
**      planing.sync for "Der Clou!"
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

#include "planing/sync.h"


/* Sync functions */
static uword plXMoveSync(U32 id, uword xpos, ubyte animate, ubyte direction,
		         uword gowhere)
{
    if (direction) {
	if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
	    lsInitScrollLandScape(gowhere, LS_SCROLL_PREPARE);

	switch (gowhere) {
	case DIRECTION_LEFT:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_LEFT,
			   -1 * LS_STD_SCROLL_SPEED, 0);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    xpos = (uword) (xpos - LS_STD_SCROLL_SPEED);
	    break;

	case DIRECTION_RIGHT:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
			   1 * LS_STD_SCROLL_SPEED, 0);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    xpos = (uword) (xpos + LS_STD_SCROLL_SPEED);
	    break;
	}
    } else {
	if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson)) {
	    switch (gowhere) {
	    case DIRECTION_LEFT:
		lsInitScrollLandScape(DIRECTION_RIGHT, LS_SCROLL_PREPARE);
		break;

	    case DIRECTION_RIGHT:
		lsInitScrollLandScape(DIRECTION_LEFT, LS_SCROLL_PREPARE);
		break;
	    }
	}

	switch (gowhere) {
	case DIRECTION_LEFT:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_LEFT,
			   1 * LS_STD_SCROLL_SPEED, 0);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    xpos = (uword) (xpos + LS_STD_SCROLL_SPEED);
	    break;

	case DIRECTION_RIGHT:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_RIGHT,
			   -1 * LS_STD_SCROLL_SPEED, 0);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    xpos = (uword) (xpos - LS_STD_SCROLL_SPEED);
	    break;
	}
    }

    return xpos;
}

static uword plYMoveSync(U32 id, uword ypos, ubyte animate, ubyte direction,
		         uword gowhere)
{
    if (direction) {
	if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
	    lsInitScrollLandScape(gowhere, LS_SCROLL_PREPARE);

	switch (gowhere) {
	case DIRECTION_UP:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_UP, 0,
			   -1 * LS_STD_SCROLL_SPEED);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    ypos = (uword) (ypos - LS_STD_SCROLL_SPEED);
	    break;

	case DIRECTION_DOWN:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
			   1 * LS_STD_SCROLL_SPEED);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    ypos = (uword) (ypos + LS_STD_SCROLL_SPEED);
	    break;
	}
    } else {
	if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson)) {
	    switch (gowhere) {
	    case DIRECTION_UP:
		lsInitScrollLandScape(DIRECTION_DOWN, LS_SCROLL_PREPARE);
		break;

	    case DIRECTION_DOWN:
		lsInitScrollLandScape(DIRECTION_UP, LS_SCROLL_PREPARE);
		break;
	    }
	}

	switch (gowhere) {
	case DIRECTION_UP:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_UP, 0,
			   1 * LS_STD_SCROLL_SPEED);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    ypos = (uword) (ypos + LS_STD_SCROLL_SPEED);
	    break;

	case DIRECTION_DOWN:
	    if (animate & PLANING_ANIMATE_STD)
		livAnimate(Planing_Name[id], ANM_MOVE_DOWN, 0,
			   -1 * LS_STD_SCROLL_SPEED);

	    if ((animate & PLANING_ANIMATE_FOCUS) && (id == CurrentPerson))
		lsScrollLandScape((ubyte) - 1);

	    ypos = (uword) (ypos - LS_STD_SCROLL_SPEED);
	    break;
	}
    }

    return ypos;
}

void plSync(ubyte animate, U32 targetTime, U32 times, ubyte direction)
{
    ubyte i;
    U32 seconds, lastAreaId = 0;

    for (seconds = 0; seconds < times; seconds++) {
	for (i = 0; i < PersonsNr; i++) {
	    struct Action *action;
	    uword xpos = livGetXPos(Planing_Name[i]);
	    uword ypos = livGetYPos(Planing_Name[i]);

	    SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, i)));

	    if (CurrentTimer(plSys) == targetTime)
		continue;

	    if (!direction && (i < BurglarsNr)) {
		if (GetMaxTimer(plSys) < (targetTime + times - seconds))
		    continue;
	    }

	    if ((i >= BurglarsNr) && Planing_Guard[i - BurglarsNr]) {
		if (animate & PLANING_ANIMATE_STD) {
		    switch (Planing_Guard[i - BurglarsNr]) {
		    case 1:
			livAnimate(Planing_Name[i],
				   livGetViewDirection(Planing_Name[i]), 0, 0);
			break;

		    case 2:
			livAnimate(Planing_Name[i], ANM_DUSEL_POLICE, 0, 0);
			break;
		    }
		}
	    } else {
		if (direction)
		    action = NextAction(plSys);
		else
		    action = CurrentAction(plSys);

		if (!action) {
		    if (direction)
			GoLastAction(plSys);
		    else
			GoFirstAction(plSys);

		    if (animate & PLANING_ANIMATE_STD)
			livAnimate(Planing_Name[i], ANM_STAND, 0, 0);
		} else {
		    if (action->Type != ACTION_GO) {
			if (animate & PLANING_ANIMATE_STD) {
			    if (i < BurglarsNr) {
				if ((action->Type == ACTION_WAIT)
				    || (action->Type == ACTION_WAIT_SIGNAL))
				    livAnimate(Planing_Name[i], ANM_STAND, 0,
					       0);
				else if (action->Type == ACTION_SIGNAL)
				    livAnimate(Planing_Name[i], ANM_MAKE_CALL,
					       0, 0);
				else
				    plWork(i);
			    } else
				livAnimate(Planing_Name[i], ANM_WORK_CONTROL, 0,
					   0);
			}
		    }

		    switch (action->Type) {
		    case ACTION_GO:
			xpos =
			    plXMoveSync(i, xpos, animate, direction,
					ActionData(action,
						   struct ActionGo *)->
					Direction);
			ypos =
			    plYMoveSync(i, ypos, animate, direction,
					ActionData(action,
						   struct ActionGo *)->
					Direction);
			break;

		    case ACTION_USE:
			if (ActionStarted(plSys)) {
			    if (plIsStair
				(ActionData(action, struct ActionUse *)->
				 ItemId)) {
				if (direction)
				    livLivesInArea(Planing_Name[i],
						   StairConnectsGet(ActionData
								    (action,
								     struct
								     ActionUse
								     *)->ItemId,
								    ActionData
								    (action,
								     struct
								     ActionUse
								     *)->
								    ItemId));
				else
				    livLivesInArea(Planing_Name[i],
						   ActionData(action,
							      struct ActionUse
							      *)->ToolId);
			    } else
				if (dbIsObject
				    (ActionData(action, struct ActionUse *)->
				     ItemId, Object_Police)) {
				Police pol =
				    (Police)
				    dbGetObject(ActionData
						(action,
						 struct ActionUse *)->ItemId);

				if (direction)
				    Planing_Guard[pol->LivingID - BurglarsNr] =
					1;
				else
				    Planing_Guard[pol->LivingID - BurglarsNr] =
					0;
			    } else {
				if (direction)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionUse *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);
				else
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionUse *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);
			    }
			}

			if (ActionEnded(plSys)) {
			    if (plIsStair
				(ActionData(action, struct ActionUse *)->
				 ItemId)) {
				if (i == CurrentPerson) {
				    if (direction)
					lastAreaId =
					    StairConnectsGet(ActionData
							     (action,
							      struct ActionUse
							      *)->ItemId,
							     ActionData(action,
									struct
									ActionUse
									*)->
							     ItemId);
				    else
					lastAreaId =
					    ActionData(action,
						       struct ActionUse *)->
					    ToolId;
				}
			    } else
				if (dbIsObject
				    (ActionData(action, struct ActionUse *)->
				     ItemId, Object_Police)) {
				Police pol =
				    (Police)
				    dbGetObject(ActionData
						(action,
						 struct ActionUse *)->ItemId);

				if (direction)
				    Planing_Guard[pol->LivingID - BurglarsNr] =
					2;
				else
				    Planing_Guard[pol->LivingID - BurglarsNr] =
					1;
			    } else {
				if (direction) {
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionUse *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);

				    if (!plIgnoreLock
					(ActionData
					 (action,
					  struct ActionUse *)->ItemId)) {
					if (!CHECK_STATE
					    (lsGetObjectState
					     (ActionData
					      (action,
					       struct ActionUse *)->ItemId),
					     Const_tcLOCK_UNLOCK_BIT)) {
					    lsSetObjectState(ActionData
							     (action,
							      struct ActionUse
							      *)->ItemId,
							     Const_tcLOCK_UNLOCK_BIT,
							     1);

					    if (((Tool)
						 dbGetObject(ActionData
							     (action,
							      struct ActionUse
							      *)->ToolId))->
						Effect & Const_tcTOOL_OPENS) {
						lsSetObjectState(ActionData
								 (action,
								  struct
								  ActionUse *)->
								 ItemId,
								 Const_tcOPEN_CLOSE_BIT,
								 1);
						plCorrectOpened((LSObject)
								dbGetObject
								(ActionData
								 (action,
								  struct
								  ActionUse *)->
								 ItemId), 1);
					    }
					} else {
					    if ((((LSObject)
						  dbGetObject(ActionData
							      (action,
							       struct ActionUse
							       *)->ItemId))->
						 Type == Item_Fenster)) {
						lsWalkThroughWindow((LSObject)
								    dbGetObject
								    (ActionData
								     (action,
								      struct
								      ActionUse
								      *)->
								     ItemId),
								    xpos, ypos,
								    &xpos,
								    &ypos);
						livSetPos(Planing_Name[i], xpos,
							  ypos);

						livRefreshAll();
					    }
					}
				    }
				} else {
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionUse *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);

				    if (!plIgnoreLock
					(ActionData
					 (action,
					  struct ActionUse *)->ItemId)) {
					if (CHECK_STATE
					    (lsGetObjectState
					     (ActionData
					      (action,
					       struct ActionUse *)->ItemId),
					     Const_tcLOCK_UNLOCK_BIT)) {
					    if ((((LSObject)
						  dbGetObject(ActionData
							      (action,
							       struct ActionUse
							       *)->ItemId))->
						 Type == Item_Fenster)
						&& !ActionData(action,
							       struct ActionUse
							       *)->ToolId) {
						lsWalkThroughWindow((LSObject)
								    dbGetObject
								    (ActionData
								     (action,
								      struct
								      ActionUse
								      *)->
								     ItemId),
								    xpos, ypos,
								    &xpos,
								    &ypos);
						livSetPos(Planing_Name[i], xpos,
							  ypos);

						livRefreshAll();
					    } else {
						lsSetObjectState(ActionData
								 (action,
								  struct
								  ActionUse *)->
								 ItemId,
								 Const_tcLOCK_UNLOCK_BIT,
								 0);

						if (((Tool)
						     dbGetObject(ActionData
								 (action,
								  struct
								  ActionUse *)->
								 ToolId))->
						    Effect & Const_tcTOOL_OPENS)
						{
						    lsSetObjectState(ActionData
								     (action,
								      struct
								      ActionUse
								      *)->
								     ItemId,
								     Const_tcOPEN_CLOSE_BIT,
								     0);
						    plCorrectOpened((LSObject)
								    dbGetObject
								    (ActionData
								     (action,
								      struct
								      ActionUse
								      *)->
								     ItemId),
								    0);
						}
					    }
					}
				    }
				}

				if (plIgnoreLock
				    (ActionData(action, struct ActionUse *)->
				     ItemId)) {
				    U32 state =
					lsGetObjectState(ActionData
							 (action,
							  struct ActionUse *)->
							 ItemId);

				    if (CHECK_STATE(state, Const_tcON_OFF)) {
					lsSetObjectState(ActionData(action, struct ActionUse *)->ItemId, Const_tcON_OFF, 0);	/* on setzen  */

					if (plIgnoreLock
					    (ActionData
					     (action,
					      struct ActionUse *)->ItemId) ==
					    PLANING_POWER) {
					    lsSetSpotStatus(ActionData
							    (action,
							     struct ActionUse
							     *)->ItemId,
							    LS_SPOT_ON);
					    lsShowAllSpots(CurrentTimer(plSys),
							   LS_ALL_VISIBLE_SPOTS);
					}
				    } else {
					lsSetObjectState(ActionData(action, struct ActionUse *)->ItemId, Const_tcON_OFF, 1);	/* off setzen */

					if (plIgnoreLock
					    (ActionData
					     (action,
					      struct ActionUse *)->ItemId) ==
					    PLANING_POWER) {
					    lsSetSpotStatus(ActionData
							    (action,
							     struct ActionUse
							     *)->ItemId,
							    LS_SPOT_OFF);
					    lsShowAllSpots(CurrentTimer(plSys),
							   LS_ALL_INVISIBLE_SPOTS);
					}
				    }
				}

				plRefresh(ActionData
					  (action, struct ActionUse *)->ItemId);
			    }
			}
			break;

		    case ACTION_TAKE:
			if (ActionStarted(plSys)) {
			    if (direction)
				lsSetObjectState(ActionData
						 (action,
						  struct ActionTake *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);
			    else
				lsSetObjectState(ActionData
						 (action,
						  struct ActionTake *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 0);
			}

			if (ActionEnded(plSys)) {
			    if (direction) {
				U32 weightLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionTake *)->
						 LootId))->Weight;
				U32 volumeLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionTake *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (action,
						  struct ActionTake *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 0);

				if ((ActionData(action, struct ActionTake *)->
				     ItemId >= 9701)
				    &&
				    (ActionData(action, struct ActionTake *)->
				     ItemId <= 9708)) {
				    lsRemLootBag(ActionData
						 (action,
						  struct ActionTake *)->ItemId);
				    Planing_Loot[ActionData
						 (action,
						  struct ActionTake *)->ItemId -
						 9701] = 0;
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT)) {
					lsTurnObject((LSObject)
						     dbGetObject(ActionData
								 (action,
								  struct
								  ActionTake
								  *)->ItemId),
						     LS_OBJECT_INVISIBLE,
						     LS_NO_COLLISION);
					lsSetObjectState(ActionData
							 (action,
							  struct ActionTake *)->
							 ItemId,
							 Const_tcACCESS_BIT, 0);
				    }
				}

				{
				    U32 newValue =
					GetP(dbGetObject
					     (ActionData
					      (action,
					       struct ActionTake *)->ItemId),
					     hasLoot(i),
					     dbGetObject(ActionData
							 (action,
							  struct ActionTake *)->
							 LootId));

				    if (Ask
					(dbGetObject
					 (OL_NR(GetNthNode(BurglarsList, i))),
					 take_RelId,
					 dbGetObject(ActionData
						     (action,
						      struct ActionTake *)->
						     LootId))) {
					U32 oldValue =
					    GetP(dbGetObject
						 (OL_NR
						  (GetNthNode
						   (BurglarsList, i))),
						 take_RelId,
						 dbGetObject(ActionData
							     (action,
							      struct ActionTake
							      *)->LootId));

					SetP(dbGetObject
					     (OL_NR
					      (GetNthNode(BurglarsList, i))),
					     take_RelId,
					     dbGetObject(ActionData
							 (action,
							  struct ActionTake *)->
							 LootId),
					     oldValue + newValue);
				    } else
					SetP(dbGetObject
					     (OL_NR
					      (GetNthNode(BurglarsList, i))),
					     take_RelId,
					     dbGetObject(ActionData
							 (action,
							  struct ActionTake *)->
							 LootId), newValue);
				}

				UnSet(dbGetObject
				      (ActionData(action, struct ActionTake *)->
				       ItemId), hasLoot(i),
				      dbGetObject(ActionData
						  (action,
						   struct ActionTake *)->
						  LootId));

				Planing_Weight[i] += weightLoot;
				Planing_Volume[i] += volumeLoot;
			    } else {
				U32 weightLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionTake *)->
						 LootId))->Weight;
				U32 volumeLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionTake *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (action,
						  struct ActionTake *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);

				if ((ActionData(action, struct ActionTake *)->
				     ItemId >= 9701)
				    &&
				    (ActionData(action, struct ActionTake *)->
				     ItemId <= 9708)) {
				    lsAddLootBag(xpos, ypos,
						 ActionData(action,
							    struct ActionTake
							    *)->ItemId - 9700);
				    Planing_Loot[ActionData
						 (action,
						  struct ActionTake *)->ItemId -
						 9701] = 1;
				    SetP(dbGetObject
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 hasLoot(i),
					 dbGetObject(ActionData
						     (action,
						      struct ActionTake *)->
						     LootId),
					 GetP(dbGetObject
					      (OL_NR
					       (GetNthNode(PersonsList, i))),
					      take_RelId,
					      dbGetObject(ActionData
							  (action,
							   struct ActionTake
							   *)->LootId)));
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT)) {
					lsTurnObject((LSObject)
						     dbGetObject(ActionData
								 (action,
								  struct
								  ActionTake
								  *)->ItemId),
						     LS_OBJECT_VISIBLE,
						     LS_COLLISION);
					lsSetObjectState(ActionData
							 (action,
							  struct ActionTake *)->
							 ItemId,
							 Const_tcACCESS_BIT, 1);
				    }

				    SetP(dbGetObject
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 hasLoot(i),
					 dbGetObject(ActionData
						     (action,
						      struct ActionTake *)->
						     LootId),
					 GetP(dbGetObject
					      (ActionData
					       (action,
						struct ActionTake *)->ItemId),
					      hasLoot_Clone_RelId,
					      dbGetObject(ActionData
							  (action,
							   struct ActionTake
							   *)->LootId)));
				}

				UnSet(dbGetObject
				      (OL_NR(GetNthNode(PersonsList, i))),
				      take_RelId,
				      dbGetObject(ActionData
						  (action,
						   struct ActionTake *)->
						  LootId));
				Planing_Weight[i] -= weightLoot;
				Planing_Volume[i] -= volumeLoot;
			    }

			    plRefresh(ActionData(action, struct ActionTake *)->
				      ItemId);
			}
			break;

		    case ACTION_DROP:
			if (ActionStarted(plSys)) {
			    if (direction)
				lsSetObjectState(ActionData
						 (action,
						  struct ActionDrop *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);
			    else
				lsSetObjectState(ActionData
						 (action,
						  struct ActionDrop *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 0);
			}

			if (ActionEnded(plSys)) {
			    if (direction) {
				U32 weightLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionDrop *)->
						 LootId))->Weight;
				U32 volumeLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionDrop *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (action,
						  struct ActionDrop *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 1);

				if ((ActionData(action, struct ActionDrop *)->
				     ItemId >= 9701)
				    &&
				    (ActionData(action, struct ActionDrop *)->
				     ItemId <= 9708)) {
				    lsAddLootBag(xpos, ypos,
						 ActionData(action,
							    struct ActionDrop
							    *)->ItemId - 9700);
				    Planing_Loot[ActionData
						 (action,
						  struct ActionDrop *)->ItemId -
						 9701] = 1;
				    SetP(dbGetObject
					 (ActionData
					  (action,
					   struct ActionDrop *)->ItemId),
					 hasLoot(i),
					 dbGetObject(ActionData
						     (action,
						      struct ActionDrop *)->
						     LootId),
					 GetP(dbGetObject
					      (OL_NR
					       (GetNthNode(PersonsList, i))),
					      take_RelId,
					      dbGetObject(ActionData
							  (action,
							   struct ActionDrop
							   *)->LootId)));
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT)) {
					lsTurnObject((LSObject)
						     dbGetObject(ActionData
								 (action,
								  struct
								  ActionDrop
								  *)->ItemId),
						     LS_OBJECT_VISIBLE,
						     LS_COLLISION);
					lsSetObjectState(ActionData
							 (action,
							  struct ActionDrop *)->
							 ItemId,
							 Const_tcACCESS_BIT, 1);
				    }

				    SetP(dbGetObject
					 (ActionData
					  (action,
					   struct ActionDrop *)->ItemId),
					 hasLoot(i),
					 dbGetObject(ActionData
						     (action,
						      struct ActionDrop *)->
						     LootId),
					 GetP(dbGetObject
					      (ActionData
					       (action,
						struct ActionDrop *)->ItemId),
					      hasLoot_Clone_RelId,
					      dbGetObject(ActionData
							  (action,
							   struct ActionDrop
							   *)->LootId)));
				}

				UnSet(dbGetObject
				      (OL_NR(GetNthNode(PersonsList, i))),
				      take_RelId,
				      dbGetObject(ActionData
						  (action,
						   struct ActionDrop *)->
						  LootId));
				Planing_Weight[i] -= weightLoot;
				Planing_Volume[i] -= volumeLoot;
			    } else {
				U32 weightLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionDrop *)->
						 LootId))->Weight;
				U32 volumeLoot =
				    ((Loot)
				     dbGetObject(ActionData
						 (action,
						  struct ActionDrop *)->
						 LootId))->Volume;

				lsSetObjectState(ActionData
						 (action,
						  struct ActionDrop *)->ItemId,
						 Const_tcIN_PROGRESS_BIT, 0);

				if ((ActionData(action, struct ActionDrop *)->
				     ItemId >= 9701)
				    &&
				    (ActionData(action, struct ActionDrop *)->
				     ItemId <= 9708)) {
				    lsRemLootBag(ActionData
						 (action,
						  struct ActionDrop *)->ItemId);
				    Planing_Loot[ActionData
						 (action,
						  struct ActionDrop *)->ItemId -
						 9701] = 0;
				} else {
				    if (CHECK_STATE
					(lsGetObjectState
					 (ActionData
					  (action,
					   struct ActionTake *)->ItemId),
					 Const_tcTAKE_BIT)) {
					lsTurnObject((LSObject)
						     dbGetObject(ActionData
								 (action,
								  struct
								  ActionTake
								  *)->ItemId),
						     LS_OBJECT_INVISIBLE,
						     LS_NO_COLLISION);
					lsSetObjectState(ActionData
							 (action,
							  struct ActionTake *)->
							 ItemId,
							 Const_tcACCESS_BIT, 0);
				    }
				}

				{
				    U32 newValue =
					GetP(dbGetObject
					     (ActionData
					      (action,
					       struct ActionDrop *)->ItemId),
					     hasLoot(i),
					     dbGetObject(ActionData
							 (action,
							  struct ActionDrop *)->
							 LootId));

				    if (Ask
					(dbGetObject
					 (OL_NR(GetNthNode(BurglarsList, i))),
					 take_RelId,
					 dbGetObject(ActionData
						     (action,
						      struct ActionDrop *)->
						     LootId))) {
					U32 oldValue =
					    GetP(dbGetObject
						 (OL_NR
						  (GetNthNode
						   (BurglarsList, i))),
						 take_RelId,
						 dbGetObject(ActionData
							     (action,
							      struct ActionDrop
							      *)->LootId));

					SetP(dbGetObject
					     (OL_NR
					      (GetNthNode(BurglarsList, i))),
					     take_RelId,
					     dbGetObject(ActionData
							 (action,
							  struct ActionDrop *)->
							 LootId),
					     oldValue + newValue);
				    } else
					SetP(dbGetObject
					     (OL_NR
					      (GetNthNode(BurglarsList, i))),
					     take_RelId,
					     dbGetObject(ActionData
							 (action,
							  struct ActionDrop *)->
							 LootId), newValue);
				}

				UnSet(dbGetObject
				      (ActionData(action, struct ActionDrop *)->
				       ItemId), hasLoot(i),
				      dbGetObject(ActionData
						  (action,
						   struct ActionDrop *)->
						  LootId));

				Planing_Weight[i] += weightLoot;
				Planing_Volume[i] += volumeLoot;
			    }

			    plRefresh(ActionData(action, struct ActionDrop *)->
				      ItemId);
			}
			break;

		    case ACTION_OPEN:
			if (ActionStarted(plSys)) {
			    if (i < BurglarsNr) {
				if (direction)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionOpen *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);
				else
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionOpen *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);
			    }
			}

			if (ActionEnded(plSys)) {
			    if (direction) {
				if (i < BurglarsNr)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionOpen *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);

				lsSetObjectState(ActionData
						 (action,
						  struct ActionOpen *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 1);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (action,
							     struct ActionOpen
							     *)->ItemId), 1);
			    } else {
				if (i < BurglarsNr)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionOpen *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);

				lsSetObjectState(ActionData
						 (action,
						  struct ActionOpen *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 0);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (action,
							     struct ActionOpen
							     *)->ItemId), 0);
			    }

			    plRefresh(ActionData(action, struct ActionOpen *)->
				      ItemId);
			}
			break;

		    case ACTION_CLOSE:
			if (ActionStarted(plSys)) {
			    if (i < BurglarsNr) {
				if (direction)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionClose *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);
				else
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionClose *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);
			    }
			}

			if (ActionEnded(plSys)) {
			    if (direction) {
				if (i < BurglarsNr)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionClose *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     0);

				lsSetObjectState(ActionData
						 (action,
						  struct ActionClose *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 0);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (action,
							     struct ActionClose
							     *)->ItemId), 0);
			    } else {
				if (i < BurglarsNr)
				    lsSetObjectState(ActionData
						     (action,
						      struct ActionClose *)->
						     ItemId,
						     Const_tcIN_PROGRESS_BIT,
						     1);

				lsSetObjectState(ActionData
						 (action,
						  struct ActionClose *)->ItemId,
						 Const_tcOPEN_CLOSE_BIT, 1);

				plCorrectOpened((LSObject)
						dbGetObject(ActionData
							    (action,
							     struct ActionClose
							     *)->ItemId), 1);
			    }

			    plRefresh(ActionData(action, struct ActionClose *)->
				      ItemId);
			}
			break;
		    }
		}

		if (!direction)
		    action = PrevAction(plSys);
	    }

	    if (animate & PLANING_ANIMATE_NO) {
		register struct Action *a;
		register struct Handler *h =
		    (struct Handler *) plSys->ActivHandler;
		register uword dir;

		if (i < BurglarsNr)
		    dir = ANM_STAND;
		else
		    dir = ANM_MOVE_DOWN;

		if (CurrentTimer(plSys) != 0) {
		    for (a = (struct Action *) LIST_HEAD(h->Actions);
			 NODE_SUCC(a); a = (struct Action *) NODE_SUCC(a)) {
			switch (a->Type) {
			case ACTION_GO:
			    switch (ActionData(a, struct ActionGo *)->Direction) {
			    case DIRECTION_LEFT:
				dir = ANM_MOVE_LEFT;
				break;

			    case DIRECTION_RIGHT:
				dir = ANM_MOVE_RIGHT;
				break;

			    case DIRECTION_UP:
				dir = ANM_MOVE_UP;
				break;

			    case DIRECTION_DOWN:
				dir = ANM_MOVE_DOWN;
				break;
			    }
			    break;

			case ACTION_WAIT:
			case ACTION_SIGNAL:
			case ACTION_WAIT_SIGNAL:
			case ACTION_CONTROL:
			    dir = ANM_MOVE_DOWN;
			    break;
			}

			if (a == (struct Action *) h->CurrentAction)
			    break;
		    }
		}

		if ((i < BurglarsNr)
		    || ((i >= BurglarsNr) && !Planing_Guard[i - BurglarsNr]))
		    livAnimate(Planing_Name[i], dir, 0, 0);

		livSetPos(Planing_Name[i], xpos, ypos);
	    }
	}
    }

    SetActivHandler(plSys, OL_NR(GetNthNode(PersonsList, CurrentPerson)));

    if ((lastAreaId) && (lastAreaId != lsGetActivAreaID())) {
	lsDoneActivArea(lastAreaId);
	lsInitActivArea(lastAreaId, livGetXPos(Planing_Name[CurrentPerson]),
			livGetYPos(Planing_Name[CurrentPerson]),
			Planing_Name[CurrentPerson]);

	if (lsGetStartArea() == lsGetActivAreaID())
	    lsShowEscapeCar();	/* Auto neu zeichnen */

	livRefreshAll();
    }
}

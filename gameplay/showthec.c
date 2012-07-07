/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

U32 ShowTheClouRequester(S32 error_class)
{
    LIST *menu, *games;
    U32 answer;
    ubyte activ, line[255];

    StopAnim();

    ClearArea(AREA_LEFT);

    TurnESC(0);

    ShowMenuBackground();

    switch (error_class) {
    case No_Error:

	PreparePict(16);
	PreparePict(17);
	PreparePict(20);

	ShowMenuBackground();

	GetFirstLine(THECLOU_TXT, "Pause", line);

	PrintStatus(line);

	menu = GoKey(MENU_TXT, "ESCMenu");

	switch (Menu(menu, 7L, 0, TCGamePause, 0L)) {
	case 0:		/* Weiterspielen */
	    break;
	case 1:		/* Speichern  */
	    games = GoKey(GAMES_TXT, "SavedGames");

	    ShowMenuBackground();

	    GetFirstLine(THECLOU_TXT, "SaveGame", line);

	    PrintStatus(line);

	    activ = Menu(games, 15L, 0, NULL, 0L);

	    /* SaveGame (activ) */
	    RemoveList(games);
	    break;
	case 2:		/* Laden */
	    games = GoKey(GAMES_TXT, "SavedGames");

	    ShowMenuBackground();

	    GetFirstLine(THECLOU_TXT, "LoadAGame", line);

	    PrintStatus(line);

	    activ = Menu(games, 15L, 0, NULL, 0L);

	    /* LoadGame (activ)  */
	    RemoveList(games);
	    break;
	default:
	    break;
	}

	RemoveList(menu);

	UnPreparePict(20);
	UnPreparePict(16);
	UnPreparePict(17);

	answer = 0L;
	break;
    case Internal_Error:
	answer = ERROR | REQUESTER;

	PrintStatus("Gravierender Fehler !");
	Show(18, CLEAR_FIRST | NO_REFRESH | ONE_STEP | BLEND_UP | DO_NOT_FREE,
	     0L, -1L, -1L);

	break;
    case No_Mem:
	answer = ERROR | REQUESTER;

	PrintStatus("Speichermangel !! - No Memory !!");

	break;
    case Disk_Defect:
	answer = ERROR | REQUESTER;

	PrintStatus("Disk Defekt !");
	Show(18, CLEAR_FIRST | NO_REFRESH | ONE_STEP | BLEND_UP | DO_NOT_FREE,
	     0L, -1L, -1L);

	break;
    case Lib_Error:
	answer = ERROR | REQUESTER;

	PrintStatus("Lib Error");
	Show(18, CLEAR_FIRST | NO_REFRESH | ONE_STEP | BLEND_UP | DO_NOT_FREE,
	     0L, -1L, -1L);

	break;
    default:
	answer = ERROR | REQUESTER;


	PrintStatus("Unbekannter Fehler !");
	Show(18, CLEAR_FIRST | NO_REFRESH | ONE_STEP | BLEND_UP | DO_NOT_FREE,
	     0L, -1L, -1L);

	break;

    }

    ShowMenuBackground();

    TurnESC(1);

    return (answer);
}

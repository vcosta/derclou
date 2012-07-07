/*
**	$Filename: gameplay/gp.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     08-04-94
**
**
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

#include "gameplay/gp.h"
#include "gameplay/gamefunc.h"

void InitLocations(void);
void FreeLocations(void);

void InitSceneInfo(void);
void FreeSceneInfo(void);

void PrepareStory(char *filename);
void LoadSceneforStory(struct NewScene *dest, FILE * file);

void InitConditions(struct Scene *scene, struct NewScene *ns);
void FreeConditions(struct Scene *scene);
S32 GetEventCount(U32 EventNr);
S32 CheckConditions(struct Scene *scene);
void EventDidHappen(U32 EventNr);

struct Scene *GetStoryScene(struct Scene *scene);

U32 GamePlayMode = 0;
ubyte RefreshMode = 0;

struct Film *film = NULL;
struct SceneArgs SceneArgs;


void InitStory(char *story_filename)
{
    if (film)
	CloseStory();

    film = (struct Film *) TCAllocMem(sizeof(*film), 0);

    film->EnabledChoices = 0xffffffffL;
    PrepareStory(story_filename);
    InitLocations();
    LinkScenes();
    PatchStory();
}

void CloseStory(void)
{
    U32 i;

    if (film) {
	if (film->loc_names)
	    RemoveList(film->loc_names);

	for (i = 0; i < film->AmountOfScenes; i++) {
	    if (film->gameplay[i].bed)
		FreeConditions(&film->gameplay[i]);
	    if (film->gameplay[i].std_succ)
		RemoveList(film->gameplay[i].std_succ);
	}

	if (film->gameplay)
	    TCFreeMem(film->gameplay,
		      sizeof(struct Scene) * film->AmountOfScenes);

	TCFreeMem(film, sizeof(*film));

	film = NULL;
    }
}

void SetEnabledChoices(U32 ChoiceMask)
{
    film->EnabledChoices = ChoiceMask;
}

void RefreshCurrScene(void)
{
    NODE *node = GetNthNode(film->loc_names, GetLocation);

    tcRefreshLocationInTitle(GetLocation);
    PlayAnim(NODE_NAME(node), 30000,
	     GFX_NO_REFRESH | GFX_ONE_STEP | GFX_BLEND_UP);

    RefreshMenu();
}

void InitLocations(void)
{
    LIST *l = CreateList();
    char pathname[DSK_PATH_MAX];

    dskBuildPathName(DISK_CHECK_FILE, TEXT_DIRECTORY, LOCATIONS_TXT, pathname);

    if (ReadList(l, sizeof(struct TCEventNode), pathname))
	film->loc_names = (LIST *) l;
    else
	ErrorMsg(Disk_Defect, ERROR_MODULE_GAMEPLAY, 1);
}

void PatchStory(void)
{
    struct TCEventNode *node;

    if (!(GamePlayMode & GP_DEMO)) {
	GetScene(26214400L)->bed->Ort = 3;	/* 4th Burglary, Hotelzimmer */
	GetScene(26738688L)->bed->Ort = 7;	/* Arrest, Polizei!          */

	GetScene(SCENE_KASERNE_OUTSIDE)->Moeglichkeiten = 15;
	GetScene(SCENE_KASERNE_INSIDE)->Moeglichkeiten = 265;

	GetScene(SCENE_KASERNE_OUTSIDE)->LocationNr = 66;
	GetScene(SCENE_KASERNE_INSIDE)->LocationNr = 65;

	GetScene(SCENE_KASERNE_OUTSIDE)->Dauer = 17;
	GetScene(SCENE_KASERNE_INSIDE)->Dauer = 57;

	GetScene(SCENE_STATION)->Moeglichkeiten |= WAIT;

        if (setup.Profidisk) {
	    GetScene(SCENE_PROFI_26)->LocationNr = 75;
        }

	/* change possibilites in story_9 too! */

	/* fr die Kaserne hier einen Successor eingetragen! */
	GetLocScene(65)->std_succ = CreateList();
	node =
	    (struct TCEventNode *) CreateNode(GetLocScene(65)->std_succ,
					      sizeof(*node), NULL);
	node->EventNr = SCENE_KASERNE_OUTSIDE;	/* wurscht... */

	GetLocScene(66)->std_succ = CreateList();
	node =
	    (struct TCEventNode *) CreateNode(GetLocScene(66)->std_succ,
					      sizeof(*node), NULL);
	node->EventNr = SCENE_KASERNE_INSIDE;	/* wurscht... */

	film->StartScene = SCENE_STATION;
    }
}

U32 PlayStory(void)
{
    struct Scene *curr, *next = NULL;
    struct Scene *story_scene = 0;
    U8 interr_allowed = 1, first = 1;

    if (GamePlayMode & GP_STORY_OFF) {
	if (GamePlayMode & GP_DEMO)
	    curr = GetScene(SCENE_CARS_VANS);
	else {
	    tcAddPlayerMoney(5000);
	    curr = GetScene(SCENE_HOTEL_ROOM);
	}
    } else
	curr = GetScene(film->StartScene);

    SetCurrentScene(curr);
    film->alter_Ort = curr->LocationNr;
    SetLocation(-2);

    while ((curr->EventNr != SCENE_THE_END)
	   && (curr->EventNr != SCENE_NEW_GAME)) {
	if (!CheckConditions(curr))
	    ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 2);

	/* Entscheidung fr eine Szene getroffen ! */

	SceneArgs.ReturnValue = 0L;
	SceneArgs.Ueberschrieben = 0;

	/* wenn Szene Storyszene ist, dann Musik beibehalten */
	/* (Storyszenen „ndern sie selbst in Done */
	/* ansonsten wird die Musi hier ver„ndert */

	if (!story_scene)
	    tcPlaySound();

	/* ab jetzt soll nach einem Diskettenwechsel refresht werden */
	/* -> GP_STORY_TOWN Flag setzen */
	/* Achtung: dieses Flag nur nach der 1.Szene setzen -> "first" */

	if (first) {
	    film->StoryIsRunning = GP_STORY_TOWN;
	    first = 0;
	}

	/* die neue Szene initialisieren ! ( Bhnenbild aufbauen ) */
#ifdef DEEP_DEBUG
	printf("----------------------------------------\n");
	printf("SCENE_INIT %" PRIu32 "\n", curr->EventNr);
#endif
	if (curr->Init)
	    curr->Init();

	/* gibt es irgendein Ereignis das jetzt geschehen kann, und */
	/* den Standardablauf unterbricht ? */

	/* if (GamePlayMode & GP_DEMO)
	   DemoDialog(); */

	story_scene = 0;

	if (interr_allowed && (!(GamePlayMode & GP_STORY_OFF))) {
#ifdef DEEP_DEBUG
	    printf("STEP_A1\n");
#endif
	    if ((story_scene = GetStoryScene(curr)))
		interr_allowed = 0;
	} else {
#ifdef DEEP_DEBUG
	    printf("STEP_A2\n");
#endif
	    story_scene = 0;
	}

	if (!story_scene) {
	    SceneArgs.Ueberschrieben = 0;

#ifdef DEEP_DEBUG
	    printf("STEP_B\n");
#endif
	    if (curr->Done) {
		SceneArgs.Moeglichkeiten =
		    curr->Moeglichkeiten & film->EnabledChoices;

#ifdef DEEP_DEBUG
		printf("SCENE_DONE\n");
#endif
		curr->Done();	/* Funktionalit„t */
	    } else
		ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 3);

	    EventDidHappen(curr->EventNr);
	    AddVTime(curr->Dauer);

	    /* jetzt kann wieder unterbrochen werden ! */
	    interr_allowed = 1;

	    /* der Spieler ist fertig mit dieser Szene - aber wie geht es weiter ? */
	    if (SceneArgs.ReturnValue)
		next = GetScene(SceneArgs.ReturnValue);	/* Nachfolger festlegen */
	    else
		ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 4);
	} else
	    next = story_scene;	/* Unterbrechung durch die Storyszene ! */

	SetCurrentScene(curr = next);
    }				/* While Schleife */

    film->StoryIsRunning = GP_STORY_BEFORE;

    StopAnim();

    return curr->EventNr;
}

struct Scene *GetStoryScene(struct Scene *curr)
{
    U32 i, j;

    for (i = 0; i < film->AmountOfScenes; i++) {
	if (film->gameplay[i].LocationNr == (U32) - 1) {
	    struct Scene *sc = &film->gameplay[i];

	    if (sc != curr) {
		j = CalcRandomNr(0L, 255L);

		if (j <= (U32) (sc->Probability))
		    if (CheckConditions(sc))
			return (sc);
	    }
	}
    }

    return (0);
}

struct Scene *GetScene(U32 EventNr)
{
    U32 i;
    struct Scene *sc = NULL;

    for (i = 0; i < film->AmountOfScenes; i++)
	if (EventNr == film->gameplay[i].EventNr)
	    sc = &(film->gameplay[i]);

    return sc;
}

S32 GetEventCount(U32 EventNr)
{
    struct Scene *sc;

    if ((sc = GetScene(EventNr)))
	return ((S32) (sc->Geschehen));
    else
	return 0;
}

void EventDidHappen(U32 EventNr)
{
    struct Scene *sc;
    U32 max = CAN_ALWAYS_HAPPEN;

    if ((sc = GetScene(EventNr)))
	if (sc->Geschehen < max)
	    sc->Geschehen += 1;
}

S32 CheckConditions(struct Scene *scene)
{
    struct Bedingungen *bed;
    NODE *node;

    /* wenn Std Szene, dann muá nichts berprft werden ! */

    if (scene->LocationNr != (U32) - 1)
	return (1L);

    /* es handelt sich um keine Std Szene -> šberprfen ! */
    /* berprfen, ob Szene nicht schon zu oft geschehen ist ! */
    if ((scene->Anzahl != ((uword) (CAN_ALWAYS_HAPPEN))) &&
	(scene->Geschehen) >= (scene->Anzahl))
	return (0L);

    /* Jetzt die einzelnen Bedingungen berprfen */
    if (!(bed = (scene->bed)))
	return (1L);

    if (bed->Ort != (U32) -1)		/* spielt der Ort eine Rolle ? */
	if (GetLocation != (bed->Ort))
	    return (0L);	/* spielt eine Rolle und ist nicht erfllt */

    /*
     * šberprfen, ob ein Event eingetreten ist,
     * das nicht geschehen darf !
     */

    if (bed->n_events) {
	for (node = LIST_HEAD(bed->n_events); NODE_SUCC(node);
	     node = NODE_SUCC(node)) {
	    if (GetEventCount(((struct TCEventNode *) node)->EventNr))
		return (0L);
	}
    }

    /*
     * sind alle Events eingetreten, die Bedingung sind ?
     *
     */

    if (bed->events) {
	for (node = LIST_HEAD(bed->events); NODE_SUCC(node);
	     node = NODE_SUCC(node)) {
	    if (!GetEventCount(((struct TCEventNode *) node)->EventNr))
		return (0L);
	}
    }

    return (1L);
}

void PrepareStory(char *filename)
	/*
	 * completely revised 02-02-93
	 * tested : 26-03-93
	 */
{
    U32 i, j;
    struct Scene *scene;
    struct StoryHeader SH;
    struct NewScene NS;
    struct TCEventNode *node;
    FILE *file;
    char pathname[DSK_PATH_MAX];

    dskBuildPathName(DISK_CHECK_FILE, DATA_DIRECTORY, filename, pathname);

    /* StoryHeader laden ! */

    file = dskOpen(pathname, "rb");

    dskRead(file, SH.StoryName, sizeof(SH.StoryName));

    dskRead_U32LE(file, &SH.EventCount);
    dskRead_U32LE(file, &SH.SceneCount);

    dskRead_U32LE(file, &SH.AmountOfScenes);
    dskRead_U32LE(file, &SH.AmountOfEvents);

    dskRead_U32LE(file, &SH.StartZeit);
    dskRead_U32LE(file, &SH.StartOrt);
    dskRead_U32LE(file, &SH.StartSzene);

    film->AmountOfScenes = SH.AmountOfScenes;

    film->StartZeit = SH.StartZeit;

    film->akt_Minute = 543;	/* 09:03 */
    film->akt_Tag = film->StartZeit;
    film->akt_Ort = film->StartOrt = SH.StartOrt;
    film->StartScene = SH.StartSzene;

    if (film->AmountOfScenes) {
	if (!
	    (film->gameplay =
	     (TCAllocMem(sizeof(struct Scene) * (film->AmountOfScenes), 0))))
	    ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 6);
    } else
	ErrorMsg(Disk_Defect, ERROR_MODULE_GAMEPLAY, 7);

    for (i = 0; i < film->AmountOfScenes; i++) {
	LoadSceneforStory(&NS, file);

	scene = &(film->gameplay[i]);

	scene->EventNr = NS.EventNr;

	if (NS.NewOrt == (U32) -1 || NS.AnzahlderEvents || NS.AnzahlderN_Events)	/* Storyszene ? */
	    InitConditions(scene, &NS);	/* ja ! -> Bedingungen eintragen */
	else
	    film->gameplay[i].bed = NULL;	/* Spielablaufszene : keine Bedingungen ! */

	/* Scene Struktur fllen : */
	scene->Done = StdDone;
	scene->Init = StdInit;
	scene->Moeglichkeiten = NS.Moeglichkeiten;
	scene->Dauer = NS.Dauer;
	scene->Anzahl = NS.Anzahl;
	scene->Geschehen = (uword) (NS.Geschehen);
	scene->Probability = NS.Possibility;
	scene->LocationNr = NS.NewOrt;

	/* Nachfolgerliste aufbauen !  */
	/* Achtung! auch Patch „ndern! */

	if (NS.AnzahlderNachfolger) {
	    scene->std_succ = CreateList();

	    for (j = 0; j < NS.AnzahlderNachfolger; j++) {
		node =
		    (struct TCEventNode *) CreateNode(scene->std_succ,
						      sizeof(*node),
						      NULL);

		node->EventNr = NS.nachfolger[j];
	    }

	    if (NS.nachfolger)
		TCFreeMem(NS.nachfolger, sizeof(U32) * NS.AnzahlderNachfolger);
	} else
	    scene->std_succ = NULL;
    }

    /* von den Events muá nichts geladen werden ! */
    dskClose(file);
}

void InitConditions(struct Scene *scene, struct NewScene *ns)
{
    struct Bedingungen *bed;
    struct TCEventNode *node;
    U32 i;

    bed = (struct Bedingungen *) TCAllocMem(sizeof(*bed), 0);

    bed->Ort = ns->Ort;

    if (ns->AnzahlderEvents) {
	bed->events = CreateList();

	for (i = 0; i < ns->AnzahlderEvents; i++) {
	    node =
		(struct TCEventNode *) CreateNode(bed->events, sizeof(*node),
						  NULL);

	    node->EventNr = ns->events[i];
	}

	if (ns->events)
	    TCFreeMem(ns->events, sizeof(U32) * (ns->AnzahlderEvents));
    } else
	bed->events = NULL;

    if (ns->AnzahlderN_Events) {
	bed->n_events = CreateList();

	for (i = 0; i < ns->AnzahlderN_Events; i++) {
	    node =
		(struct TCEventNode *) CreateNode(bed->n_events,
						  sizeof(*node), NULL);

	    node->EventNr = ns->n_events[i];
	}

	if (ns->n_events)
	    TCFreeMem(ns->n_events, sizeof(U32) * (ns->AnzahlderN_Events));
    } else
	bed->n_events = NULL;

    scene->bed = bed;
}

void FreeConditions(struct Scene *scene)
{
    if (scene->bed) {
	if (scene->bed->events)
	    RemoveList(scene->bed->events);
	if (scene->bed->n_events)
	    RemoveList(scene->bed->n_events);

	TCFreeMem(scene->bed, sizeof(struct Bedingungen));

	scene->bed = NULL;
    }
}

void LoadSceneforStory(struct NewScene *dest, FILE * file)
{
    U32 *event_nrs = NULL;
    U32 dummy;

    dskRead_U32LE(file, &dest->EventNr);

    dskRead(file, dest->SceneName, sizeof(dest->SceneName));

    dskRead_S32LE(file, &dest->Tag);
    dskRead_S32LE(file, &dest->MinZeitPunkt);
    dskRead_S32LE(file, &dest->MaxZeitPunkt);
    dskRead_U32LE(file, &dest->Ort);

    dskRead_U32LE(file, &dest->AnzahlderEvents);
    dskRead_U32LE(file, &dest->AnzahlderN_Events);

    dskRead_U32LE(file, &dummy);
    dskRead_U32LE(file, &dummy);

    dskRead_U32LE(file, &dest->AnzahlderNachfolger);

    dskRead_U32LE(file, &dummy);

    dskRead_U32LE(file, &dest->Moeglichkeiten);
    dskRead_U32LE(file, &dest->Dauer);

    dskRead_U16LE(file, &dest->Anzahl);
    dskRead_U16LE(file, &dest->Geschehen);

    dskRead_U8(file, &dest->Possibility);

    dskRead_U32LE(file, &dest->Sample);
    dskRead_U32LE(file, &dest->Anim);
    dskRead_U32LE(file, &dest->NewOrt);

    /* allocate mem for events and read them */
    if (dest->AnzahlderEvents) {
	U32 i;

	if (!
	    (event_nrs =
	     (U32 *) TCAllocMem(sizeof(U32) * (dest->AnzahlderEvents), 0)))
	    ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 8);

	for (i = 0; i < dest->AnzahlderEvents; i++)
	    dskRead_U32LE(file, &event_nrs[i]);
    } else
	event_nrs = NULL;

    dest->events = event_nrs;

    /* allocate mem for n_events and read them */
    if (dest->AnzahlderN_Events) {
	U32 i;

	if (!
	    (event_nrs =
	     (U32 *) TCAllocMem(sizeof(U32) * (dest->AnzahlderN_Events), 0)))
	    ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 9);

	for (i = 0; i < dest->AnzahlderN_Events; i++)
	    dskRead_U32LE(file, &event_nrs[i]);
    } else
	event_nrs = NULL;

    dest->n_events = event_nrs;

    /* allocate mem for successors and read them */
    if (dest->AnzahlderNachfolger) {
	U32 i;

	if (!
	    (event_nrs =
	     (U32 *) TCAllocMem(sizeof(U32) * (dest->AnzahlderNachfolger), 0)))
	    ErrorMsg(No_Mem, ERROR_MODULE_GAMEPLAY, 10);

	for (i = 0; i < dest->AnzahlderNachfolger; i++)
	    dskRead_U32LE(file, &event_nrs[i]);
    } else
	event_nrs = NULL;

    dest->nachfolger = event_nrs;
}

void AddVTime(U32 add)
{
    U32 time;
    U32 tag;

    time = GetMinute + add;

    if (time >= MINUTES_PER_DAY) {
	tag = GetDay + time / MINUTES_PER_DAY;

	SetDay(tag);
	time = time % MINUTES_PER_DAY;
    }

    SetTime(time);
    tcTheAlmighty(time);
}
#ifdef UNUSED
static U32 GetAmountOfScenes(void)
{				/* for extern modules */
    return (film ? film->AmountOfScenes : 0);
}
#endif
void SetCurrentScene(struct Scene *scene)
{
    film->act_scene = scene;
}

struct Scene *GetCurrentScene(void)
{
    return (film ? film->act_scene : NULL);
}

struct Scene *GetLocScene(U32 locNr)
{
    U32 i;
    struct Scene *sc;

    for (i = 0; i < film->AmountOfScenes; i++)
	if (((sc = &film->gameplay[i])->LocationNr) == locNr)
	    return (sc);

    ErrorMsg(Internal_Error, ERROR_MODULE_GAMEPLAY, 12);
    return NULL;
}

void FormatDigit(U32 digit, char *s)
{
    if (digit < 10)
	sprintf(s, "0%" PRIu32, digit);
    else
	sprintf(s, "%" PRIu32, digit);
}

char *BuildTime(U32 min, char *time)
{
    U32 h = (min / 60) % 24;
    char s[TXT_KEY_LENGTH];

    min = min % 60;

    FormatDigit(h, s);
    sprintf(time, "%s:", s);
    FormatDigit(min, s);
    strcat(time, s);

    return (time);
}

char *BuildDate(U32 days, char *date)
{
    U8 days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    U32 i, p_year, year, month = 0, day;
    char s[TXT_KEY_LENGTH];

    for (i = 0, p_year = 0; i < 12; i++)
	p_year += days_per_month[i];

    year = days / p_year;	/* which daywelches Jahr ? */
    days = days % p_year;	/* wieviele Tage noch in diesem Jahr */

    for (i = 0, p_year = 0; i < 12; i++) {
	p_year += days_per_month[i];

	if (days < p_year) {
	    month = i;
	    break;
	}
    }

    day = days - (p_year - days_per_month[month]);

    FormatDigit(day + 1, s);
    strcpy(date, s);
    strcat(date, ".");
    FormatDigit(month + 1, s);
    strcat(date, s);
    strcat(date, ".");
    FormatDigit(year, s);
    strcat(date, s);

    return (date);
}

char *GetCurrLocName(void)
{
    U32 index;

    index = GetCurrentScene()->LocationNr;

    return (NODE_NAME(GetNthNode(film->loc_names, index)));
}

/*
 * SEdFunc.h
 * Functions to work with Stories/Events/Scenes for StoryEd
 * (c) 1992 by H.Gaberschek, K.Kazemi
 */

/* 
 *
 * revision history :
 *
 * 17/08/92	first structures und functions (hg)
 * 20/08/92	changed struct Scene		 (hg)
 * 31/08/92	changed struct NewScene	(Tag) (hg)
 * 11/12/92 revised (kk)
 * 12/12/92 Nodenames changed (kk)
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define SCENE_NODE              UINT8_C(100)
#define EVENT_NODE              UINT8_C(101)
#define MAX_SCENES_AMOUNT	(1<<14) /* 14 + 18 = 32 Bits !!! */
#define MAX_EVENTS_AMOUNT	(1<<18) /* wichtig da story das ben”tigt ! */


struct StoryHeader {
    ubyte StoryName[20];

    U32 EventCount;		/* Zaehler mit der h”chsten EventNr */
    U32 SceneCount;

    U32 AmountOfScenes;
    U32 AmountOfEvents;

    U32 StartZeit;
    U32 StartOrt;
    U32 StartSzene;
};

struct NewStory {
    ubyte StoryName[20];

    struct StoryHeader *sh;

    struct List *scenes;	/* Liste von SceneNodes */
    struct List *events;

    U32 StartZeit;
    U32 StartOrt;
};

struct NewScene {
    U32 EventNr;
    U8 SceneName[20];

    S32 Tag;			/* der Tag an dem sie eintritt */
    S32 MinZeitPunkt;		/* zeitlicher Bereich in dem    */
    S32 MaxZeitPunkt;		/* sie eintritt                         */
    U32 Ort;			/* der erfllt sein muá                 */

    U32 AnzahlderEvents;
    U32 AnzahlderN_Events;

    U32 *events;		/* Nr. der Events, die erfllt sein mssen */
    U32 *n_events;		/* Nr. der Events, die nicht erfllt sein mssen */

    U32 AnzahlderNachfolger;
    U32 *nachfolger;		/* Nr. der NachfolgerEvents */

    U32 Moeglichkeiten;		/* siehe defines oben                   */
    U32 Dauer;			/* Dauer dieser Szene in Minuten     */
    U16 Anzahl;			/* wie oft sie geschehen kann                */
    U16 Geschehen;		/* wie oft sie SCHON geschehen ist */
    U8 Possibility;		/* mit der sie eintritt 0-255   */

    U32 Sample;			/* Nummer des Samples */
    U32 Anim;			/* Nummer der Animation */
    U32 NewOrt;			/* Ort der Scene */
};

struct SceneNode {
    struct Node Link;
    struct NewScene ns;
};

struct NewEvent {
    U32 EventNr;
    ubyte EventName[20];
};

struct EventNode {
    struct Node Link;
    struct NewEvent ne;
};


/* Prototypes */
/* Story */
extern void NewStory(ubyte * name, U32 StartZeit, U32 StartOrt);
extern void ChangeStory(U32 StartZeit, U32 StartOrt);
extern void RemoveStory(void);
extern void WriteStory(ubyte * filename);
extern void ReadStory(ubyte * filename);

/* Scenes */
extern void AddScene(struct NewScene *ns);
extern void ChangeScene(ubyte * name, struct NewScene *ns);
extern void RemoveScene(ubyte * name);
extern void RemoveAllScenes(void);

/* Events */
extern void AddEvent(struct NewEvent *ev);
extern void RemoveEvent(ubyte * name);
extern void RemoveAllEvents(void);

/* Specials */
char *GetName(U32 EventNr);


extern struct NewStory *story;

/*
**	$Filename: gameplay/gp.h
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

#ifndef MODULE_GAMEPLAY
#define MODULE_GAMEPLAY

#include "theclou.h"

#include "base/base.h"
#include "memory/memory.h"
#include "list/list.h"
#include "disk/disk.h"
#include "gfx/gfx.h"
#include "anim/sysanim.h"
#include "story/story.h"
#include "data/dataappl.h"
#include "random/random.h"
#include "present/interac.h"


#define GO                      (1)
#define WAIT                    (1<<1)
#define BUSINESS_TALK           (1<<2)
#define LOOK                    (1<<3)
#define INVESTIGATE             (1<<4)
#define PLAN                    (1<<5)
#define CALL_TAXI               (1<<6)
#define MAKE_CALL               (1<<7)
#define INFO                    (1<<8)
#define SLEEP                   (1<<9)

#define GP_ALL_CHOICES_ENABLED  UINT32_MAX

/* Szene kann UNENDLICH oft geschehen */
#define CAN_ALWAYS_HAPPEN       UINT16_MAX

#define MINUTES_PER_DAY          1440L

#define GP_STORY_BEFORE             0
#define GP_STORY_TOWN               1
#define GP_STORY_PLAN               2

#define GP_STORY_OFF              (1)
#define GP_DEBUG_ON               (1<< 1)
#define GP_DEMO                   (1<< 2)
#define GP_LEVEL_DESIGN           (1<< 3)
#define GP_GUARD_DESIGN           (1<< 4)
#define GP_COORDINATES            (1<< 5)
#define GP_DEBUG_PLAYER           (1<< 6)
#define GP_FULL_ENV               (1<< 7)
#define GP_MUSIC_OFF              (1<< 8)
#define GP_NO_MUSIC_IN_PLANING    (1<<10)
#define GP_NO_SAMPLES             (1<<11)
#define GP_COLLISION_CHECKING_OFF (1<<12)
#define GP_SHOW_ROOMS             (1<<13)

/* Zugriffsdefines */

#define SetMinute(zeit)         (film->akt_Minute=(U32)(zeit))
#define SetLocation(loc)	{film->alter_Ort = film->akt_Ort; film->akt_Ort=(U32)(loc);}
#define SetDay(tag)             (film->akt_Tag=(U32)(tag))
#define SetTime(time)		(film->akt_Minute=(U32)(time))

#define GetDay                  (film->akt_Tag)
#define GetMinute               (film->akt_Minute)
#define GetLocation             (film->akt_Ort)
#define GetOldLocation          (film->alter_Ort)

#define GetFromDay(x)            ((x) >> 16)
#define GetToDay(x)             (((x) << 16) >> 16)

struct Film {
    U32 AmountOfScenes;

    struct Scene *act_scene;
    struct Scene *gameplay;

    LIST *loc_names;		/* Liste aller Orte im Spiel */
    /* OrtNr = Nr der Node in der */
    /* Liste */
    U32 StartScene;
    U32 StartZeit;		/* =Tag seit dem Jahr 0 */
    U32 StartOrt;

    U32 akt_Tag;
    U32 akt_Minute;
    U32 akt_Ort;
    U32 alter_Ort;

    U32 EnabledChoices;

    ubyte StoryIsRunning;
};

struct SceneArgs {
    U32 Moeglichkeiten;
    U32 ReturnValue;		/* wird AUCH (!) als Input verwendet,
				 * wenn als Output verwendet = EventNr der
				 * Nachfolgerszene
				 */
    ubyte Ueberschrieben;	/*  0...direkter Nachfahre,
				 * >0......uberschriebene Methode
				 */
};

struct Scene {
    U32 EventNr;

    void (*Init) (void);
    void (*Done) (void);

    struct Bedingungen *bed;	/* damit das Ereignis eintritt */

    LIST *std_succ;		/* Standardnachfolger TCEventNode */

    U32 Moeglichkeiten;		/* siehe defines oben                   */
    U32 Dauer;			/* Dauer dieser Szene in Sekunden       */
    uword Anzahl;		/* wie oft sie geschehen kann           */
    uword Geschehen;		/* wie oft sie SCHON geschehen ist */
    ubyte Probability;		/* mit der sie eintritt         0-255   */

    U32 LocationNr;		/* Ort, den diese Szene darstellt       */
    /* == -1 falls Szene = StorySzene       */
    /* ansonsten Nr des Ortes               */
};

struct Bedingungen {
    U32 Ort;			/* der erfÅllt sein mu· */

    LIST *events;		/* welche Events schon geschehen sein muessen */
    LIST *n_events;		/* Events, die nicht geschehen sein dÅrfen */
};

struct TCEventNode {
    NODE Node;

    U32 EventNr;
};

/* global functions */
void InitStory(char *story_filename);
extern void CloseStory(void);

extern U32 PlayStory(void);
extern void PatchStory(void);

extern void SetEnabledChoices(U32 ChoiceMask);

extern void StdDone(void);
extern void StdInit(void);

extern void RefreshCurrScene(void);
extern void SetCurrentScene(struct Scene *scene);

extern char *GetCurrLocName(void);

char *BuildDate(U32 days, char *date);
char *BuildTime(U32 min, char *time);

void FormatDigit(U32 digit, char *s);

extern struct Scene *GetCurrentScene(void);
extern struct Scene *GetLocScene(U32 locNr);
extern struct Scene *GetScene(U32 EventNr);

extern void AddVTime(U32 Zeit);

extern void LinkScenes(void);	/* Init und Done in jeder Scene Struktur setzen */

extern struct SceneArgs SceneArgs;
extern struct Film *film;
extern U32 GamePlayMode;
extern ubyte RefreshMode;

#endif

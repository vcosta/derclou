/*
 * TheClou.h
 *
 * (c) 1993 K. Kazemi, H. Gaberschek
 *
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef THECLOU_MODULE
#define THECLOU_MODULE
/*
 * verwendete Typen
 */
#include <string.h>

#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

typedef uint8_t U8;
typedef int8_t S8;
typedef uint16_t U16;
typedef int16_t S16;
typedef uint32_t U32;
typedef int32_t S32;

typedef U8 ubyte;
typedef S8 byte;
typedef U16 uword;
typedef S16 word;

#define max(a, b)               ((a) > (b) ? (a) : (b))
#define min(a, b)               ((a) < (b) ? (a) : (b))
#define clamp(x, lower, upper)  \
                                ((x) < (lower) ? \
                                    (lower) : \
                                    (x) > (upper) ? (upper) : (x))

#define array_len(array)	(sizeof(array) / sizeof((array)[0]))

#define EOS                     ((char)'\0')

#define	DIR_SEP                 "/"

/*
 * some version defines
 */

/* COMPUTER VERSION DEFINES */
/* #define THECLOU_500_VERSION */
/* define THECLOU_4000_VERSION */
/* define THECLOU_PC_VERSION */
/* define THECLOU_OS2_VERSION */
#define THECLOU_SDL_VERSION

/*  ADDITIONAL VERSION DEFINES */
/* define THECLOU_CDROM_VERSION */
#define THECLOU_PROFIDISK

/* LANGUAGE DEFINES */
#define THECLOU_VERSION_GERMAN
/* define THECLOU_VERSION_ENGLISH */
/* define THECLOU_VERSION_SLOWAKISCH */

/* PASSWORD DEFINES */
#define PASSWORD_BLOCKED
#define PASSWORD_LEN             1118
#define PASSWORD_POS            86096
#define PASSWORD                   14

/* SPECIALS DEFINES */
#define THECLOU_JOYSTICK_DISABLED
/* define THECLOU_DEBUG */

/*  PROTECTION DEFINES */
#ifdef THECLOU_OS2_VERSION
#define PASSWORD_BLOCKED
#endif

#ifndef PASSWORD_BLOCKED
#ifdef THECLOU_CDROM_VERSION
#define THECLOU_CDPROTECTION
#else
#define THECLOU_CHKSUM
#endif
#endif


#define THECLOU_NODE               50

/*
 * Textdateien, die auch als Daten verwendet werden
 *
 */

#define LOCATIONS_TXT		"location.lst"
#define COLL_LIST_TXT		"coll.lst"
#define PICT_LIST_TXT		"pict.lst"
#define GAMES_LIST_TXT		"games.lst"
#define GAMES_ORIG_TXT		"origin.lst"

#define LIV_ANIM_TEMPLATE_LIST	"template.lst"
#define LIV_LIVINGS_LIST        "livings.lst"

#define SND_SOUND_LIST          "sounds.list"
#define SND_SOUND_DIRECTORY     "sounds"

/*
 * Namen der anderen Dateien
 *
 * Diese Dateien befinden sich im Verzeichnis theclou:data
 *
 */

#define STORY_DAT               "tcstory.pc"
#define STORY_DAT_DEMO          "tcstory.dem"

/*
 * Nummern der Textdateien in denen die MenÅs stehen
 *
 * siehe Texts.list im Verzeichnis Texts
 *
 */

#define MENU_TXT		UINT32_C( 0)
#define OBJECTS_TXT		UINT32_C( 1)
#define BUSINESS_TXT 		UINT32_C( 2)
#define HOUSEDESC_TXT		UINT32_C( 3)
#define THECLOU_TXT		UINT32_C( 4)
#define INVESTIGATIONS_TXT 	UINT32_C( 5)
#define CDROM_TXT 		UINT32_C( 6)
#define OBJECTS_ENUM_TXT	UINT32_C( 7)
#define ANIM_TXT		UINT32_C( 8)
#define PRESENT_TXT		UINT32_C( 9)
#define STORY_0_TXT		UINT32_C(10)
#define PLAN_TXT		UINT32_C(11)
#define TOOLS_TXT 		UINT32_C(12)
#define LOOK_TXT		UINT32_C(13)
#define ABILITY_TXT		UINT32_C(14)
#define STORY_1_TXT		UINT32_C(15)
#define TALK_0_TXT		UINT32_C(16)
#define TALK_1_TXT              UINT32_C(17)

/*
 * Disk Nr.
 *
 */

/*
 * Defines betreffend Disketten und Verzeichnisse
 *
 */

#define PICTURE_DIRECTORY	"pictures"
#define TEXT_DIRECTORY		"texts"
#define INCLUDE_DIRECTORY	"include"
#define DATA_DIRECTORY		"data"
#define SOUND_DIRECTORY 	"sounds"
#define SAMPLES_DIRECTORY	"samples"

#define DATADISK                "datadisk"

#define BUILD_DATA_NAME 	"tcbuild"
#define MAIN_DATA_NAME		"tcmain"
#define GAME_DATA_EXT		".dat"
#define GAME_REL_EXT 		".rel"
#define TAXI_LOC_EXT 		".loc"

#define STORY_DATA_NAME 	"tcstory"

/*
 * defines for Error Module
 *
 */

#define ERR_OPEN_FUNC           fopen
#define ERR_CLOSE_FUNC		fclose
#define ERR_HANDLING_FUNC	ShowTheClouRequester

U32 ShowTheClouRequester(S32 error_class);	/* siehe Base.c */

/*
 * defines for Disk Module
 *
 */

#define DISK_ALLOC_FUNC         TCAllocMem
#define DISK_FREE_FUNC          TCFreeMem
#define DISK_ERR_FUNC           ErrorMsg

void ShowInsertDiskReq(U16 disk_id);

/*
 * some defines ...
 */

#define MATT_PICTID             UINT16_C(  7)
#define OLD_MATT_PICTID         UINT16_C(125)
#define BIG_SHEET               UINT16_C(  5)

#define BGD_LONDON              UINT16_C( 21)
#define BGD_PLANUNG             UINT16_C( 23)
#define BGD_EINBRUCH            UINT16_C( 23)
#define BGD_CLEAR               UINT16_C(  0)

#define RADIO_BUBBLE            UINT16_C( 12)
#define SPEAK_BUBBLE            UINT16_C( 12)
#define THINK_BUBBLE            UINT16_C( 13)

#define GFX_COLL_PARKING        UINT16_C( 27)

#endif

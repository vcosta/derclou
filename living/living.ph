/*
**	$Filename: living\living.ph
**	$Release:  0
**	$Revision: 0.1
**	$Date:     13-04-94
**
**	"sprite" functions for "Der Clou!"
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

#define LIV_LOAD_BUFFER			StdBuffer0	/* muá ein StdBuffer sein -> sonst ClearBuffer in .c „ndern */
#define LIV_DECR_BUFFER			StdBuffer1

#define LIV_TEMPLATE_BUFFER	StdBuffer1	/* auch LS_LOOTBAG_BUFFER „ndern!!! */

#define LIV_NO_FRAME				((uword) -1)

#define LIV_COLL_WIDTH			308	/* ein Frame ist 14 * 14 */

#define LIV_COLL_NAME			("allmaxi")

#define LIV_TEMPL_BULLE_NAME	("bullemax")


struct SpriteControl {
    LIST *p_Livings;
    LIST *p_Template;

    U32 ul_SprPlayMode;
    U32 ul_ActivAreaId;

    uword us_VisLScapeX;
    uword us_VisLScapeY;
    uword us_VisLScapeWidth;
    uword us_VisLScapeHeight;

    uword us_TotalLScapeWidth;
    uword us_TotalLScapeHeight;

    ubyte uch_FirstFrame;
    ubyte uch_LastFrame;
    ubyte uch_FrameCount;	/* Anzahl der Frames pro Anim */

    char ch_PlayDirection;
};

struct AnimTemplate {
    NODE Link;

    uword us_Width;
    uword us_Height;

    uword us_FrameOffsetNr;
};

struct Living {			/* komplette Daten einer Instanz   *//* eines Lebewesens                */
    NODE Link;

    U32 ul_LivesInAreaId;	/* Area -> LandScap */

    uword us_LivingNr;

    struct AnimTemplate *p_OriginTemplate;

    ubyte uch_XSize;
    ubyte uch_YSize;

    word s_XSpeed;
    word s_YSpeed;

    uword us_XPos;		/* absolut */
    uword us_YPos;

    ubyte uch_ViewDirection;	/* 0 .. left, right, up, down */

    ubyte uch_Action;
    ubyte uch_OldAction;

    char ch_CurrFrameNr;

    ubyte uch_Status;		/* enabled or disabled */
};

static struct SpriteControl *sc = NULL;

static struct Living *livGet(char *uch_Name);

static void livLoadTemplates(void);
static void livLoadLivings(void);
static void livRemTemplate(struct AnimTemplate *tlt);
static void livRem(struct Living *liv);
static void livHide(struct Living *liv);
static void livShow(struct Living *liv);

static void livAdd(char *uch_Name, char *uch_TemplateName, ubyte uch_XSize,
		   ubyte uch_YSize, word s_XSpeed, word s_YSpeed);

static ubyte livIsVisible(struct Living *liv);

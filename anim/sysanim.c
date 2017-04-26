/*
**	$Filename: anim/sysanim.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-02-94
**
**	anim functions for "Der Clou!"
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

#include "inphdl/inphdl.h"

#include "anim/sysanim.h"

#define	PIC_MODE_POS            UINT16_C( 1)
#define	PIC_P_SEC_POS		UINT16_C( 2)
#define	PIC_1_ID_POS		UINT16_C( 3)
#define	ANIM_COLL_ID_POS	UINT16_C( 4)
#define	PIC_COUNT_POS		UINT16_C( 5)
#define	PHASE_WIDTH_POS         UINT16_C( 6)
#define	PHASE_HEIGHT_POS	UINT16_C( 7)
#define	PHASE_OFFSET_POS	UINT16_C( 8)
#define	X_DEST_OFFSET_POS	UINT16_C( 9)
#define	Y_DEST_OFFSET_POS	UINT16_C(10)
#define	PLAY_MODE_POS		UINT16_C(11)

/* Defines fÅr Playmode */
#define	PM_NORMAL			     1
#define	PM_PING_PONG		     2
#define  PM_SYNCHRON            4

/* Defines fÅr AnimPic Aufbau */
#define 	Y_OFFSET		           0	/* 1 Pixel zwischen 2 Reihen */

struct AnimHandler {
    char *RunningAnimID;	/* Anim, die gerade lÑuft */

    U16 destX;
    U16 destY;
    U16 width;
    U16 height;

    U16 offset;
    U16 frameCount;

    U16 pictsPerRow;
    U16 totalWidth;

    U16 NrOfAnims;
    U16 PictureRate;		/* Rate mit der Anim gespielt wird */
    U16 Repeatation;		/* wie oft wiederholen */

    U16 RepeatationCount;	/* wie oft schon wiederholt */

    U16 AnimCollection;	/* einzelnen Animphasen */

    U32 WaitCounter;

    U16 CurrPictNr;
    S16 Direction;

    U8 PlayMode;

    U8 AnimatorState;
};

static struct AnimHandler Handler;
char RunningAnimLine[TXT_KEY_LENGTH];

#define ANIM_FRAME_MEM_RP	AnimRPInMem

#define ANIM_STATE_SUSPENDED	(1<<0)

void LoadAnim(char *AnimID);

/*
 * init & dones
 */

void InitAnimHandler(void)
{
    Handler.RunningAnimID = RunningAnimLine;
    Handler.RunningAnimID[0] = '\0';
}

void CloseAnimHandler(void)
{
    StopAnim();
}

void SuspendAnim(void)
{
    Handler.AnimatorState |= ANIM_STATE_SUSPENDED;
}

void ContinueAnim(void)
{
    Handler.AnimatorState &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
 * prepare...
 */

static void PrepareAnim(char *AnimID)
/* initializes various values and afterwards copies anim phases into memory */
{
    char pict_list[TXT_KEY_LENGTH];
    struct Collection *coll;

    GetAnim(AnimID, pict_list);

    if ((U32) (txtCountKey(pict_list)) > PIC_1_ID_POS) {
	coll =
	    gfxGetCollection(txtGetKeyAsULONG(ANIM_COLL_ID_POS, pict_list));

	Handler.frameCount = txtGetKeyAsULONG(PIC_COUNT_POS, pict_list);

	Handler.width =
	    (U16) txtGetKeyAsULONG(PHASE_WIDTH_POS, pict_list);
	Handler.height =
	    (U16) txtGetKeyAsULONG(PHASE_HEIGHT_POS, pict_list);

	Handler.offset =
	    (U16) txtGetKeyAsULONG(PHASE_OFFSET_POS, pict_list);

	Handler.destX =
	    (U16) txtGetKeyAsULONG(X_DEST_OFFSET_POS, pict_list);
	Handler.destY =
	    (U16) txtGetKeyAsULONG(Y_DEST_OFFSET_POS, pict_list);

	/* need to add an offset for total width! Example:
	 * 3 images with Width = 80, Offset = 2 -> TotalWidth = 244
	 * but 244 / 3 is only 2, even though there are 3 images in
         * this row!
	 */
	Handler.pictsPerRow =
	    (coll->us_TotalWidth + Handler.offset) / (Handler.width +
						      Handler.offset);
	Handler.totalWidth = coll->us_TotalWidth;

	Handler.AnimCollection = coll->us_CollId;

	/* jetzt die Animphasen vorbereiten und ins Mem kopieren */
	gfxCollToMem(coll->us_CollId, &ANIM_FRAME_MEM_RP);
    }
}

/*
 * PlayAnim
 * StopAnim
 */

void PlayAnim(char *AnimID, U16 how_often, U32 mode)
{
    char pict_list[TXT_KEY_LENGTH];
    U16 pict_id = 0, rate;

    GetAnim(AnimID, pict_list);

    if (pict_list[0] == '\0')
	gfxClearArea(l_gc);
    else {
	StopAnim();

	PrepareAnim(AnimID);

	if (!(mode & GFX_DONT_SHOW_FIRST_PIC)) {
	    if (!mode)
		mode = (U32) txtGetKeyAsULONG((U16) PIC_MODE_POS, pict_list);

	    pict_id = (U16) txtGetKeyAsULONG((U16) PIC_1_ID_POS, pict_list);
	}

	if (pict_id)
	    gfxShow(pict_id, mode, 2, -1L, -1L);

	if ((U32) (txtCountKey(pict_list)) > PIC_1_ID_POS) {
	    rate = (U16) txtGetKeyAsULONG((U16) PIC_P_SEC_POS, pict_list);

/* ZZZZZZXXXZZZZZ NOTE: UHHHHHHHHHHHHHHH? WTF???
   LOOK AT 'texts/animd.txt! WTF?'
   PLAY_MODE_POS is allegedly field 11, with possible values of
	 PlayMode (1=Normal, 2=PingPong, 4=Syncron)
	 but no stinking entry in 'texts/animd.txt' has that field!
	 dismiss as programmer error? maybe this was for amiga version and
	 they 'dropped' it for the PC version? too hard to do pingpong?
	 who knows!

			Handler.PlayMode    = (U8) txtGetKeyAsULONG((U16)PLAY_MODE_POS, pict_list);
*/
	    Handler.PlayMode = PM_NORMAL;

	    Handler.PictureRate = rate;
	    Handler.Repeatation = how_often;

	    Handler.CurrPictNr = 0;
	    Handler.Direction = 1;
	    Handler.RepeatationCount = 0;

	    Handler.WaitCounter = 1;

	    /* DoAnim ist ready to play and our anim is decrunched */
	    strcpy(Handler.RunningAnimID, AnimID);

	    ContinueAnim();	/* in case anim has been suspended */
	} else
	    Handler.RunningAnimID[0] = '\0';
    }
}

void StopAnim(void)
{
    char pict_list[TXT_KEY_LENGTH];
    struct Picture *pict;

    if (Handler.RunningAnimID) {	/* anim currently playing */
	if (Handler.RunningAnimID[0] != '\0') {
	    GetAnim(Handler.RunningAnimID, pict_list);

	    /* "unprepare" pictures for the sake of completeness */
	    pict =
		gfxGetPicture((U16)
			      txtGetKeyAsULONG((U16) PIC_1_ID_POS,
					       pict_list));

	    if (pict)
		gfxUnPrepareColl((U16) pict->us_CollId);

	    if (txtCountKey(pict_list) > PIC_1_ID_POS)
		gfxUnPrepareColl((U16)
				 txtGetKeyAsULONG((U16) ANIM_COLL_ID_POS,
						  pict_list));

	    Handler.RunningAnimID[0] = '\0';
	}
    }
}

/*
 * access function
 * GetAnim
 */

void GetAnim(char *AnimID, char *Dest)
{
    int i;
    char ID[TXT_KEY_LENGTH];

    strcpy(ID, AnimID);

    for (i = 0; ID[i] != '\0'; i++)
	if (ID[i] == ',')
	    ID[i] = '_';

    txtGetNthString(ANIM_TXT, ID, 0, Dest);
}

/*
 * Animator
 */

void animator(void)
{
    U16 destX = Handler.destX;
    U16 destY = Handler.destY;

    if (!(Handler.AnimatorState & ANIM_STATE_SUSPENDED)) {
	if (Handler.RunningAnimID && Handler.RunningAnimID[0] != '\0') {
	    if (Handler.RepeatationCount <= Handler.Repeatation) {
		if ((--Handler.WaitCounter) == 0) {
		    Handler.WaitCounter =
			Handler.PictureRate + CalcRandomNr(0, 3);

		    if (Handler.CurrPictNr == 0) {
			Handler.RepeatationCount++;

			Handler.Direction = 1;
		    }

		    if (Handler.CurrPictNr == Handler.frameCount) {
			Handler.RepeatationCount++;

			if (Handler.PlayMode & PM_NORMAL)
			    Handler.CurrPictNr = 0;
			else
			    Handler.Direction = -1;
		    }

		    if (Handler.RepeatationCount <= Handler.Repeatation) {
			U16 sourceX;
			U16 sourceY;

                        sourceX =
			    ((Handler.width +
			      Handler.offset) * Handler.CurrPictNr) %
			    (Handler.totalWidth);

                        if (Handler.pictsPerRow != 0)
                            sourceY =
			    (Handler.CurrPictNr / Handler.pictsPerRow) *
			    (Handler.height + Y_OFFSET);
                        else
                            sourceY = 0;

			/* sicherstellen, da· Animframes immer vorhanden sind */

			inpMousePtrOff();

                        gfxBlit(l_gc, &ANIM_FRAME_MEM_RP, sourceX, sourceY,
                                destX, destY, Handler.width, Handler.height,
                                false);

			inpMousePtrOn();
		    }

		    Handler.CurrPictNr = Handler.CurrPictNr + Handler.Direction;
		}
	    }
	}
    }
}

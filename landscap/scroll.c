/*
**	$Filename: landscap/scroll.c
**	$Release:  0
**	$Revision: 0.1
**	$Date:     06-04-94
**
**	landscap scroll functions for "Der Clou!"
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

#include "base/base.h"

#include "landscap/landscap.h"
#include "landscap/landscap.ph"

uword DX, DY, PX, PY;

ubyte lsScrollLandScape(ubyte direction)
{
    struct LandScape *l = ls;
    ubyte collis = 0, back = 0, speed = ls->uch_ScrollSpeed;

    if (direction != (ubyte) - 1)
	collis = lsInitScrollLandScape(direction, LS_SCROLL_PREPARE);

    if (!collis) {
	lsScrollCorrectData(DX * speed, DY * speed);

	l->us_PersonXPos += (PX * speed);
	l->us_PersonYPos += (PY * speed);

	back = (ubyte) direction;
    }

    return back;
}

ubyte lsInitScrollLandScape(ubyte direction, ubyte mode)
{
    register struct LandScape *l = ls;
    S32 dx = 0, dy = 0, px = 0, py = 0, tx = 0, ty = 0, speed =
	(S32) l->uch_ScrollSpeed;
    ubyte collis = 0;

    if (direction & LS_SCROLL_LEFT) {
	if ((l->us_WindowXPos >= speed) && (l->us_PersonXPos <= LS_CENTER_X)) {
	    dx = -1L;

	    l->us_LivingXSpeed = (word) dx;
	    l->us_LivingYSpeed = 0;
	    l->uch_LivingAction = ANM_MOVE_LEFT;
	} else {
	    if (l->us_PersonXPos > speed) {
		px = -1L;

		l->us_LivingXSpeed = (word) px;
		l->us_LivingYSpeed = 0;
		l->uch_LivingAction = ANM_MOVE_LEFT;
	    }
	}
    }

    if (direction & LS_SCROLL_RIGHT) {
	if ((l->us_WindowXPos <=
	     (LS_MAX_AREA_WIDTH - LS_VISIBLE_X_SIZE - speed))
	    && (l->us_PersonXPos >= LS_CENTER_X)) {
	    dx = 1;

	    l->us_LivingXSpeed = (word) dx;
	    l->us_LivingYSpeed = 0;
	    l->uch_LivingAction = ANM_MOVE_RIGHT;
	} else {
	    if (l->us_PersonXPos < (LS_VISIBLE_X_SIZE - speed)) {
		px = 1L;

		l->us_LivingXSpeed = (word) px;
		l->us_LivingYSpeed = 0;
		l->uch_LivingAction = ANM_MOVE_RIGHT;
	    }
	}
    }

    if (direction & LS_SCROLL_UP) {
	if ((l->us_WindowYPos >= (speed)) && (l->us_PersonYPos <= LS_CENTER_Y)) {
	    dy = -1L;

	    l->us_LivingXSpeed = 0;
	    l->us_LivingYSpeed = (word) dy;
	    l->uch_LivingAction = ANM_MOVE_UP;
	} else {
	    if (l->us_PersonYPos > (speed)) {
		py = -1L;

		l->us_LivingXSpeed = 0;
		l->us_LivingYSpeed = (word) py;
		l->uch_LivingAction = ANM_MOVE_UP;
	    }
	}
    }

    if (direction & LS_SCROLL_DOWN) {
	if (l->us_WindowYPos <= (LS_MAX_AREA_HEIGHT - LS_VISIBLE_Y_SIZE - speed)
	    && (l->us_PersonYPos >= LS_CENTER_Y)) {
	    dy = 1L;

	    l->us_LivingXSpeed = 0;
	    l->us_LivingYSpeed = (word) dy;
	    l->uch_LivingAction = ANM_MOVE_DOWN;
	} else {
	    if (l->us_PersonYPos < (LS_VISIBLE_Y_SIZE - speed)) {
		py = 1L;

		l->us_LivingXSpeed = 0;
		l->us_LivingYSpeed = (word) py;
		l->uch_LivingAction = ANM_MOVE_DOWN;
	    }
	}
    }

    tx = livGetXPos(l->uch_ActivLiving) + (px + dx) * speed;
    ty = livGetYPos(l->uch_ActivLiving) + (py + dy) * speed;

    collis = lsIsCollision(tx, ty, direction);

    if (mode & LS_SCROLL_PREPARE) {
	DX = dx;
	DY = dy;
	PX = px;
	PY = py;
    }

    return (collis);
}

ubyte lsIsCollision(S32 x, S32 y, ubyte direction)
{
    S32 i;
    ubyte collis = 0;
    ubyte color[14], collisDir = 0;

    if (GamePlayMode & GP_COLLISION_CHECKING_OFF)
	return 0;

    /* checks borders! */
    if (x <= 2)
	collis |= LS_COLLIS_LEFT;
    if (x >= (LS_MAX_AREA_WIDTH - LS_PERSON_X_SIZE - 3))
	collis |= LS_COLLIS_RIGHT;

    if (y <= 2)
	collis |= LS_COLLIS_UP;
    if (y >= (LS_MAX_AREA_HEIGHT - LS_PERSON_Y_SIZE - 3))
	collis |= LS_COLLIS_DOWN;

    if (collis)
	return collis;


    for (i = 0; i < 14; i++)
	color[i] = 255;

    /* check collision
       As the Maxi always moves by 2 pixels, pixels must be checked
       sequentially, and for each corner on a colision side */

    switch (direction) {
	/*
	   case LS_SCROLL_LEFT:
	   color[0] = gfxLSReadPixel(x - 2, y - 2);
	   color[1] = gfxLSReadPixel(x - 2, y + 0);
	   color[2] = gfxLSReadPixel(x - 2, y + 2);
	   color[3] = gfxLSReadPixel(x - 2, y + 4);
	   color[4] = gfxLSReadPixel(x - 2, y + 6);
	   color[5] = gfxLSReadPixel(x - 2, y + 8);
	   color[6] = gfxLSReadPixel(x - 2, y + 10);

	   color[7] = gfxLSReadPixel(x - 1, y - 2);
	   color[8] = gfxLSReadPixel(x - 1, y + 0);
	   color[9] = gfxLSReadPixel(x - 1, y + 2);
	   color[10] = gfxLSReadPixel( x - 1, y + 4);
	   color[11] = gfxLSReadPixel( x - 1, y + 6);
	   color[12] = gfxLSReadPixel( x - 1, y + 8);
	   color[13] = gfxLSReadPixel( x - 1, y + 10);

	   collisDir |= LS_COLLIS_LEFT;
	   break;
	   case LS_SCROLL_RIGHT:
	   color[0] = gfxLSReadPixel(x + 16, y - 2);
	   color[1] = gfxLSReadPixel(x + 16, y + 0);
	   color[2] = gfxLSReadPixel(x + 16, y + 2);
	   color[3] = gfxLSReadPixel(x + 16, y + 4);
	   color[4] = gfxLSReadPixel(x + 16, y + 6);
	   color[5] = gfxLSReadPixel(x + 16, y + 8);
	   color[6] = gfxLSReadPixel(x + 16, y + 10);

	   color[7] = gfxLSReadPixel(x + 17, y - 2);
	   color[8] = gfxLSReadPixel(x + 17, y + 0);
	   color[9] = gfxLSReadPixel(x + 17, y + 2);
	   color[10] = gfxLSReadPixel(x + 17, y + 4);
	   color[11] = gfxLSReadPixel(x + 17, y + 6);
	   color[12] = gfxLSReadPixel(x + 17, y + 8);
	   color[13] = gfxLSReadPixel(x + 17, y + 10);

	   collisDir |= LS_COLLIS_RIGHT;
	   break;
	   case LS_SCROLL_UP:
	   color[0] = gfxLSReadPixel(x + 1 , y - 3);
	   color[1] = gfxLSReadPixel(x + 2 , y - 3);
	   color[2] = gfxLSReadPixel(x + 4 , y - 3);
	   color[3] = gfxLSReadPixel(x + 6 , y - 3);
	   color[4] = gfxLSReadPixel(x + 9 , y - 3);
	   color[5] = gfxLSReadPixel(x + 12, y - 3);
	   color[6] = gfxLSReadPixel(x + 14, y - 3);

	   color[7] = gfxLSReadPixel(x + 1 , y - 4);
	   color[8] = gfxLSReadPixel(x + 2 , y - 4);
	   color[9] = gfxLSReadPixel(x + 4 , y - 4);
	   color[10] = gfxLSReadPixel(x + 6 , y - 4);
	   color[11] = gfxLSReadPixel(x + 9 , y - 4);
	   color[12] = gfxLSReadPixel(x + 12, y - 4);
	   color[13] = gfxLSReadPixel(x + 14, y - 4);
	   collisDir |= LS_COLLIS_UP;
	   break;
	   case LS_SCROLL_DOWN:
	   color[0] = gfxLSReadPixel(x + 1 , y + 13);
	   color[1] = gfxLSReadPixel(x + 2 , y + 13);
	   color[2] = gfxLSReadPixel(x + 4 , y + 13);
	   color[3] = gfxLSReadPixel(x + 6 , y + 13);
	   color[4] = gfxLSReadPixel(x + 9 , y + 13);
	   color[5] = gfxLSReadPixel(x + 12, y + 13);
	   color[6] = gfxLSReadPixel(x + 14, y + 13);

	   color[7] = gfxLSReadPixel(x + 1 , y + 14);
	   color[8] = gfxLSReadPixel(x + 2 , y + 14);
	   color[9] = gfxLSReadPixel(x + 4 , y + 14);
	   color[10] = gfxLSReadPixel(x + 6 , y + 14);
	   color[11] = gfxLSReadPixel(x + 9 , y + 14);
	   color[12] = gfxLSReadPixel(x + 12, y + 14);
	   color[13] = gfxLSReadPixel(x + 14, y + 14);

	   collisDir |= LS_COLLIS_DOWN;
	   break;
	 */

    case LS_SCROLL_LEFT:
	color[0] = gfxLSReadPixel(x - 2, y - 2);
	color[1] = gfxLSReadPixel(x - 2, y + 0);
	color[2] = gfxLSReadPixel(x - 2, y + 2);
	color[3] = gfxLSReadPixel(x - 2, y + 4);
	color[4] = gfxLSReadPixel(x - 2, y + 6);
	color[5] = gfxLSReadPixel(x - 2, y + 8);
	color[6] = gfxLSReadPixel(x - 2, y + 10);

	color[7] = gfxLSReadPixel(x - 1, y - 2);
	color[8] = gfxLSReadPixel(x - 1, y + 0);
	color[9] = gfxLSReadPixel(x - 1, y + 2);
	color[10] = gfxLSReadPixel(x - 1, y + 4);
	color[11] = gfxLSReadPixel(x - 1, y + 6);
	color[12] = gfxLSReadPixel(x - 1, y + 8);
	color[13] = gfxLSReadPixel(x - 1, y + 10);

	collisDir |= LS_COLLIS_LEFT;
	break;
    case LS_SCROLL_RIGHT:
	color[0] = gfxLSReadPixel(x + 16, y - 2);
	color[1] = gfxLSReadPixel(x + 16, y + 0);
	color[2] = gfxLSReadPixel(x + 16, y + 2);
	color[3] = gfxLSReadPixel(x + 16, y + 4);
	color[4] = gfxLSReadPixel(x + 16, y + 6);
	color[5] = gfxLSReadPixel(x + 16, y + 8);
	color[6] = gfxLSReadPixel(x + 16, y + 10);

	color[7] = gfxLSReadPixel(x + 17, y - 2);
	color[8] = gfxLSReadPixel(x + 17, y + 0);
	color[9] = gfxLSReadPixel(x + 17, y + 2);
	color[10] = gfxLSReadPixel(x + 17, y + 4);
	color[11] = gfxLSReadPixel(x + 17, y + 6);
	color[12] = gfxLSReadPixel(x + 17, y + 8);
	color[13] = gfxLSReadPixel(x + 17, y + 10);

	collisDir |= LS_COLLIS_RIGHT;
	break;
    case LS_SCROLL_UP:
	color[0] = gfxLSReadPixel(x + 3, y - 3);
	color[1] = gfxLSReadPixel(x + 3, y - 3);
	color[2] = gfxLSReadPixel(x + 4, y - 3);
	color[3] = gfxLSReadPixel(x + 6, y - 3);
	color[4] = gfxLSReadPixel(x + 9, y - 3);
	color[5] = gfxLSReadPixel(x + 11, y - 3);
	color[6] = gfxLSReadPixel(x + 11, y - 3);

	color[7] = gfxLSReadPixel(x + 3, y - 4);
	color[8] = gfxLSReadPixel(x + 3, y - 4);
	color[9] = gfxLSReadPixel(x + 4, y - 4);
	color[10] = gfxLSReadPixel( x + 6, y - 4);
	color[11] = gfxLSReadPixel( x + 9, y - 4);
	color[12] = gfxLSReadPixel( x + 11, y - 4);
	color[13] = gfxLSReadPixel( x + 11, y - 4);

	collisDir |= LS_COLLIS_UP;
	break;
    case LS_SCROLL_DOWN:
	color[0] = gfxLSReadPixel(x + 3, y + 13);
	color[1] = gfxLSReadPixel(x + 3, y + 13);
	color[2] = gfxLSReadPixel(x + 4, y + 13);
	color[3] = gfxLSReadPixel(x + 6, y + 13);
	color[4] = gfxLSReadPixel(x + 9, y + 13);
	color[5] = gfxLSReadPixel(x + 11, y + 13);
	color[6] = gfxLSReadPixel(x + 11, y + 13);

	color[7] = gfxLSReadPixel(x + 3, y + 14);
	color[8] = gfxLSReadPixel(x + 3, y + 14);
	color[9] = gfxLSReadPixel(x + 4, y + 14);
	color[10] = gfxLSReadPixel(x + 6, y + 14);
	color[11] = gfxLSReadPixel(x + 9, y + 14);
	color[12] = gfxLSReadPixel(x + 11, y + 14);
	color[13] = gfxLSReadPixel(x + 11, y + 14);

	collisDir |= LS_COLLIS_DOWN;
	break;

    default:
	break;
    }

    /* from now on color[i] contains a non-zero value when there is a collision */
    for (i = 0; i < 14; i++) {
	if ((color[i] != LS_COLLIS_COLOR_0) && (color[i] != LS_COLLIS_COLOR_1)
	    && (color[i] != LS_COLLIS_COLOR_2)
	    && (color[i] != LS_COLLIS_COLOR_3))
	    color[i] = 0;
	else
	    color[i] = 1;
    }

    for (i = 0; i < 14; i++)
	if (color[i])
	    collis = collisDir;

    /* if outer collide, but inner do not -> no collision (door!) */
    if ((color[0] || color[1]) && (color[6] || color[5]) && !color[3])
	collis = 0;

    if ((color[7] || color[8]) && (color[13] || color[12]) && !color[10])
	collis = 0;
    return collis;
}

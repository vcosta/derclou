/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "inphdl/inphdl.h"

#include "SDL.h"

struct IHandler {
    S32 ul_XSensitivity;
    S32 ul_YSensitivity;
    U32 ul_WaitTicks;

    SDLKey us_KeyCode;

    bool MouseExists;
    bool EscStatus;
    bool FunctionKeyStatus;
    bool MouseStatus;

    bool JoyExists;
    SDL_Joystick *Joystick;
};

struct IHandler IHandler;


void gfxWaitTOF(void);


static void inpDoPseudoMultiTasking(void)
{
    animator();
}

void inpOpenAllInputDevs(void)
{
    inpSetKeyRepeat((1 << 5) | 10);

    IHandler.EscStatus = true;
    IHandler.FunctionKeyStatus = true;

    IHandler.MouseExists = true;
    IHandler.MouseStatus = true;

    IHandler.JoyExists = false;

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0) {
        if (SDL_NumJoysticks() > 0) {
            IHandler.Joystick = SDL_JoystickOpen(0);

            if (IHandler.Joystick) {
                IHandler.JoyExists = true;
            } else {
                DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                         "Failed to open Joystick 0");
            }
        }
    } else {
        DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                 "SDL_InitSubSystem: %s", SDL_GetError());
    }

    inpClearKbBuffer();
}

void inpCloseAllInputDevs(void)
{
    if (SDL_JoystickOpened(0)) {
        SDL_JoystickClose(IHandler.Joystick);
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
}

void inpMousePtrOn(void)
{
/*      SDL_ShowCursor(SDL_ENABLE); */
}

void inpMousePtrOff(void)
{
/*      SDL_ShowCursor(SDL_DISABLE); */
}

S32 inpWaitFor(S32 l_Mask)
{
    SDL_Event ev;
    S32 action;
    U32 WaitTime = 0;

    if ((IHandler.EscStatus) && (!(l_Mask & INP_NO_ESC))) {
	l_Mask |= INP_ESC;
    }

    if ((IHandler.FunctionKeyStatus) && (!(l_Mask & INP_FUNCTION_KEY))) {
	l_Mask |= INP_FUNCTION_KEY;
    }

    action = 0;

    /* Nun wird auf den Event gewartet... */
    WaitTime = 0;

    while (action == 0 && SDL_PollEvent(&ev) >= 0) {

	gfxWaitTOF();

	WaitTime++;
	/* Abfrage des Zeit-Flags */
	if ((l_Mask & INP_TIME) && WaitTime >= IHandler.ul_WaitTicks) {
	    action |= INP_TIME;
        }

	switch (ev.type) {
	case SDL_KEYDOWN:
	    {
		switch (ev.key.keysym.sym) {
		case SDLK_LEFT:
		    if ((l_Mask & INP_LEFT))
			action |= INP_KEYBOARD + INP_LEFT;
		    break;

		case SDLK_RIGHT:
		    if ((l_Mask & INP_RIGHT))
			action |= INP_KEYBOARD + INP_RIGHT;
		    break;

		case SDLK_UP:
		    if ((l_Mask & INP_UP))
			action |= INP_KEYBOARD + INP_UP;
		    break;

		case SDLK_DOWN:
		    if ((l_Mask & INP_DOWN))
			action |= INP_KEYBOARD + INP_DOWN;
		    break;

		case SDLK_SPACE:
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		    if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)))
			action |= INP_KEYBOARD + INP_LBUTTONP;
		    break;

		case SDLK_ESCAPE:
		    if (IHandler.EscStatus)
			action |= INP_KEYBOARD + INP_ESC;
		    break;

		case SDLK_F1:
		case SDLK_F2:
		case SDLK_F3:
		case SDLK_F4:
		case SDLK_F5:
		case SDLK_F6:
		case SDLK_F7:
		case SDLK_F8:
		case SDLK_F9:
		case SDLK_F10:
		case SDLK_F11:
		case SDLK_F12:
		case SDLK_F13:
		case SDLK_F14:
		case SDLK_F15:
		    if (IHandler.FunctionKeyStatus)
			action |= INP_KEYBOARD + INP_FUNCTION_KEY;
		    break;

		default:
		    break;
		}
	    }
	    break;

	case SDL_MOUSEMOTION:
	    if (IHandler.MouseExists && IHandler.MouseStatus) {
		if ((l_Mask & INP_LEFT) && (ev.motion.xrel < 0))
		    action |= INP_MOUSE + INP_LEFT;
		if ((l_Mask & INP_RIGHT) && (ev.motion.xrel > 0))
		    action |= INP_MOUSE + INP_RIGHT;
		if ((l_Mask & INP_UP) && (ev.motion.yrel < 0))
		    action |= INP_MOUSE + INP_UP;
		if ((l_Mask & INP_DOWN) && (ev.motion.yrel > 0))
		    action |= INP_MOUSE + INP_DOWN;
	    }
	    break;

        case SDL_MOUSEBUTTONDOWN:
            if (IHandler.MouseExists && IHandler.MouseStatus) {
                if (ev.button.button == SDL_BUTTON_LEFT) {
                    action |= INP_MOUSE + INP_LBUTTONP;
                }
                if (ev.button.button == SDL_BUTTON_RIGHT) {
                    action |= INP_MOUSE + INP_RBUTTONP;
                }
            }
            break;

        case SDL_JOYBUTTONDOWN:
            if (IHandler.JoyExists) {
                switch (ev.jbutton.button) {
                case 0:
		    action |= INP_MOUSE + INP_LBUTTONP;
                    break;

                case 1:
		    action |= INP_MOUSE + INP_RBUTTONP;
                    break;

                default:
                    break;
                }
            }
            break;

	default:
	    break;
	}

        inpClearKbBuffer();
	inpDoPseudoMultiTasking();
    }

    return action;
}

void inpSetWaitTicks(U32 l_Ticks)
{
    IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(bool us_NewStatus)
{
    IHandler.EscStatus = us_NewStatus;
}

void inpTurnFunctionKey(bool us_NewStatus)
{
    IHandler.FunctionKeyStatus = us_NewStatus;
}

void inpTurnMouse(bool us_NewStatus)
{
    IHandler.MouseStatus = us_NewStatus;
}

void inpDelay(S32 l_Ticks)
{
    S32 i;

    for (i = 0; i < l_Ticks; i++) {
	gfxWaitTOF();
	inpDoPseudoMultiTasking();
    }
}

void inpSetKeyRepeat(unsigned char rate)
{
    int delay, interval;
    SDL_Event ev;

    delay = (rate >> 5);
    interval = (rate & 0x1f);

    if (delay == 0)
	delay = 1000;
    else
	delay = 750;

    if (interval == 0)
	interval = 30;
    else
	interval = 60;

    SDL_EnableKeyRepeat(delay, interval);

    /* flush event queue */
    while (SDL_PollEvent(&ev) > 0) {
        /* do nothing. */
    }
}

void inpClearKbBuffer(void)
{
}

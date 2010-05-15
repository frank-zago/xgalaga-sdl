/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
 * Copyright (c) 2010 Frank Zago
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "data.h"
#include "struct.h"
#include "defs.h"

static SDL_Joystick *js_device;

/*
 * Initialize the joystick device
 */
void init_joystick()
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	if (SDL_NumJoysticks()>0) {
		// Open joystick
		js_device = SDL_JoystickOpen(0);

		if (js_device) {
			printf("Opened Joystick 0\n");
			printf("Name: %s\n", SDL_JoystickName(0));
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(js_device));
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(js_device));
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(js_device));
		}
		else
			printf("Couldn't open Joystick 0\n");
	}
}

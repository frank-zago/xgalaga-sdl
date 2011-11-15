/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
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

#include "xgalaga.h"

#include <SDL/SDL_framerate.h>

static FPSmanager manager;

void init_framerate(void)
{
	SDL_initFramerate(&manager);
	SDL_setFramerate(&manager, FPS);
}

void do_framerate(void)
{
#if SDL_VERSION_ATLEAST(1,3,0)
	// todo - SDL_framerateDelay not compatible or libs mixed up.
	SDL_Delay(30);
#else
	SDL_framerateDelay(&manager);
#endif
}

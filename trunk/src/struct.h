/*
 * XGalaga-SDL - a SDL port of XGalaga, clone of the game Galaga
 * Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net)
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

#ifndef STRUCT_H
#define STRUCT_H

/* SDL 1.2 and 1.3 represent colors differently.
 * Alpha channel is always opaque. */
struct color {
#if SDL_VERSION_ATLEAST(1,3,0)
	Uint8 r;
	Uint8 g;
	Uint8 b;
#else
	Uint32 rgb;
#endif
};

struct torp {
    struct torp *next, *prev;
    int alive, frame;
    int x, y;
    int xspeed, yspeed;
};

struct star {
    int x, y;
    int speed;
	struct color pixel;
};

struct alien {
    int x, y;
    int dir;
    int steer;
    int alive;
    int dying;
    int path, path_pos;
    int escorting;
    int entering, enterdelay;
    struct W_Image *shape;
};

struct explosion {
    struct explosion *next, *prev;
    int x, y;
    int frame;
    struct W_Image *shape;
};

struct score_bubble {
    struct score_bubble *next, *prev;
    int x, y;
    int count;
    struct W_Image *shape;
};

struct js_state {
    signed short but;
    signed long dir;
};

struct FNT_fixed {
	int width, height;
	unsigned char data[];
};

enum gstate {
	INTRO,			   /* Main screen */
	PLAYING,		   /* Playing */
	PAUSED,			   /* Paused (playing) */
	GETTING_NAME,	   /* Game over, getting name for new highscore */
};

#endif

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

#include "xgalaga.h"

#define MAXPATH 50

static int paths[NUMPATHS][MAXPATH][2] = {
    {
		/* PEELLEFT = 0*/
		{ 0, 3},
		{ 15,3},
		{ 14,3},
		{ 13,3},
		{ 12,3},
		{ 11,3},
		{ 10,3},
		{ 9,3},
		{ 8,3},
		{ -1,-1},
    },
    {
		/* PEELRIGHT = 1 */
		{ 0,3},
		{ 1,3},
		{ 2,3},
		{ 3,3},
		{ 4,3},
		{ 5,3},
		{ 6,3},
		{ 7,3},
		{ 8,3},
		{ -1,-1},
    },
    {
		/* LOOP = 2 */
		{ 8,2},
		{ 9,2},
		{ 10,2},
		{ 11,2},
		{ 12,2},
		{ 13,2},
		{ 14,2},
		{ 15,2},
		{ 0,2},
		{ 1,2},
		{ 2,2},
		{ 3,2},
		{ 4,2},
		{ 5,2},
		{ 6,2},
		{ 7,2},
		{ 8,2},
		{ -1,-1},
    },
    {
		/* SWOOP1 = 3 */
		{ 8,20},
		{ 9,3},
		{ 10,3},
		{ 11,3},
		{ 12,3},
		{ -1,-1},
    },
    {
		/* SWOOP2 = 4 */
		{ 8,20},
		{ 7,3},
		{ 6,3},
		{ 5,3},
		{ 4,3},
		{ -1,-1},
    },
    {
		/* ZIGZAG = 5 */
		{ 7,15 },
		{ 8,2 },
		{ 9,15 },
		{ 8,2 },
		{ 7,15 },
		{ 8,2 },
		{ 9,15 },
		{ 8,2 },
		{ -1,-1 },
    },
    {
		/* LOOP2 = 6 */
		{ 8,2},
		{ 7,2},
		{ 6,2},
		{ 5,2},
		{ 4,2},
		{ 3,2},
		{ 2,2},
		{ 1,2},
		{ 0,2},
		{ 15,2},
		{ 14,2},
		{ 13,2},
		{ 12,2},
		{ 11,2},
		{ 10,2},
		{ 9,2},
		{ 8,2},
		{ -1,-1},
    },
    {
		/* SPIN = 7 */
		{ 8,1},
		{ 7,1},
		{ 6,1},
		{ 5,1},
		{ 4,1},
		{ 3,1},
		{ 2,1},
		{ 1,1},
		{ 0,1},
		{ 15,1},
		{ 14,1},
		{ 13,1},
		{ 12,1},
		{ 11,1},
		{ 10,1},
		{ 9,1},
		{ 8,1},
		{ -1,-1},
    },
    {
		/* LEFTDIAG */
		{ 8,2},
		{ 9,2},
		{ 10,30},
		{ 9,2},
		{ 8,1},
		{ -1,-1},
    },
    {
		/* RIGHTDIAG */
		{ 8,2},
		{ 7,2},
		{ 6,30},
		{ 7,2},
		{ 8,1},
		{ -1,-1},
    },
    {
		/* ENTER1 */
		{ 8,65},
		{ 9,5},
		{ 10,5},
		{ 11,5},
		{ 12,5},
		{ 13,5},
		{ 14,20},
		{ -1,-1},
    },
    {
		/* ENTER2 */
		{ 8,65},
		{ 7,5},
		{ 6,5},
		{ 5,5},
		{ 4,5},
		{ 3,5},
		{ 2,20},
		{ -1,-1},
    },
    {
		/* ENTER3 */
		{ -1,-1 }
	},

    {
		/* ENTER4 */
		{ 10,45},
		{ 11,5},
		{ 12,20},
		{ 13,5},
		{ 14,10},
		{ -1,-1},
	},
    {
		/* ENTER5 */
		{ 6,45},
		{ 5,5},
		{ 4,20},
		{ 3,5},
		{ 2,10},
		{ -1,-1},
	},
    {
		/* ENTER6 */
		{ 8,80},
		{ 6,10},
		{ 4,40},
		{ 0,20},
		{ 14,30},
		{ -1,-1 }
	},
    {
		/* ENTER7 */
		{ 8,80},
		{ 10,10},
		{ 12,40},
		{ 0,20},
		{ 2,30},
		{ -1,-1 }
	},
    {
		/* ENTER8 */
		{ 0,50},
		{ 2,50},
		{ 3,5},
		{ 4,5},
		{ 5,5},
		{ 6,5},
		{ 7,5},
		{ 8,5},
		{ 9,5},
		{ 10,5},
		{ 11,5},
		{ 12,30},
		{ 13,5},
		{ 14,5},
		{ 15,5},
		{ 0,5},
		{ -1,-1}
	},
    {
		/* ENTER9 */
		{ 0,50},
		{ 14,50},
		{ 13,5},
		{ 12,5},
		{ 11,5},
		{ 10,5},
		{ 9,5},
		{ 8,5},
		{ 7,5},
		{ 6,5},
		{ 5,5},
		{ 4,30},
		{ 3,5},
		{ 2,5},
		{ 1,5},
		{ 0,5},
		{ -1,-1}
	},
    {
		/* ENTER10 */
		{ 4,80},
		{ 10,30},
		{ 0,70},
		{ -1,-1}
	},
    {
		/* ENTER11 */
		{ 12,80},
		{ 6,30},
		{ 0,70},
		{ -1,-1}
	},
    {
		/* ENTER12 */
		{ 14,80},
		{ -1, -1}
	},
    {
		/* ENTER13 */
		{ 2,80},
		{ -1,-1}
	},
    {
		/* ENTER14 */
		{ 4,8},
		{ 3,6},
		{ 2,8},
		{ 1,10},
		{ 2,8},
		{ 3,6},
		{ 4,4},
		{ 5,6},
		{ 6,8},
		{ 7,10},
		{ 6,8},
		{ 5,6},
		{ 4,4},
		{ 3,6},
		{ 2,8},
		{ 1,10},
		{ 0,20},
		{ -1,-1}
	},
    {
		/* ENTER15 */
		{ 12,60},
		{ 11,3},
		{ 10,3},
		{ 9,3},
		{ 8,3},
		{ 7,3},
		{ 6,3},
		{ 5,3},
		{ 4,3},
		{ 3,3},
		{ 2,3},
		{ 1,3},
		{ 0,3},
		{ 15,3},
		{ 14,3},
		{ 13,3},
		{ 12,3},
		{ 11,3},
		{ 10,3},
		{ 9,3},
		{ 8,3},
		{ 7,3},
		{ 6,3},
		{ 5,3},
		{ 4,3},
		{ 3,3},
		{ 2,3},
		{ 1,3},
		{ 0,3},
		{ 15,3},
		{ 14,3},
		{ 13,3},
		{ 12,3},
		{ 11,3},
		{ 10,3},
		{ 9,3},
		{ 8,3},
		{ 7,3},
		{ 6,3},
		{ 5,3},
		{ 4,3},
		{ 3,3},
		{ 2,3},
		{ 1,3},
		{ 0,20},
		{ -1,-1}
	},
    {
		/* ENTER16 */
		{ 4,60},
		{ 5,3},
		{ 6,3},
		{ 7,3},
		{ 8,3},
		{ 9,3},
		{ 10,3},
		{ 11,3},
		{ 12,3},
		{ 13,3},
		{ 14,3},
		{ 15,3},
		{ 0,3},
		{ 1,3},
		{ 2,3},
		{ 3,3},
		{ 4,3},
		{ 5,3},
		{ 6,3},
		{ 7,3},
		{ 8,3},
		{ 9,3},
		{ 10,3},
		{ 11,3},
		{ 12,3},
		{ 13,3},
		{ 14,3},
		{ 15,3},
		{ 0,3},
		{ 1,3},
		{ 2,3},
		{ 3,3},
		{ 4,3},
		{ 5,3},
		{ 6,3},
		{ 7,3},
		{ 8,3},
		{ 9,3},
		{ 10,3},
		{ 11,3},
		{ 12,3},
		{ 13,3},
		{ 14,3},
		{ 15,3},
		{ 0,20},
		{ -1, -1 }
	},
    {
		/* ENTER17 */
		{ 7,15},
		{ 8,3},
		{ 9,15},
		{ 8,3},
		{ 7,15},
		{ 8,3},
		{ 9,15},
		{ 8,3},
		{ 7,15},
		{ 8,3},
		{ 9,15},
		{ 8,3},
		{ -1,-1},
    },
    {
		/* ENTER18 */
		{ 12,60},
		{ 4,30},
		{ 12,30},
		{ 4,30},
		{ 12,30},
		{ 8,20},
		{ -1,-1 }
	},
    {
		/* ENTER19 */
		{ 10,80},
		{ -1,-1},
	},
    {
		/* ENTER20 */
		{ 6,80},
		{ -1,-1},
	},
};

void path_dir(int path, int pos, int *dir, int *steer)
{
    *dir =   paths[path][pos][0];
    *steer = paths[path][pos][1];
}

void enter_path_dir(int path, int pos, int *dir, int *steer)
{
    *dir =   get_dir(path,pos);
    *steer = get_duration(path, pos);
}

void start_path(int path, struct alien *al)
{
    int i;

    for(i=0;(i<MAXPATH) && (paths[path][i][0] >= 0);i++) {
	if(paths[path][i][0] == al->dir)
	    break;
	if(paths[path][i][0] < 0) {
	    al->steer = TURNSPEED;
	    al->path = -1;
	    return;
	}
    }
    if(i >= MAXPATH) {
	al->steer = TURNSPEED;
	al->path = -1;
	return;
    }

    al->path = path;
    al->path_pos = i;
    al->steer = paths[path][i][1];
}

void new_alien(int anum, struct alien *al)
{
    if(get_shape(anum) >= 0) {
		al->alive = 1;
		al->dying = 0;
		al->entering = 1;
		get_xy(anum, &al->x, &al->y);
		al->enterdelay = get_delay(anum);
		al->enterdelay /= 1 + ((metaLevel -1) * .5);
		al->path = get_path(anum);
		al->path_pos = 0;
		al->dir = get_dir(al->path, 0);
		al->steer = get_duration(al->path, 0);
		if(metaLevel > 1)
			al->steer /= 1 + ((metaLevel - 1) * .5);
		al->escorting = -1;
		al->shape = getImage(alien_shape[get_shape(anum)]);
    } else {
		al->alive = 0;
    }
}

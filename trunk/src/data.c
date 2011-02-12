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

#include "xgalaga.h"

int buttonDown = 0;

int score;

int ships;

int level, metaLevel;
int startLevel=1;       /* default starting level defined here */

int nextBonus;

int counter;

int wantStars = 1;

int winwidth  = WINWIDTH;
int winheight = WINHEIGHT;

int alien_shape[ALIENSHAPES] = {
    I_ALIEN1,
    I_ALIEN2,
    I_ALIEN3,
    I_ALIEN4,
    I_ALIEN5,
    I_ALIEN6,
    I_ALIEN7,
    I_ALIEN8,
    I_ALIEN9,
    I_ALIEN10,
    I_ALIEN11,
    I_ALIEN12,
    I_ALIEN13,
    I_ALIEN14,
    I_ALIEN15,
    I_ALIEN16,
    I_ALIEN17,
};

int weapon = 0;
int maxtorps = 4, numtorps = 0;

int plx;
int pldead = 0;

int movespeed = MINSPEED;

int playSounds=1;

int plshield = 0;
int shieldsleft = 0;
int shieldon = 0;

unsigned int title_page=0, pagetimer=300;

int gotlemon = 0;

struct alien aliens[MAXALIENS];

SFont_Font *fnt_reg_green;
SFont_Font *fnt_reg_cyan;
SFont_Font *fnt_reg_yellow;
SFont_Font *fnt_reg_red;
SFont_Font *fnt_reg_grey;

SFont_Font *fnt_big_red;

enum gstate gstate;

int fullscreen = 1;				/* windowed = 0, fullscreen != 0 */

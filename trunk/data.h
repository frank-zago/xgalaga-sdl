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

#include "struct.h"
#include "SFont.h"

extern SDL_Surface *screen;

extern char *imagedir;
extern int verbose_image_loading;
extern int forceMono;

extern int buttonDown;
extern char *imagedirend;

extern int score;

extern int ships;
extern int shieldsleft;
extern int shieldon;

extern int level, metaLevel;
extern int startLevel;

extern int nextBonus;

extern int counter;

extern int mouseControl;
extern int wantStars;

extern int winwidth;
extern int winheight;

extern int alien_shape[];

extern int weapon;
extern int maxtorps, numtorps;

extern int plx;
extern int pldead;

extern int movespeed;

extern int playSounds;

extern int plshield;

extern unsigned int title_page, pagetimer;

extern int gotlemon;

extern struct alien aliens[];

extern SFont_Font *fnt_reg_green;
extern SFont_Font *fnt_reg_cyan;
extern SFont_Font *fnt_reg_yellow;
extern SFont_Font *fnt_reg_red;
extern SFont_Font *fnt_reg_grey;
extern SFont_Font *fnt_big_red;

extern const SDL_Color S_White;
extern const SDL_Color S_Green;
extern const SDL_Color S_Cyan;
extern const SDL_Color S_Red;
extern const SDL_Color S_Yellow;
extern const SDL_Color S_Black;
extern const SDL_Color S_Grey;

enum gstate gstate;

extern int fullscreen;

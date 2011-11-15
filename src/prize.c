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

#include <stdlib.h>

#include "xgalaga.h"

#define PR_SING 0
#define PR_DOUB 1
#define PR_TRIP 2
#define PR_SPEED 3
#define PR_SHIELD 4
#define PR_SMART 5
#define PR_LEMON 6
#define PR_EXTRABULLET 7

#ifdef ENABLE_SPREAD_SHOT
#define PR_SPREAD 8
#endif

#ifdef ENABLE_MACHINE_GUN
# ifdef ENABLE_SPREAD_SHOT
#  define PR_MACHINE 9
#  define NUMPRIZES 10
# else
#  define PR_MACHINE 8
#  define NUMPRIZES 9
# endif /* ENABLE_SPREAD_SHOT */
#else
# ifdef ENABLE_SPREAD_SHOT
#  define PR_SPREAD 8
#  define NUMPRIZES 9
# else
#  define NUMPRIZES 8
# endif /* ENABLE_SPREAD_SHOT */
#endif /* ENABLE_MACHINE_GUN */

#define PRIZESPEED 3

static struct {
	struct W_Image *image;
	const char *desc;
} prizes[NUMPRIZES];

static struct prize {
    struct prize *next, *prev;
    int x, y, type, dying;
} *first_prize; /* 8-) */

void init_prizes(void)
{
    prizes[0].image = getImage(I_PR_SING);
	prizes[0].desc = "Shot single torpedo";

    prizes[1].image = getImage(I_PR_DOUB);
	prizes[1].desc = "Shot double torpedo";

    prizes[2].image = getImage(I_PR_TRIP);
	prizes[2].desc = "Shot triple torpedo";

    prizes[3].image = getImage(I_PR_SPEED);
	prizes[3].desc = "Increase ship speed";

    prizes[4].image = getImage(I_PR_SHIELD);
	prizes[4].desc = "Shield";

    prizes[5].image = getImage(I_PR_SMART);
	prizes[5].desc = "Smart";

    prizes[6].image = getImage(I_PR_LEMON);
	prizes[6].desc = "Increase chance to get a bonus";

    prizes[7].image = getImage(I_PR_EXTRABULLET);
	prizes[7].desc = "Add a torpedo";

#ifdef ENABLE_SPREAD_SHOT
    prizes[8].image = getImage(I_PR_SPREAD);
	prizes[8].desc = "Spread Shot";
#endif

#ifdef ENABLE_MACHINE_GUN
    prizes[NUMPRIZES - 1].image = getImage(I_PR_MACHINE);
	prizes[NUMPRIZES - 1].desc = "Machine Gun";
#endif
}

void new_prize(int x, int y)
{
    struct prize *p;

    p = malloc(sizeof(struct prize));
    p->next = first_prize;
    p->prev = NULL;
    if(first_prize)
		first_prize->prev = p;
    first_prize = p;
    p->x = x;
    p->y = y;
    p->type = random()%NUMPRIZES;
    p->dying = 0;
}

void undo_prizes(void)
{
    struct prize *p = first_prize, *nextp;

    while(p) {
		nextp = p->next;

		if(p->dying) {
			if(p->next)
				p->next->prev = p->prev;
			if(p->prev)
				p->prev->next = p->next;
			if(p == first_prize)
				first_prize = p->next;
			free(p);
		}
		p = nextp;
    }
}

void do_prizes(void)
{
    struct prize *p = first_prize;
    int i,k, ne;
    int oldPlaySounds;

    while(p) {
		if (gstate != PAUSED)
			p->y+=PRIZESPEED;
		S_DrawImage(p->x-prizes[p->type].image->width/2, p->y-prizes[p->type].image->width/2,
					0, prizes[p->type].image);

		if(p->y > (winheight-20) && (ABS(p->x - plx) < 15)
#ifdef NO_PRIZE_WHILE_DEAD
		   && !pldead
#endif
			) {
			p->dying = 1;
			play_sound(SND_DDLOO);
			switch(p->type) {
			case PR_SING:
				if(weapon == SINGLESHOT)
					maxtorps++;
				else
					weapon = SINGLESHOT;
				break;
			case PR_DOUB:
				if(weapon == DOUBLESHOT)
					maxtorps++;
				else {
					weapon = DOUBLESHOT;
					if(maxtorps < 4)
						maxtorps = 4;
				}
				break;
			case PR_TRIP:
				if(weapon == TRIPLESHOT)
					maxtorps++;
				else {
					weapon = TRIPLESHOT;
					if(maxtorps < 6)
						maxtorps=6;
				}
				break;
			case PR_SPEED:
				if(movespeed < MAXSPEED)
					movespeed++;
				break;
#ifdef ENABLE_SPREAD_SHOT
			case PR_SPREAD:
                if (weapon == SPREADSHOT)
                    maxtorps++;
                else {
                    weapon = SPREADSHOT;
                    if (maxtorps < 5)
						maxtorps = 5;
				}
				break;
#endif /* ENABLE_SPREAD_SHOT */
#ifdef ENABLE_MACHINE_GUN
			case PR_MACHINE:
                if (weapon == MACHINEGUN)
					maxtorps++;
                else {
					weapon = MACHINEGUN;
					if (maxtorps < 3)
						maxtorps = 3;
                }
                break;
#endif /* ENABLE_MACHINE_GUN */
			case PR_SHIELD:
#ifdef ACTIVATED_SHIELD
				if (shieldon)
					plshield += SHIELDTIME;
				else
					shieldsleft += SHIELDTIME;
				if (shieldsleft + plshield > MAXSHIELDS)
				{
					if (shieldon)
					{
						shieldsleft = 0;
						plshield = MAXSHIELDS;
					} else {
						shieldsleft = MAXSHIELDS;
						plshield = 0;
					}
				}
#else
				plshield = SHIELDTIME;
				play_sound(SND_SHIELD);
#endif /* ACTIVATED_SHIELD */
				break;
			case PR_SMART:
				play_sound(SND_SMART);
				oldPlaySounds = playSounds;
				playSounds = 0;
				for(i=0;i<MAXALIENS;i++) {
					if(aliens[i].alive && !aliens[i].dying) {
						aliens[i].dying = 1;
						if(i >= 10) {
							if(aliens[i].dir < 0)
								score += 50;
							else {
								score += (6-(i/10))*100;
								if(!(random()%(gotlemon ? 3 : PRIZECHANCE)))
									new_prize(aliens[i].x, aliens[i].y);
							}
							new_explosion(aliens[i].x, aliens[i].y, 0);
						} else {
							if(aliens[i].dir < 0)
								score += 200;
							else {
								ne=0; /* count how many escorts */
								for(k = i+9;k < i+12; k++) {
									if(aliens[k].escorting == i)
										ne++;
								}
								score_flagship(aliens[i].x, aliens[i].y, ne);
							}
							new_explosion(aliens[i].x, aliens[i].y, 1);
						}
					}
				}
				playSounds = oldPlaySounds;
				break;
			case PR_LEMON:
				gotlemon = 1;
				maxtorps = MINTORPS;
				weapon = 0;
				movespeed = MINSPEED;
				break;
			case PR_EXTRABULLET:
				if(maxtorps < MAXTORPS)
					maxtorps++;
				break;
			}
			if(maxtorps > MAXTORPS)
				maxtorps = MAXTORPS;
		} else if(p->y > winheight) {
			p->dying = 1;
		}
		p=p->next;
    }
}

void show_bonuses(int top)
{
	int i;
	int dy = SFont_TextHeight(fnt_reg_yellow);
	const char *label = "Bonuses";
	int length;
	struct color color;

	if (dy < 22)
		dy = 22;

	SFont_WriteCenter(fnt_reg_yellow, top, label);

	/* Draw a line. */
	length = SFont_TextWidth(fnt_reg_yellow, label);
	// todo: make that a global set in sdl.c. see other use of S_DrawRect
#if SDL_VERSION_ATLEAST(1,3,0)
	color.r = 0xff;
	color.g = 0x00;
	color.b = 0x00;
#else
	color.rgb = SDL_MapRGB(screen->format, 0xff, 0, 0);
#endif

	S_DrawRect((winwidth-length)/2, top + 1 + SFont_TextHeight(fnt_reg_yellow),
			   length, 1,
			   color);

	top += dy;

    for (i=0; i<NUMPRIZES; i++) {
		S_DrawImage(100, top, 0, prizes[i].image);

		SFont_Write(fnt_reg_yellow, 130, top, prizes[i].desc);
		top += dy;
	}
}


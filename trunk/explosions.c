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

#include <config.h>

#include <stdlib.h>

#include "struct.h"
#include "images.h"
#include "data.h"
#include "proto.h"
#include "sound.h"

static struct explosion *first_exp=0;
static struct score_bubble *first_bub=0;

struct W_Image *expImage, *bubbleImages[4];

static void undo_bubbles()
{
    struct score_bubble *bub=first_bub, *nextbub;

    while(bub) {
		nextbub = bub->next;
		if(bub->count <= 0) {
			if(bub->prev)
				bub->prev->next = bub->next;
			if(bub->next)
				bub->next->prev = bub->prev;
			if(first_bub == bub)
				first_bub = 0;
			free(bub);
		}
		bub=nextbub;
    }
}

static void do_bubbles()
{
    struct score_bubble *bub=first_bub;

    while(bub) {
		if (gstate == PLAYING) {
			bub->count--;
			bub->y--;
		}
		S_DrawImage(bub->x-(bub->shape->width/2),
					bub->y-(bub->shape->height/2),
					0, bub->shape);
		bub=bub->next;
    }
}

void undo_explosions()
{
    struct explosion *exp=first_exp, *nextexp;

    while(exp) {
		nextexp = exp->next;
		if(exp->frame >= exp->shape->frames) {
			if(exp->prev)
				exp->prev->next = exp->next;
			if(exp->next)
				exp->next->prev = exp->prev;
			if(first_exp == exp)
				first_exp = 0;
			free(exp);
		}
		exp=nextexp;
    }
    undo_bubbles();
}

void do_explosions()
{
    struct explosion *exp=first_exp;

    while(exp) {
		S_DrawImage(exp->x-(exp->shape->width/2),
					exp->y-(exp->shape->height/2),
					exp->frame, exp->shape);
		if(gstate == PLAYING)
			exp->frame++;
		exp=exp->next;
    }
    do_bubbles();
}

void new_explosion(int x, int y, int type)
{
    struct explosion *exp;

    exp = malloc(sizeof(struct explosion));
    exp->next = first_exp;
    exp->prev = 0;
    if(exp->next)
		exp->next->prev = exp;
    first_exp = exp;

    exp->x = x;
    exp->y = y;
    exp->frame = 0;
    exp->shape = getImage(I_EXPLOSION);

    switch(type) {
	case 1:
		play_sound(SND_EXPLOSION);
		break;
	case 2:
		play_sound(SND_EXP_SB);
		break;
	case 0:
	default:
		play_sound(SND_TORPHIT);
		break;
    }
}

void score_flagship(int x, int y, int ne)
{
    struct score_bubble *bub;

    bub = malloc(sizeof(struct score_bubble));
    bub->next = first_bub;
    bub->prev = 0;
    if(bub->next)
		bub->next->prev = bub;
    first_bub = bub;

    bub->x = x;
    bub->y = y;
    bub->count=25;
    bub->shape = bubbleImages[ne];
    switch(ne) {
	case 0:
		score+=500;
		break;
	case 1:
		score+=1000;
		break;
	case 2:
		score+=2000;
		break;
	case 3:
		score+=4000;
		break;
    }
}

void init_explosions()
{
    expImage = getImage(I_EXPLOSION);

    bubbleImages[0] = getImage(I_S500);
    bubbleImages[1] = getImage(I_S1000);
    bubbleImages[2] = getImage(I_S2000);
    bubbleImages[3] = getImage(I_S4000);
}

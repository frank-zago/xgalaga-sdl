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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "xgalaga.h"

static struct W_Image *miniship, *extraImage;
static int drawExtra = 0, extrax, extray;

void do_score()
{
    static int lastscore;

    if (lastscore != score)
    {
		if ((score > 0) && (score >= nextBonus))
        {
			ships++;
			extrax = 0 - extraImage->width/2;
			extray = winheight/2;
			drawExtra = 1;
			if (nextBonus < BONUSSHIPSCORE)
				nextBonus = BONUSSHIPSCORE;
			else
				nextBonus += BONUSSHIPSCORE;
		}
    }

	draw_score();
	lastscore=score;

    if (drawExtra)
    {
		extrax += 4;
		S_DrawImage(extrax-(extraImage->width/2), extray-(extraImage->height/2), 0, extraImage);
		if((extrax-(int)extraImage->width/2) > winwidth)
			drawExtra = 0;
    }
}

void draw_score()
{
    int i;
	char str[20];

    sprintf(str, "%07d", score);
    SFont_WriteCenter(fnt_reg_cyan, 0, str);

    sprintf(str, "Level %02d", level);
    SFont_WriteRight(fnt_reg_yellow, 0, str);

	if (ships < 18) {
		for(i=0; i<ships; i++)
			S_DrawImage(i*(miniship->width+2), 0, 0, miniship);
	} else {
		S_DrawImage(0, 0, 0, miniship);
		sprintf(str, "(%d)", ships);
		SFont_Write(fnt_reg_yellow, miniship->width + 2, 0, str);
    }
}

void init_score()
{
    miniship = getImage(I_MINISHIP);
    extraImage = getImage(I_EXTRA);
}


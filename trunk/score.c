/* $Id: score.c,v 1.2 1998/04/30 05:11:58 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>
#include <stdio.h>
#include <string.h>

#include "struct.h"
#include "images.h"
#include "data.h"
#include "defs.h"
#include "proto.h"

struct W_Image *miniship, *extraImage;
int drawExtra = 0, extrax, extray;

void do_score()
{
    static int lastscore;
	
    if (lastscore != score) 
    {
		if ((score > 0) && (score >= nextBonus)) 
        {
			ships++;
			extrax = 0 - extraImage->width/2;
			extray = WINHEIGHT/2;
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
		if((extrax-(int)extraImage->width/2) > WINWIDTH)
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


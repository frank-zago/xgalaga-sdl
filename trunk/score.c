/* $Id: score.c,v 1.2 1998/04/30 05:11:58 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>
#include <stdio.h>
#include <string.h>
#include "struct.h"
#include "Wlib.h"
#include "images.h"
#include "data.h"
#include "defs.h"
#include "proto.h"

char scorestr[40] = "Score: 0000000", shipstr[4] ="";
W_Image *miniship, *extraImage;
int drawExtra = 0, extrax, extray;

void undo_score()
{
    if(drawExtra)
	W_ClearArea(baseWin, extrax-(extraImage->width/2), extray-(extraImage->height/2), 
		    extraImage->width, extraImage->height);
}

void do_score()
{
    static int lastscore, lastlevel, lastships;
#ifdef SOUND
    static int lastsounds;
#endif
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

#ifdef SOUND
    if (lastscore != score || lastlevel != level || lastships != ships || lastsounds != playSounds) 
#else
    if (lastscore != score || lastlevel != level || lastships != ships) 
#endif
    {
    	W_ClearArea(shellWin, 0, 0, WINWIDTH, W_Textheight + 1);
	draw_score();
	lastscore=score;
	lastlevel=level;
	lastships=ships;
#ifdef SOUND
        lastsounds=playSounds;
#endif
    }

    if (drawExtra) 
    {
	extrax += 10;
	W_DrawImage(baseWin, extrax-(extraImage->width/2), extray-(extraImage->height/2), 0, extraImage, W_White);
	if((extrax-(int)extraImage->width/2) > WINWIDTH)
	    drawExtra = 0;
    }
}

void draw_score()
{
    int basex;
    int i;

#ifdef SOUND
    sprintf(scorestr, "Score: %07d     Level: %02d  %c", score, level, playSounds ? ' ':'Q');
#else
    sprintf(scorestr, "Sore: %07d     Level: %02d", score, level);
#endif

    basex = WINWIDTH/2 - ((strlen(scorestr)/2)*W_Textwidth);
    W_MaskText(shellWin, basex, 1, W_Grey, scorestr, strlen(scorestr), W_RegularFont);
    W_MaskText(shellWin, basex+1, 0, W_Yellow, scorestr, strlen(scorestr), W_RegularFont);

    for(i=0;i<((ships < 6) ? ships : 6);i++) {
	W_DrawImage(shellWin, i*(miniship->width+2), 0, 0, miniship, W_White);
    }

    if(ships>6) {
	sprintf(shipstr, "%d", ships);
	basex = 6*(miniship->width+2) + 2;
	W_MaskText(shellWin, basex, 1, W_Grey, shipstr, strlen(shipstr), W_RegularFont);
	W_MaskText(shellWin, basex+1, 0, W_Yellow, shipstr, strlen(shipstr), W_RegularFont);
    }
}

void init_score()
{
    miniship = getImage(I_MINISHIP);
    extraImage = getImage(I_EXTRA);
}


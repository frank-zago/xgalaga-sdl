/* $Id: explosions.c,v 1.2 1998/04/30 05:11:54 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>


#include <stdlib.h>
#include "Wlib.h"
#include "images.h"
#include "struct.h"
#include "data.h"
#include "proto.h"
#include "sound.h"

static struct explosion *first_exp=0;
static struct score_bubble *first_bub=0;

W_Image *expImage, *bubbleImages[4];

static void undo_bubbles()
{
    struct score_bubble *bub=first_bub, *nextbub;

    while(bub) {
	W_CacheClearArea(baseWin, bub->x-(bub->shape->width/2), bub->y-(bub->shape->height/2), bub->shape->width, bub->shape->height);
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
	if(!paused) {
	    bub->count--;
	    bub->y--;
	}
	W_DrawImage(baseWin, 
		    bub->x-(bub->shape->width/2), 
		    bub->y-(bub->shape->height/2), 
		    0, bub->shape, W_Red);
	bub=bub->next;
    }
}

void undo_explosions()
{
    struct explosion *exp=first_exp, *nextexp;

    while(exp) {
	W_CacheClearArea(baseWin, exp->x-(exp->shape->width/2), exp->y-(exp->shape->height/2), exp->shape->width, exp->shape->height);
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
	W_DrawImage(baseWin, 
		    exp->x-(exp->shape->width/2), 
		    exp->y-(exp->shape->height/2), 
		    exp->frame, exp->shape, W_Red);
	if(!paused) 
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
#ifdef SOUND
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
#endif
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

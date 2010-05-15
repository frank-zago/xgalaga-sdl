/* $Id: prize.c,v 1.2 1998/04/30 05:11:58 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"
#include <config.h>

#include <stdlib.h>
#include "Wlib.h"
#include "images.h"
#include "struct.h"
#include "data.h"
#include "defs.h"
#include "proto.h"
#include "sound.h"

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

W_Image *prizeImages[NUMPRIZES];
    
struct prize {
    struct prize *next, *prev;
    int x, y, type, dying;
} *first_prize; /* 8-) */

void init_prizes()
{
    prizeImages[0] = getImage(I_PR_SING);
    prizeImages[1] = getImage(I_PR_DOUB);
    prizeImages[2] = getImage(I_PR_TRIP);
    prizeImages[3] = getImage(I_PR_SPEED);
    prizeImages[4] = getImage(I_PR_SHIELD);
    prizeImages[5] = getImage(I_PR_BRAIN);
    prizeImages[6] = getImage(I_PR_LEMON);
    prizeImages[7] = getImage(I_PR_EXTRABULLET);
#ifdef ENABLE_SPREAD_SHOT
    prizeImages[8] = getImage(I_PR_SPREAD);
#endif

#ifdef ENABLE_MACHINE_GUN
    prizeImages[NUMPRIZES - 1] = getImage(I_PR_MACHINE);
#endif 
}
    
void new_prize(int x, int y)
{
    struct prize *p;

    p = malloc(sizeof(struct prize));
    p->next = first_prize;
    p->prev = 0;
    if(first_prize)
	first_prize->prev = p;
    first_prize = p;
    p->x = x;
    p->y = y;
    p->type = random()%NUMPRIZES;
    p->dying = 0;
}

void undo_prizes()
{
    struct prize *p = first_prize, *nextp;

    while(p) {
	nextp = p->next;

	W_CacheClearArea(baseWin, 
			 p->x-prizeImages[p->type]->width/2, p->y-prizeImages[p->type]->width/2,
			 prizeImages[p->type]->width+1, prizeImages[p->type]->height);
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

void do_prizes()
{
    struct prize *p = first_prize;
    int i,k, ne;
#ifdef SOUND
    int oldPlaySounds;
#endif

    while(p) {
	if(!paused)
	    p->y+=PRIZESPEED;
	W_DrawImage(baseWin, 
		    p->x-prizeImages[p->type]->width/2, p->y-prizeImages[p->type]->width/2,
		    0, prizeImages[p->type], W_Green);

	if(p->y > (WINHEIGHT-20) && (ABS(p->x - plx) < 15)
#ifdef NO_PRIZE_WHILE_DEAD
		&& !pldead
#endif
	  ) {
	    p->dying = 1;
#ifdef SOUND
	    play_sound(SND_DDLOO);
#endif
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
#ifdef SOUND
		play_sound(SND_SHIELD);
#endif /* SOUND */
#endif /* ACTIVATED_SHIELD */
		break;
	      case PR_SMART:
#ifdef SOUND
		play_sound(SND_SMART);
		oldPlaySounds = playSounds;
		playSounds = 0;
#endif
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
#ifdef SOUND
		playSounds = oldPlaySounds;
#endif
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
	} else if(p->y > WINHEIGHT) {
	    p->dying = 1;
	}
	p=p->next;
    }
}


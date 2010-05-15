/* Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"
/*
 * main.c for XGalaga
 */
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include "libsprite/Wlib.h"
#include "struct.h"
#include "data.h"
#include "images.h"
#include "defs.h"
#include "paths.h"
#include "proto.h"
#include "sound.h"

struct torp torps[MAXTORPS];
struct star stars[NUMSTARS];
struct torp *first_etorp=0;

static int convoyx = 0, convoymove = 1;
static int livecount = 0;
static int starspeed = 1;
static int attacking = 0, maxattacking, entering=0;
static int maxetorps = 5, numetorps=0;
static int plflash = 50;
static int fullscreen = 0;
#ifndef ORIGINAL_XGALAGA
static int shots = 0;
static int hits = 0;
#endif

#define convoy_x_pos(i) (convoyx+(20 * (i - 10 * (i/10))))
#define convoy_y_pos(i) (20 + (20*(i/10)))

static int moves[16][2] = {
    {0,-4},
    {1,-4},
    {3,-3},
    {4,-1},
    {4,0},
    {4,1},
    {3,3},
    {1,4},
    {0,4},
    {-1,4},
    {-3,3},
    {-4,1},
    {-4,0},
    {-4,-1},
    {-3,-3},
    {-1,-4}
};

void
xgal_exit(int v)
{
    /*    W_AutoRepeatOn();*/
    /* Destroy our main window so the fullscreen mode gets unset if we're
       running fullscreen. (We should really clean up much more here!) */
    W_DestroyWindow(shellWin);
#ifdef SOUND
    kill_sound();
#endif
    /*    sleep(1);*/ /* Without this, the auto-repeat request fails on my machine... */
    /* Note if we ever need this autorepeat thingie again, the sleep can and should be replaced by a call to XSync() */
    exit(v);
}


static void print_usage()
{
#ifdef SOUND 
#define PRINT_SOUND	"-nosound             Turn sound OFF\n"
#else
#define PRINT_SOUND	""
#endif

    printf("\
XGalaga v%s\n\
Copyright (c) 1995-1998 Joe Rumsey\n\
Contributions by Hermann Riedel\n\
Command line options:\n\
-scores              Prints out the high score files and exits\n\
-display <display>   Set your display\n\
-mouse               Use mouse control (same as 'm' at the title screen)\n\
-keyboard            Use keyboard control (same as 'k')\n%s\
-level <number>      Choose starting level (>= 1)\n\
-window              Start in windowed mode instead of fullscreen\n\
-winsize <WxH>       Window size (default 468 x 596)\n\
-b                   turn buffered mode off, use this if it runs\n\
too slowly.  Will cause flicker, maybe lots,\n\
maybe only a little.\n\
\n\
This game is now free software, under the GPL\n\
\n\
Basic instructions:\n\
It's Galaga, you know how to play Galaga, stop bothering me.\n\
(Ship follows the mouse, button fires.  Auto-fire by holding it\n\
down, so no-one accuses me of breaking their mouse!)\n\
\n\
Keyboard commands:\n\
\n\
p - pauses\n\
q - end this game\n\
b - Toggle buffering (flicker vs. speed.)\n\
alt + enter - Toggle fullscreen - window\n", VERSION,
	PRINT_SOUND
           );
}

/*------------------stars-----------------*/
static void init_stars()
{
    int i;

    for(i=0;i<NUMSTARS;i++) {
        stars[i].x = random()%WINWIDTH;
        stars[i].y = random()%WINHEIGHT;
        stars[i].speed = (random()%3)+1;
        switch(random()%5) {
        case 0:
            stars[i].color = W_White;
            break;
        case 1:
            stars[i].color = W_Green;
            break;
        case 2:
            stars[i].color = W_Cyan;
            break;
        case 3:
            stars[i].color = W_Red;
            break;
        default:
            stars[i].color = W_Yellow;
            break;
        }
    }
}

static int drewlevel = 0;

static void undo_stars()
{
    int i;
    
    if(wantStars) {
        for(i=0;i<NUMSTARS;i++) {
            W_DrawPoint(baseWin, stars[i].x, stars[i].y, W_Black);
        }
    }
    if(drewlevel) {
        W_ClearArea(baseWin, WINWIDTH/2 - (W_StringWidth("LEVEL 000", W_BigFont)/2), WINHEIGHT/2-W_BigTextheight/2,
                    10*W_BigTextwidth, W_BigTextheight);
        drewlevel = 0;
    }
}

static void do_stars()
{
    int i;
    
    if(wantStars) {
        for(i=0;i<NUMSTARS;i++) {
            if(!paused)
                stars[i].y+=stars[i].speed*((starspeed < 20) ? ABS(starspeed) : 20);
            if(stars[i].y >= WINHEIGHT) {
                stars[i].y-=WINHEIGHT+starspeed;
                stars[i].x = random() % WINWIDTH;
                switch(random()%5) {
                case 0:
                    stars[i].color = W_White;
                    break;
                case 1:
                    stars[i].color = W_Green;
                    break;
                case 2:
                    stars[i].color = W_Cyan;
                    break;
                case 3:
                    stars[i].color = W_Red;
                    break;
                default:
                    stars[i].color = W_Yellow;
                    break;
                }
            }
            W_DrawPoint(baseWin, stars[i].x, stars[i].y, stars[i].color);
        }
#ifdef SHOW_SHIELD_BAR
	    if ((plshield > 0) || (shieldsleft > 0)) { 
		int shieldcount = 0;
		int total_shields = (plshield + shieldsleft) * 19 / SHIELDTIME + 1;
		while (total_shields > 0) {
			shieldcount++;
			total_shields -= 19;
			W_DrawImage(baseWin, WINWIDTH - 20 * shieldcount, 0, 0, shieldImage, W_Cyan);
		}
		while (total_shields < 0) {
			int column;
			column = WINWIDTH - 20 * shieldcount - total_shields++;
			W_MakeLine(baseWin, column, 0, column, 20, W_Black);
			W_MakeLine(baseWin, column - 1, 0, column - 1, 20, W_Black);
		}
       	    }
#endif /* SHOW_SHIELD_BAR */


    }
    if(starspeed != 1) {
        char buf[20];
#ifndef ORIGINAL_XGALAGA
	int y;
#endif
    
        drewlevel = 1;
        sprintf(buf, "LEVEL %d", level+1);
#ifndef ORIGINAL_XGALAGA
	y = WINHEIGHT/2-W_BigTextheight/2;
#endif
        W_MaskText(baseWin, WINWIDTH/2 - (W_StringWidth(buf, W_BigFont)/2), WINHEIGHT/2-W_BigTextheight/2, W_Red, buf, strlen(buf), W_BigFont);
#ifndef ORIGINAL_XGALAGA
	if (shots > 0) {
		int x = WINWIDTH/2 - 14 * W_Textwidth;
		y += W_BigTextheight + 20;
	
		sprintf(buf, "Torps: %d  Hits: %d", shots, hits);
		W_MaskText(baseWin, x, y, W_Yellow, buf, strlen(buf), W_RegularFont);
	
		x += 23 * W_Textwidth;
		sprintf(buf, "(%d%%)", 100 * hits / shots);
		W_MaskText(baseWin, x, y, W_Green, buf, strlen(buf), W_RegularFont);
	}
#endif
    }
}

/*-------------------aliens---------------*/

static void delete_etorps()
{
    struct torp *tmp;

    while(first_etorp) {
        tmp = first_etorp->next;
        free(first_etorp);
        first_etorp = tmp;
    }
}

static void init_aliens(int level)
{
    int i;

    convoyx=0;
    convoymove = 1;

    maxattacking = 1 + (level*2);
    if(maxattacking > 30)
        maxattacking = 30;
    attacking = 0;
    maxetorps = 10 + (level*5);
    numetorps = 0;

    delete_etorps();
    metaLevel = 1;
    if(read_level(level) <= 0)
    {
	fprintf(stderr, "Error reading level %d\n", level);
	exit(0);
    }

    for(i=0;i<MAXALIENS;i++) {

        livecount++;
        new_alien(level, i, &aliens[i]);
    }

    for(i=0;i<MAXTORPS;i++)
        torps[i].alive = 0;
}

    
static void undo_aliens()
{
    int i;

    for(i=0;i<MAXALIENS;i++) {
        if(aliens[i].alive)
            W_ClearArea(baseWin, 
                        aliens[i].x-(aliens[i].shape->width/2), 
                        aliens[i].y-(aliens[i].shape->height/2),
                        aliens[i].shape->width, aliens[i].shape->height);
        if(aliens[i].dying) {
            aliens[i].alive = 0;
            aliens[i].dying=0;
        }
    }
}

static void
do_escort(int i)
{
    int fs = aliens[i].escorting;
    
    if(!aliens[fs].alive) {
        aliens[i].escorting = -1;
    } else if(aliens[fs].dir >= 0) {
        aliens[i].dir = aliens[fs].dir;
    } else {
        aliens[i].x = 20 * (i - 10 * (i/10)) + convoyx + convoymove;
        aliens[i].y = -10;
        aliens[i].dir = -2;
        aliens[i].path = -1;
        aliens[i].steer = 2;
        aliens[i].escorting = -1;
    }
}

static void
do_convoy(int i)
{
    aliens[i].x += convoymove;
    if((entering == 0) &&
       (attacking < maxattacking) && 
       ((livecount < maxattacking) || 
        ((random()%10000) < (level + 2 *(48-(livecount)))))) {
        switch(random()%2) {
        case 0:
            path_dir(P_PEELLEFT, 0, &aliens[i].dir, &aliens[i].steer);
            aliens[i].path = P_PEELLEFT;
            break;
        case 1:
            path_dir(P_PEELRIGHT, 0, &aliens[i].dir, &aliens[i].steer);
            aliens[i].path = P_PEELRIGHT;
            break;
        }
        aliens[i].path_pos = 0;
        attacking++;
        if(i<10) { /* Flagship, grab escorts */
            int e;
            for(e=i+9;e<i+12;e++) {
                if(aliens[e].alive && aliens[e].dir == -1) {
                    aliens[e].escorting = i;
                }
            }
        }
    }
}

static void new_etorp(int x, int y, int xs, int ys)
{
    struct torp *t;

    t = malloc(sizeof(struct torp));
    t->next = first_etorp;
    if(t->next)
        t->next->prev = t;
    t->prev = 0;
    first_etorp = t;

    t->x = x;
    t->y = y;
    t->xspeed = xs;
    t->yspeed = ys;
    t->alive = 1;
    t->frame = 0;
    numetorps++;
}

static void do_enter(int i)
{
    int diffx, diffy;
    int tc;

    if(aliens[i].enterdelay) {
        aliens[i].enterdelay--;
        return;
    }

    if(aliens[i].path >= 0) {
        aliens[i].x += moves[aliens[i].dir][0] + metaLevel * moves[aliens[i].dir][0]/2;
        aliens[i].y += moves[aliens[i].dir][1] + metaLevel * moves[aliens[i].dir][1]/2;
    
        aliens[i].steer--;
        if(aliens[i].steer <= 0) {
            aliens[i].path_pos++;
            enter_path_dir(aliens[i].path, aliens[i].path_pos, &aliens[i].dir, &aliens[i].steer);
	    if(metaLevel > 1)
		aliens[i].steer = aliens[i].steer / (1 + ((metaLevel - 1) * .5));
	    /*aliens[i].steer -= ((metaLevel - 1) * (aliens[i].steer / 3));*/

            if(aliens[i].dir < 0) {
                aliens[i].path = -1;
            }
        }
        tc = TORPCHANCE - level/2 - weapon*5;
        if(tc < 35) tc = 35;
        if(numetorps < maxetorps && (!(random()%tc))) {
            int xs, ys;
        
            /* could aim better, not sure it should! */
        
            if(aliens[i].x > plx + 200) {
                xs = -3;
            } else if(aliens[i].x > plx + 100) {
                xs = -2;
            } else if(aliens[i].x < plx - 200) {
                xs = 3;
            } else if(aliens[i].x < plx - 100) {
                xs = 2;
            } else {
                xs = 0;
            }
            ys = (ETORPSPEED+level/5) - ABS(xs);
            new_etorp(aliens[i].x, aliens[i].y, xs, ys);
        }
    } else {
        diffx = ABS(convoy_x_pos(i) - aliens[i].x);
        diffy = ABS(convoy_y_pos(i) - aliens[i].y);
        if(diffy< 4 + (metaLevel * 2)) {
            aliens[i].y = convoy_y_pos(i);
            if(diffx < 4 + (metaLevel * 2)) {
                aliens[i].x = convoy_x_pos(i);
                aliens[i].dir = -1;
                aliens[i].entering = 0;
                return;
            }
            aliens[i].dir = 0;
            if(convoy_x_pos(i) > aliens[i].x)
                aliens[i].dir = 4;
            else
                aliens[i].dir = 12;
        } else {
	    if(convoy_y_pos(i) < aliens[i].y) {
		if(diffx < 4 + (metaLevel * 2)) {
		    aliens[i].x = convoy_x_pos(i);
		    aliens[i].dir = 0;
		} else {
		    if(convoy_x_pos(i) > aliens[i].x)
			aliens[i].dir = 2;
		    else
			aliens[i].dir = 14;
		}
	    } else {
		if(diffx < 4 + (metaLevel * 2)) {
		    aliens[i].x = convoy_x_pos(i);
		    aliens[i].dir = 8;
		} else {
		    if(convoy_x_pos(i) > aliens[i].x)
			aliens[i].dir = 6;
		    else
			aliens[i].dir = 10;
		}
	    }
        }
        aliens[i].x += moves[aliens[i].dir][0] + metaLevel * moves[aliens[i].dir][0]/2;
        aliens[i].y += moves[aliens[i].dir][1] + metaLevel * moves[aliens[i].dir][1]/2;
    }
}

static void do_aliens()
{
    int i, j;
    int tc;

    if(!paused) {
        convoyx += convoymove;
        if(convoyx <= 0) {
            convoyx=0;
            convoymove = -convoymove;
        } else if(convoyx >= (WINWIDTH-180)) {
            convoyx = WINWIDTH - 180;
            convoymove = -convoymove;
        }
    }
    
    livecount=0; attacking = 0;
    for(i=0, livecount=0, entering=0; i < MAXALIENS; i++) {
        if(aliens[i].alive) {
            livecount++;
            if(aliens[i].dir >= 0 && aliens[i].escorting < 0 && !(aliens[i].entering))
                attacking++;
            if(aliens[i].entering)
                entering++;
        }
    }

    for(i=0; i < MAXALIENS; i++) {
        if(aliens[i].alive) {
            if(!paused) {
                if(aliens[i].escorting >= 0) {
                    do_escort(i);
                }

                if(aliens[i].entering) {
                    do_enter(i);
                } else if(aliens[i].dir == -1) {
                    do_convoy(i);
                } else if(aliens[i].dir == -2) {
                    aliens[i].x += convoymove;
                    aliens[i].y+=2;
                    if(aliens[i].y >= 20 + (20*(i/10))) {
                        aliens[i].y = 20 + (20*(i/10));
                        aliens[i].dir = -1;
                    }
                } else {
                    aliens[i].x += moves[aliens[i].dir][0];
                    aliens[i].y += moves[aliens[i].dir][1];
                    if(aliens[i].x > WINWIDTH+20)
                        aliens[i].x = -20;
                    else if(aliens[i].x < -20)
                        aliens[i].x = WINWIDTH+20;
            
                    if(aliens[i].y > WINHEIGHT) {
                        aliens[i].x = 20 * (i - 10 * (i/10)) + convoyx + convoymove;
                        aliens[i].y = -30;
                        aliens[i].dir = -2;
                        aliens[i].path = -1;
                        aliens[i].steer = 2;
                        aliens[i].escorting = -1;
                        attacking--;
                        if(i < 10) {
                            for(j=i+9;j<i+12;j++)
                                aliens[j].escorting = -1;
                        }
                    } else if(aliens[i].y < 0) {
                        aliens[i].dir = 8;
                    }
            
                    if(aliens[i].escorting < 0) {
                        aliens[i].steer--;
                        if(aliens[i].steer <= 0) {
                            if(aliens[i].path >= 0) {
                                int lastdir=aliens[i].dir;
                
                                aliens[i].path_pos++;
                                path_dir(aliens[i].path, aliens[i].path_pos, &aliens[i].dir, &aliens[i].steer);
                                if(aliens[i].dir < 0) {
                                    aliens[i].dir = lastdir;
                                newpath:
                                    switch(random()%8) {
                                    case 0:
                                        start_path(P_LOOP, &aliens[i]);
                                        break;
                                    case 1:
                                        start_path(P_SWOOP1, &aliens[i]);
                                        break;
                                    case 2:
                                        start_path(P_SWOOP2, &aliens[i]);
                                        break;
                                    case 3:
                                        start_path(P_ZIGZAG, &aliens[i]);
                                        break;
                                    case 4:
                                        start_path(P_LOOP2, &aliens[i]);
                                        break;
                                    case 5:
                                        start_path(P_SPIN, &aliens[i]);
                                        break;
                                    case 6:
                                        start_path(P_LEFTDIAG, &aliens[i]);
                                        break;
                                    case 7:
                                        start_path(P_RIGHTDIAG, &aliens[i]);
                                        break;
                                    default:
                                        aliens[i].steer = TURNSPEED;
                                        aliens[i].path = -1;
                                    }
                                    if((aliens[i].path < 0) || (aliens[i].steer < 0)) {
                                        goto newpath;
                                    }
                                }
                            } else {
                                if(random()%2) {
                                    aliens[i].dir++;
                                    if(aliens[i].dir > 15)
                                        aliens[i].dir = 0;
                                } else {
                                    aliens[i].dir--;
                                    if(aliens[i].dir < 0)
                                        aliens[i].dir = 15;
                                }
                                aliens[i].steer = TURNSPEED;
                            }
                        }
                    }
                    tc = TORPCHANCE - level/2 - weapon*5;
                    if(tc < 35) tc = 35;

                    if(numetorps < maxetorps && (!(random()%tc))) {
                        int xs, ys;
            
                        /* could aim better, not sure it should! */

                        if(aliens[i].x > plx + 200) {
                            xs = -3;
                        } else if(aliens[i].x > plx + 100) {
                            xs = -2;
                        } else if(aliens[i].x < plx - 200) {
                            xs = 3;
                        } else if(aliens[i].x < plx - 100) {
                            xs = 2;
                        } else {
                            xs = 0;
                        }
                        ys = (ETORPSPEED+level/5) - ABS(xs);
                        new_etorp(aliens[i].x, aliens[i].y, xs, ys);
                    }
                }
                W_DrawImage(baseWin, 
                            aliens[i].x-(aliens[i].shape->width/2), 
                            aliens[i].y-(aliens[i].shape->height/2),
                            aliens[i].dir < 0 ? 0 : aliens[i].dir, aliens[i].shape, W_Green);
            } else {  /* paused */
                W_DrawImage(baseWin, 
                            aliens[i].x-(aliens[i].shape->width/2), 
                            aliens[i].y-(aliens[i].shape->height/2),
                            aliens[i].dir < 0 ? 0 : aliens[i].dir, aliens[i].shape, W_Green);
            }
        }
    }
    if(livecount == 0 && !paused) {
        starspeed++;
#ifdef SOUND
        if(starspeed == 2)
            play_sound(SND_WARP);
#endif
        if(starspeed >= 120) {
            starspeed = -20;
        } else if(starspeed == 1) {
            init_aliens(++level);
            gotlemon = 0;
            starspeed = 1;
            numtorps=0;
        }
    }
}


/*------------------player----------------*/
static void init_player()
{
    int i;
    for(i=0;i<MAXTORPS;i++)
        torps[i].alive=0;
}

static void new_torp(int x, int y, int xs, int ys)
{
    int i;

    for(i=0;i<maxtorps;i++) {
        if(!torps[i].alive) {
            torps[i].x = x;
            torps[i].y = y;
            torps[i].alive = 1;
            torps[i].xspeed = xs;
            torps[i].yspeed = ys;
            numtorps++;
#ifdef SOUND
            play_sound(SND_FIRETORP);
#endif
            return;
        }
    }
}

static void undo_torps()
{
    int i;

    for(i=0;i<MAXTORPS;i++) {
        if(torps[i].alive) {
            W_CacheClearArea(baseWin, 
                             torps[i].x-(playerTorp->width/2), 
                             torps[i].y-(playerTorp->height/2), 
                             playerTorp->width, playerTorp->height+1);
        }
    }
}

static void do_torps()
{
    int i,j,k, ne;

    for(i=0;i<MAXTORPS;i++) {
        if(torps[i].alive) {
            if(!paused) {
                torps[i].y += torps[i].yspeed;
                torps[i].x += torps[i].xspeed;
                torps[i].frame++;
                for(j=0;j<MAXALIENS;j++) {
                    if(aliens[j].alive && !aliens[j].dying &&
                       (ABS(torps[i].x - aliens[j].x) < 8) &&
                       ((ABS(torps[i].y - aliens[j].y) < 8) ||
                        (ABS((torps[i].y + torps[i].yspeed/2)-aliens[j].y) < 8))) {
                        aliens[j].dying = 1;
                        if(aliens[j].dir >= 0)
                            attacking--;
            
                        torps[i].alive=0;
                        numtorps--;
                        if(j >= 10) {
                            if(aliens[j].dir < 0)
                                score += 50;
                            else {
                                score += (6-(j/10))*100;
                                if(!(random()%(gotlemon ? 3 : PRIZECHANCE)))
                                    new_prize(aliens[j].x, aliens[j].y);
                            }
                            new_explosion(aliens[j].x, aliens[j].y, 0);
                        } else {
                            if(aliens[j].dir < 0)
                                score += 200;
                            else {
                                ne=0; /* count how many escorts */
                                for(k = j+9;k < j+12; k++) {
                                    if(aliens[k].escorting == j)
                                        ne++;
                                }                   
                                score_flagship(aliens[j].x, aliens[j].y, ne);
                            }
                            new_explosion(aliens[j].x, aliens[j].y, 1);
                        }
                        goto skip;
                    }
                }
                if(torps[i].y < -torps[i].yspeed || 
                   torps[i].x < ABS(torps[i].xspeed) || 
                   torps[i].x > WINWIDTH-ABS(torps[i].xspeed)) {
                    torps[i].alive = 0;
                    numtorps--;
                } else
                    W_DrawImage(baseWin, 
                                torps[i].x-(playerTorp->width/2), 
                                torps[i].y-(playerTorp->height/2), 
                                torps[i].frame, playerTorp, W_Red);
            skip: ;
            } else {/* paused */
                W_DrawImage(baseWin, 
                            torps[i].x-(playerTorp->width/2), 
                            torps[i].y-(playerTorp->height/2), 
                            torps[i].frame, playerTorp, W_Red);
            }
        }
    }
}

static void do_etorps()
{
    struct torp *t = first_etorp, *nextt;

    while(t) {
        nextt=t->next;
        if(t->alive) {
            if(!paused) {
                t->y+=t->yspeed;
                t->x+=t->xspeed;
                t->frame++;
                if(t->y > WINHEIGHT || t->x < 0 || t->x > WINWIDTH) {
                    if(t->next)
                        t->next->prev = t->prev;
                    if(t->prev)
                        t->prev->next = t->next;
                    if(t == first_etorp)
                        first_etorp = t->next;
                    free(t);
                    numetorps--;
                } else if(!pldead && !plflash && !plshield &&
                          (ABS(t->x - plx) < 8) &&
                          (ABS(t->y - (WINHEIGHT - (int)playerShip->height / 2)) < 8)) { /* DEAD! */
                    pldead = 1;
                    new_explosion(plx, WINHEIGHT - playerShip->height/2, 2);
                } else {
                    W_DrawImage(baseWin, 
                                t->x-(enemyTorp->width/2), 
                                t->y-(enemyTorp->height/2), 
                                t->frame, enemyTorp, W_Red);
                }
            } else {
                W_DrawImage(baseWin, 
                            t->x-(enemyTorp->width/2), 
                            t->y-(enemyTorp->height/2), 
                            t->frame, enemyTorp, W_Red);
            }
        }
        t = nextt;
    }
}

static void undo_etorps()
{
    struct torp *t = first_etorp;

    while(t) {
        W_CacheClearArea(baseWin, 
                         t->x-(enemyTorp->width/2), 
                         t->y-(enemyTorp->width/2), 
                         enemyTorp->width, enemyTorp->height);
        t=t->next;
    }
}

static void undo_player()
{
    int y = WINHEIGHT - playerShip->height, h = playerShip->height;
    if(plshield) {
        y -= 3;
        h += 3;
    }
    W_CacheClearArea(baseWin, plx-(playerShip->width/2), y,
                     playerShip->width, h);
}

static void do_player(mx, my, but)
int mx, my, but;
{
    static int torpok;
#ifdef ENABLE_MACHINE_GUN
    static int shotside = 0;
#endif
    W_Event wev;
    static int keys = 0;

    if(gameOver) {
        while(W_EventsPending()) {
            W_NextEvent(&wev);
        
	    if(gameOver)
	      mouseControl = 1;

            switch(wev.type) {
            case W_EV_KEY:
                if(score_key(&wev))
                    continue;
                switch(wev.key) {
                case 'q':
                case 'Q':
                case 256+'Q':   //369:
                case 256+'q':   //337:
                    xgal_exit(0);
                    break;
                case 'm':
                case 256+'m':    //365:
                    mouseControl = 2;
                    break;
                 case 'k':
                 case 256+'k':   //363:
                     mouseControl = 0;
                     W_UngrabPointer();
                     break;
                case '\r'+256:
                    W_ToggleFullscreen(shellWin);
                    fullscreen = !fullscreen;
                    if (fullscreen)
                        W_BlankCursor(baseWin);
                    else
                        W_RevertCursor(baseWin);
                    return;
                    break;
#ifdef SOUND
                case 's':       /* toggle sound on the title screen */
                case 256+'s':
                    playSounds = !playSounds;
                    return;     /* this key must not start the game */
                    break;
#endif
		default:
/*printf ("1keyevent %d\n", wev.key); */
                    return;     /* unhandled key must not cause any action */
		    break;
                }
                if(mouseControl < 2)
                    mouseControl = 0;		
            case W_EV_BUTTON:
                if(!getting_name) {
                    if(mouseControl)
                        W_GrabPointer(baseWin);
                    gameOver = 0;
                    maxtorps = MINTORPS;
                    weapon = 0;
                    movespeed = MINSPEED;
                    ships=2;
#ifdef ACTIVATED_SHIELD
		    shieldsleft = STARTSHIELDS;
		    shieldon = 0;
#else
		    shieldsleft = 0;
#endif
                    level=startLevel;  /* change made here */
                    init_aliens(level);
                    gotlemon = 0;
                    pldead = 0;
                    score = 0;
                    nextBonus = 20000;
                    plx = WINWIDTH/2;
                    W_ClearWindow(baseWin);
                }
                break;
            case W_EV_EXPOSE:
	    	if (wev.Window == shellWin)
		    draw_score();
		break;
            default:
/*printf ("2keyevent %d\n", wev.key);*/
              break;
            }
        }
        return;
    }

    while(W_EventsPending()) {
        W_NextEvent(&wev);

        switch(wev.type) {
        case W_EV_KEY_OFF:
            switch(wev.key) {
            case 'f'+128:
                keys &= ~(RIGHTKEY);
                break;
            case 'b'+128:
                keys &= ~(LEFTKEY);
                break;
            case ' ':
            case 256+' ':       //288:
                keys &= ~(FIREKEY);
                break;
#ifdef ACTIVATED_SHIELD
	    case 'x':
            case 256+'x':
		shieldsleft += plshield;
		plshield = 0;
		shieldon = 0;
		break;
#endif
            }
            break;
        case W_EV_KEY:
            switch(wev.key) {
            case 'f'+128:
                keys |= RIGHTKEY;
                break;
            case 'b'+128:
                keys |= LEFTKEY;
                break;
            case ' ':
            case 256+' ':       //288:
                keys |= FIREKEY;
                break;
            case 'k':
            case 256+'k':       //363:
                mouseControl = 0;
                W_UngrabPointer();
                break;
            case 'm':
            case 256+'m':       //365:
                mouseControl = 1;
                W_GrabPointer(baseWin);
                break;
#ifdef ACTIVATED_SHIELD
	    case 'x':
            case 256+'x':
		plshield += shieldsleft;
		shieldsleft = 0;
		shieldon = 1;
		break;
#endif
            case 'q':
            case 256+'q':       //369:
                if(!pldead  && !paused) {
                    new_explosion(plx, WINHEIGHT - ((playerShip->height)/2), 2);
                    ships = 0;
                    pldead = 1;
                }
                break;
            case 'Q':
            case 256+'Q':       //337:
                xgal_exit(0);
                break;
            case 'b':
            case 256+'b':       //354:
                W_Buffer(baseWin, !W_IsBuffered(baseWin));
                W_ClearWindow(baseWin);
                break;
            case 'p':
            case 256+'p':       //368:
                paused=!paused;
                if(!paused) {
                    undo_pause();
                    if(mouseControl)
                        W_GrabPointer(baseWin);
                } else
                    W_UngrabPointer();
                break;
#ifdef SOUND
            case 's':
            case 256+'s':       //371:
                playSounds = !playSounds;
                break;
#endif
#ifdef IM_A_BIG_FAT_CHEATER
            case 'i':
            case 256+'i':       //361:
                if(plflash >= 0)
                    plflash = -2;
                else
                    plflash = 0;
                break;
            case 'l':
            case 256+'l':       //364:
                {
                    int i;
                    for(i=0;i<MAXALIENS;i++)
                        aliens[i].alive=0;
                    if(starspeed != 1)
                        level++;
                }
                break;
            case 'c':
            case 256+'c':       //355:
                score+= BONUSSHIPSCORE;
                break;
            case 'h':
            case 256+'h':       //360:
                plshield = SHIELDTIME;
#ifdef SOUND
                play_sound(SND_SHIELD);
#endif
                break;
            case 'w':
            case 256+'w':       //375:
                weapon++;
                if(weapon == NUMWEAPONS)
                    weapon=0;
                break;
            case 't':
            case 256+'t':       //372:
                maxtorps++;
                if(maxtorps > MAXTORPS)
                    maxtorps = MINTORPS;
                break;
#endif /* IM_A_BIG_FAT_CHEATER */
            case '\r'+256:
                W_ToggleFullscreen(shellWin);
                fullscreen = !fullscreen;
                if (fullscreen)
                    W_BlankCursor(baseWin);
                else
                    W_RevertCursor(baseWin);
                break;
            default:
/*printf ("3keyevent %d\n", wev.key);*/
                break;
            }
        case W_EV_EXPOSE:
	    if (wev.Window == shellWin)
	    	draw_score();
	    break;
        }
    }

    if(!paused) {
        torpok--;

        if((!mouseControl && ! js_device) ||
	   (js_device && mx == plx && but == 0)) {
            if(keys & LEFTKEY)
                mx = 0;
            else if(keys & RIGHTKEY)
                mx = WINWIDTH;
            else
                mx = plx;
        
            if(keys & FIREKEY)
                but = W_LBUTTON;
            else
                but = 0;
        }
        
        if(pldead) {
            pldead++;
            if(pldead >= 100) {
                if(ships<=0) {
                    gameOver = 1;
                    W_UngrabPointer();
                    if(check_score(score)) {
#ifdef USE_REAL_NAMES
                        add_score(getUsersFullName(), score);
                        title_page = 1; pagetimer = 300;
#else
                        getting_name = 1;
#endif
                    }
                } else {
#ifdef DISABLE_RESET_ON_DEATH
                    ships--;
                    maxtorps--;
                    if (maxtorps < MINTORPS)
                    	maxtorps = MINTORPS;
                    switch (weapon)
                    {
                    	case SINGLESHOT:
                    		if (maxtorps < 3)
                    		{
                    			maxtorps = 3;
                    			weapon = SINGLESHOT;
                    		}
                    		break;
                    	case DOUBLESHOT:
	                    	if (maxtorps < 4)
                    		{
                    			maxtorps = 4;
                    		}
        					break;
                   		case SPREADSHOT:
                    		if (maxtorps < 5)
                    		{
                    			maxtorps = 5;
                    		}
                    		break;
						case TRIPLESHOT:
							if (maxtorps < 6)
                    		{
                    			maxtorps = 6;
                    		}
                    		break;
                    	case MACHINEGUN:
                    		if (maxtorps < 3)
                    		{
                    			maxtorps = 3;
                    		}
                    		break;
		}
#else
                    ships--;
                    maxtorps = MINTORPS;
                    weapon = 0;
                    movespeed = MINSPEED;
#endif /* DISABLE_RESET_ON_DEATH */
                    pldead = 0;
                    plflash = 50;
                    plx = WINWIDTH/2;
                }
            }
            return;
        }

        if(but && torpok <= 0 && (starspeed == 1)) {
            switch(weapon) {
            case SINGLESHOT:
                if(numtorps < maxtorps)
                    new_torp(plx, WINHEIGHT - playerShip->height, 0, -TORPSPEED);
		    torpok = TORPDELAY;
                break;
            case DOUBLESHOT:
                if(numtorps < maxtorps-1) {
                    new_torp(plx-5, WINHEIGHT - playerShip->height, 0, -TORPSPEED);
                    new_torp(plx+5, WINHEIGHT - playerShip->height, 0, -TORPSPEED);
		    torpok = TORPDELAY;
                }
                break;
            case TRIPLESHOT:
                if(numtorps < maxtorps-2) {
                    new_torp(plx-5, WINHEIGHT - playerShip->height, -2, 1-TORPSPEED);
                    new_torp(plx,   WINHEIGHT - playerShip->height, 0,   -TORPSPEED);
                    new_torp(plx+5, WINHEIGHT - playerShip->height, 2, 1-TORPSPEED);
		    torpok = TORPDELAY;
                }
                break;
#ifdef ENABLE_SPREAD_SHOT
            case SPREADSHOT:
            	if (numtorps == 0)
                {
			if ((maxtorps % 2) == 1)
				new_torp(plx, WINHEIGHT - playerShip->height, 0, -TORPSPEED*1.15);
			else
			{
				new_torp(plx - 5, WINHEIGHT - playerShip->height, 0, -TORPSPEED*1.15);
				new_torp(plx + 5, WINHEIGHT - playerShip->height, 0, -TORPSPEED*1.15);
			}
			if (maxtorps > 2)
			{
				new_torp(plx, WINHEIGHT - playerShip->height - 15, -2, -TORPSPEED*1.15);
				new_torp(plx, WINHEIGHT - playerShip->height - 15, 2, -TORPSPEED*1.15);
			}
			if (maxtorps > 4)
			{
				new_torp(plx, WINHEIGHT - playerShip->height - 25, -4, -TORPSPEED*1.15);
				new_torp(plx, WINHEIGHT - playerShip->height - 25, 4, -TORPSPEED*1.15);
			}
			if (maxtorps > 6)
			{
				new_torp(plx, WINHEIGHT - playerShip->height - 35, -6, -TORPSPEED*1.15);
				new_torp(plx, WINHEIGHT - playerShip->height - 35, 6, -TORPSPEED*1.15);
			}
			if (maxtorps > 8)
			{
				new_torp(plx, WINHEIGHT - playerShip->height - 50, -8, -TORPSPEED*1.15);    
				new_torp(plx, WINHEIGHT - playerShip->height - 50, 8, -TORPSPEED*1.15);
			}
			if (maxtorps > 10)
			{
				new_torp(plx, WINHEIGHT - playerShip->height - 60, -10, -TORPSPEED*1.15);
				new_torp(plx, WINHEIGHT - playerShip->height - 60, 10, -TORPSPEED*1.15);
                        }
                        torpok = TORPDELAY;
                 }
		 break;
#endif /* ENABLE_SPREAD_SHOT */
#ifdef ENABLE_MACHINE_GUN
            case MACHINEGUN:
                 if(numtorps < maxtorps)
                 {
			shotside = (shotside == -15) ? 15 : -15;
			new_torp(plx + shotside, WINHEIGHT - playerShip->height, 0, -TORPSPEED * 1.3);
			torpok = TORPDELAY - 2;
		}
		break;
#endif /* ENABLE_MACHINE_GUN */
	    }
	}


        if(!but)
            torpok=0;

        if((mx/movespeed) > (plx/movespeed))
            plx+=movespeed;
        else if((mx/movespeed) < (plx/movespeed))
            plx-=movespeed;
#ifdef ENABLE_SHIP_WRAP

        if(plx < 10)
        	plx=WINWIDTH - 10;
        if(plx > WINWIDTH - 10)
			plx=10;
#else
  
    
        if(plx < playerShip->width/2)
            plx=playerShip->width/2;
        if(plx> WINWIDTH - playerShip->width/2)
            plx=WINWIDTH - playerShip->width/2;
#endif
    
        if(plflash > 0)
            plflash--;
        if(!(plflash % 2))
            W_DrawImage(baseWin, plx-(playerShip->width/2), WINHEIGHT - playerShip->height, counter, playerShip, W_Red);
        if(plshield > 0)
            plshield--;
        if(plshield && ((plshield > SHIELDTIME/4) || plshield % 2)) {
            W_DrawImage(baseWin, plx-(shieldImage->width/2), WINHEIGHT - shieldImage->height - 3, 0, shieldImage,
                        W_Cyan);
        }
    } else if (!pldead) { /* paused */
        W_DrawImage(baseWin, plx-(playerShip->width/2), WINHEIGHT - playerShip->height, counter, playerShip, W_Red);
    }
}

int
main(argc, argv)
int     argc;
char  **argv;
{
    int ac;
    char *dpyname = 0;
    int mx, my, but;
    int start_fullscreen = 1;
   
    for(ac = 1; ac < argc; ac++) {
        if(*argv[ac] == '-') {
            int w, h;
            if(strcmp(argv[ac], "-scores") == 0) {
                print_scores();
                exit(0);
            }
            if(strcmp(argv[ac], "-display") == 0 && (ac+1 < argc)) {
                dpyname = argv[ac+1];
                ac++;
            } else if (strcmp(argv[ac], "-b") == 0) {
                useBuffered = !useBuffered;
            } else if (strcmp(argv[ac], "-keyboard") == 0) {
                mouseControl = 0;
            } else if (strcmp(argv[ac], "-mouse") == 0) {
                mouseControl = 1;
#ifdef SOUND
            } else if (strcmp(argv[ac], "-nosound") == 0) {
                playSounds = 0;
#endif
	/* '-level' option defined here */
            } else if (strcmp(argv[ac], "-level") == 0 && (ac+1 < argc) 
                       && atoi(argv[ac+1]) >= 1) 
            {
                int nlev;
                nlev = atoi(argv[ac+1]); 
                if (nlev > 15 ) nlev = 15;
                startLevel = nlev;
                ac++;
            } else if (strcmp(argv[ac], "-nostars") == 0) {
                wantStars = 0;
            } else if (strcmp(argv[ac], "-nouseor") == 0) {
                useOR = 0;
            } else if (strcmp(argv[ac], "-noclipmask") == 0) {
                useClipMask = 0;
            } else if (strcmp(argv[ac], "-window") == 0) {
                start_fullscreen = 0;
            } else if ((strcmp(argv[ac], "-winsize") == 0) && (++ac < argc) &&
                       (sscanf(argv[ac], "%dx%d", &w, &h) == 2)) {
                WINWIDTH  = w;
                WINHEIGHT = h;
            } else {
                print_usage();
                exit(0);
            }
        } else {
            print_usage();
            exit(0);
        }
    }
        
    W_Initialize(dpyname);

    backColor = W_Black;
    WINHEIGHT -= (W_Textheight+1);
    shellWin = W_MakeWindow("XGalaga", 0, 0, WINWIDTH, WINHEIGHT + W_Textheight+1, 0, "tiny", 0, W_White);
    baseWin = W_MakeWindow("", 0, W_Textheight+1, WINWIDTH, WINHEIGHT, shellWin, "tiny", 0, W_White);
    W_Buffer(shellWin, 0);
    W_MapWindow(shellWin);
    W_MapWindow(baseWin);
    if (start_fullscreen)
    {
      W_ToggleFullscreen(shellWin);
      W_BlankCursor(baseWin);
      fullscreen = 1;
    }

    /*    W_AutoRepeatOff();*/

    W_Flush();

    W_SetImageDir(IMAGEDIR);

    playerShip = getImage(I_PLAYER1);
    playerTorp = getImage(I_MTORP);
    enemyTorp = getImage(I_ETORP);
    shieldImage = getImage(I_SHIELD);

    level=startLevel;   /* change made here */

    load_scores();
    init_titles();
    init_player();
    init_stars();
    init_explosions();
    init_score();
    init_prizes();
#ifdef SOUND
    init_sound();       /* starts the soundserver */
#endif
    init_aliens(level);
    init_framerate();
#ifdef __linux__
    init_joystick();
#endif

    ships = 2;
    nextBonus = 20000;

    while(1) {
        counter++;

	/* For the benefit of unbuffered mode, the most important things are
	 * erased/redrawn closest together so they spend the least time blanked.
	 * player, aliens and etorps are most important for game play.
	 * pause, title and name are important in their modes and aren't done
	 * otherwise.
	 *
	 * The title, name, pause and score "extra ship" want to overlay
	 * everything else drawn, so they come last.
	 */
	undo_stars();
        undo_explosions();
        undo_prizes();
        undo_torps();
        undo_etorps();
        undo_aliens();
        undo_player();
        if(gameOver && getting_name) undo_name();
        if(paused) undo_pause();
        undo_score();

        W_FlushClearAreaCache(baseWin);

        do_etorps();
        do_player(mx, my, but);
        do_aliens();
        do_torps();
        do_prizes();
        do_explosions();
 	do_stars();
        do_score();
        if(gameOver) { do_title(); if(getting_name) do_name(); }
        if(paused) do_pause();

        W_DisplayBuffer(baseWin);

	/* This is an XSync style round trip to the server with the bonus of
	 * getting the mouse position.
	 * If the server can't draw at the UTIMER frame rate then this will be
	 * the only delay in the loop.
	 */
	W_GetMouse(baseWin, &mx, &my, &but);
#ifdef __linux__
	do_joystick(&mx, &my, &but);
#endif
        do_framerate();

    }
    return (0);
}


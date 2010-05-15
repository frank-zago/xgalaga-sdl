/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>
#include "Wlib.h"
#include "images.h"
#include "defs.h"
#include "data.h"
#include "struct.h"

int buttonDown = 0;

W_Window shellWin, baseWin;

W_Image *playerShip, *playerTorp, *enemyTorp, *shieldImage;

int score;
int paused;

int ships;

int level, metaLevel;
int startLevel=1;       /* default starting level defined here */

int nextBonus;

int gameOver=1;
int getting_name = 0;

int counter;

int mouseControl = 0;
int wantStars = 1;

/* Modified slightly to better fit on 800x600 in fullscreen mode */
int winwidth  = 468; /* was 400 */
int winheight = 596; /* was 511 */

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

int plx = 200;
int pldead = 0;

int movespeed = MINSPEED;

#ifdef SOUND
char *unixSoundPath=SOUNDDIR;
#ifndef NAS_SOUND
char *unixSoundDev=SOUNDDEV;
#endif
int playSounds=1;
#endif

int plshield = 0;
int shieldsleft = 0;
int shieldon = 0;

int title_page=0, pagetimer=300;

int gotlemon = 0;

struct alien aliens[MAXALIENS];

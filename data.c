/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <config.h>
#include <SDL/SDL.h>

#include "defs.h"
#include "data.h"
#include "struct.h"
#include "images.h"

int buttonDown = 0;

int score;

int ships;

int level, metaLevel;
int startLevel=1;       /* default starting level defined here */

int nextBonus;

int counter;

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

int plx;
int pldead = 0;

int movespeed = MINSPEED;

char *unixSoundPath=SOUNDDIR;
int playSounds=0;

int plshield = 0;
int shieldsleft = 0;
int shieldon = 0;

int title_page=0, pagetimer=300;

int gotlemon = 0;

struct alien aliens[MAXALIENS];

#if 0
const SDL_Color S_White  = { 0xFF, 0xFF, 0xFF, 0x00 };
const SDL_Color S_Green  = { 0x00, 0xFF, 0x00, 0x00 };
const SDL_Color S_Cyan   = { 0x00, 0xFF, 0xFF, 0x00 };
const SDL_Color S_Red    = { 0xFF, 0x00, 0x00, 0x00 };
const SDL_Color S_Yellow = { 0xFF, 0xFF, 0x00, 0x00 };
const SDL_Color S_Black  = { 0x00, 0x00, 0x00, 0x00 };
const SDL_Color S_Grey   = { 0xA9, 0xA9, 0xA9, 0x00 };
#endif

SFont_Font *fnt_reg_green;
SFont_Font *fnt_reg_cyan;
SFont_Font *fnt_reg_yellow;
SFont_Font *fnt_reg_red;
SFont_Font *fnt_reg_grey;

SFont_Font *fnt_big_red;

enum gstate gstate;

int fullscreen = 0;				/* windowed = 0, fullscreen != 0 */

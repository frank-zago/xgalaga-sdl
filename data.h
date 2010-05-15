#include "Wlib.h"
#include "struct.h"

extern W_Window gal, shellWin, baseWin;

extern char *imagedir;
extern int verbose_image_loading;
extern int forceMono;
extern int xpm, useOR, useClipMask, useBuffered;

extern W_Color backColor;

extern int buttonDown;
extern char *imagedirend;

extern W_Image *playerShip, *playerTorp, *enemyTorp, *shieldImage;

extern int score;
extern int paused;

extern int ships;
extern int shieldsleft;
extern int shieldon;

extern int level, metaLevel;
extern int startLevel;

extern int nextBonus;

extern int gameOver;
extern int getting_name;

extern int counter;

extern int mouseControl;
extern int wantStars;

extern int winwidth;
extern int winheight;

extern int alien_shape[];

extern int weapon;
extern int maxtorps, numtorps;

extern int plx;
extern int pldead;

extern int movespeed;

#ifdef SOUND
extern char *unixSoundPath;
#ifndef NAS_SOUND
extern char *unixSoundDev;
#endif
extern int playSounds;
#endif

extern int plshield;

extern int title_page, pagetimer;

extern int gotlemon;

extern struct alien aliens[];

extern struct js_state js_curstate;
extern int js_device;

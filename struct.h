#ifndef STRUCT_H
#define STRUCT_H

#include <SDL/SDL.h>

struct torp {
    struct torp *next, *prev;
    int alive, frame;
    int x, y;
    int xspeed, yspeed;
};

struct star {
    int x, y;
    int speed;
	Uint32 pixel;       /* pixel color */
};

struct alien {
    int x, y;
    int dir;
    int steer;
    int alive;
    int dying;
    int path, path_pos;
    int escorting;
    int entering, enterdelay;
    struct W_Image *shape;
};

struct explosion {
    struct explosion *next, *prev;
    int x, y;
    int frame;
    struct W_Image *shape;
};

struct score_bubble {
    struct score_bubble *next, *prev;
    int x, y;
    int count;
    struct W_Image *shape;
};

struct js_state {
    signed short but;
    signed long dir;
};

struct FNT_fixed {
	int width, height;
	unsigned char data[];
};

enum gstate {
	INTRO,			   /* Main screen */
	PLAYING,		   /* Playing */
	PAUSED,			   /* Paused (playing) */
	GETTING_NAME,	   /* Game over, getting name for new highscore */
};

#endif

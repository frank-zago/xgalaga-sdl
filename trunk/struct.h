#ifndef STRUCT_H
#define STRUCT_H

#include "Wlib.h"

struct torp {
    struct torp *next, *prev;
    int alive, frame;
    int x, y;
    int xspeed, yspeed;
};

struct star {
    int x, y;
    int speed;
    W_Color color;
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
    W_Image *shape;
};

struct explosion {
    struct explosion *next, *prev;
    int x, y;
    int frame;
    W_Image *shape;
};

struct score_bubble {
    struct score_bubble *next, *prev;
    int x, y;
    int count;
    W_Image *shape;
};

struct js_state {
    signed short but;
    signed long dir;
};

#endif

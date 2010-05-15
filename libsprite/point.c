#include "allincludes.h"

void
W_DrawPoint(window, x, y, color)
    W_Window window;
    int     x, y;
    W_Color color;
{
    Window  win;

#ifdef DEBUG
    printf("Point on %d\n", window);
#endif
    win = W_Void2Window(window)->drawable;
    XDrawPoint(W_Display, win, colortable[color].contexts[0], x, y);
}

static XPoint _pcache[NCOLORS][MAXCACHE];
static int _pcache_index[NCOLORS];

 void
FlushPointCache(win, color)
    Window  win;
    int     color;
{
    XDrawPoints(W_Display, win, colortable[color].contexts[0],
		_pcache[color], _pcache_index[color], CoordModeOrigin);
    _pcache_index[color] = 0;
}

void
W_CachePoint(window, x, y, color)
    W_Window window;
    int     x, y, color;
{
    Window  win = W_Void2Window(window)->drawable;
    register XPoint *p;

    if (_pcache_index[color] == MAXCACHE)
	FlushPointCache(win, color);

    p = &_pcache[color][_pcache_index[color]++];
    p->x = (short) x;
    p->y = (short) y;
}

void
W_FlushPointCaches(window)
    W_Window window;
{
    Window  win = W_Void2Window(window)->drawable;
    int i;
    for (i = 0; i < NCOLORS; i++) {
	if (_pcache_index[i])
	    FlushPointCache(win, i);
    }
}

